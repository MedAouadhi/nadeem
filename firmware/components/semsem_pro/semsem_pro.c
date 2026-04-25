#include "semsem_pro.h"
#include "nadeem_events.h"
#include "nadeem_config.h"
#include "bsp.h"
#include "board_io.h"
#include "stats.h"
#include "smf/smf.h"

#include "esp_websocket_client.h"
#include "esp_log.h"
#include "esp_timer.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"

#include <string.h>
#include <stdio.h>

static const char *TAG = "pro";

/* 20ms chunks @ 16kHz mono PCM16 = 320 samples = 640 bytes. */
#define PRO_FRAME_SAMPLES 320
#define PRO_FRAME_BYTES   (PRO_FRAME_SAMPLES * 2)
#define PRO_RX_BUF_BYTES  16384   /* ~0.5 s of TTS buffer */

/* ---------- SMF states ------------------------------------------------ */

enum { P_IDLE, P_CONNECTING, P_STREAMING, P_CLOSING, P_ERROR };

typedef enum { EV_ENTER, EV_EXIT, EV_WS_CONNECTED, EV_WS_DISCONNECTED, EV_WS_ERROR } pro_cmd_t;

typedef struct {
    pro_cmd_t        cmd;
    nev_pro_enter_t  enter;
} pro_msg_t;

static struct smf_ctx              s_ctx;
static QueueHandle_t               s_q;
static esp_websocket_client_handle_t s_ws;
static StreamBufferHandle_t        s_rx_stream;
static TaskHandle_t                s_mic_task;
static TaskHandle_t                s_speaker_task;
static bool                        s_mic_run;
static bool                        s_spk_run;
static uint64_t                    s_session_start_us;
static nev_pro_enter_t             s_enter;

static const struct smf_state s_states[];

/* ---------- Tasks ----------------------------------------------------- */

static void mic_task(void *arg) {
    (void)arg;
    int mic_ch = bsp_mic_channels();
    if (mic_ch <= 0) mic_ch = 4;
    size_t raw_sz = PRO_FRAME_SAMPLES * sizeof(int16_t) * mic_ch;
    int16_t *raw = malloc(raw_sz);
    int16_t *mono = malloc(PRO_FRAME_BYTES);
    if (!raw || !mono) { vTaskDelete(NULL); return; }

    while (s_mic_run) {
        if (bsp_i2s_read_mic(false, raw, raw_sz) == ESP_OK) {
            for (int i = 0; i < PRO_FRAME_SAMPLES; i++) mono[i] = raw[i * mic_ch];
            if (s_ws && esp_websocket_client_is_connected(s_ws)) {
                esp_websocket_client_send_bin(s_ws, (const char *)mono, PRO_FRAME_BYTES,
                                              pdMS_TO_TICKS(100));
            }
        }
    }
    free(raw); free(mono);
    s_mic_task = NULL;
    vTaskDelete(NULL);
}

static void speaker_task(void *arg) {
    (void)arg;
    int16_t buf[PRO_FRAME_SAMPLES];
    while (s_spk_run) {
        size_t got = xStreamBufferReceive(s_rx_stream, buf, PRO_FRAME_BYTES,
                                          pdMS_TO_TICKS(200));
        if (got > 0) bsp_i2s_write(buf, (int)got, pdMS_TO_TICKS(100));
    }
    s_speaker_task = NULL;
    vTaskDelete(NULL);
}

/* ---------- WebSocket client ----------------------------------------- */

static void ws_event(void *handler_args, esp_event_base_t base,
                     int32_t event_id, void *event_data) {
    (void)handler_args; (void)base;
    esp_websocket_event_data_t *d = (esp_websocket_event_data_t *)event_data;
    pro_msg_t m = {0};
    switch (event_id) {
        case WEBSOCKET_EVENT_CONNECTED:
            ESP_LOGI(TAG, "ws connected");
            m.cmd = EV_WS_CONNECTED;
            xQueueSend(s_q, &m, 0);
            break;
        case WEBSOCKET_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "ws disconnected");
            m.cmd = EV_WS_DISCONNECTED;
            xQueueSend(s_q, &m, 0);
            break;
        case WEBSOCKET_EVENT_DATA:
            if (d->op_code == 0x2 /* binary */) {
                xStreamBufferSend(s_rx_stream, d->data_ptr, d->data_len,
                                  pdMS_TO_TICKS(20));
            } else if (d->op_code == 0x1 /* text */) {
                /* JSON control frame — minimal handling: listening/speaking. */
                if (d->data_ptr && d->data_len > 0) {
                    if (strnstr(d->data_ptr, "speaking", d->data_len))
                        nadeem_events_post(NEV_SYS_PRO_SPEAKING, NULL, 0, 0);
                    else if (strnstr(d->data_ptr, "listening", d->data_len))
                        nadeem_events_post(NEV_SYS_PRO_LISTENING, NULL, 0, 0);
                }
            }
            break;
        case WEBSOCKET_EVENT_ERROR:
            m.cmd = EV_WS_ERROR;
            xQueueSend(s_q, &m, 0);
            break;
        default: break;
    }
}

static esp_err_t open_ws_for(const nev_pro_enter_t *e) {
    char base[128] = {0};
    if (nadeem_config_get_backend_url(base, sizeof base) != ESP_OK || base[0] == '\0') {
        ESP_LOGE(TAG, "no backend URL");
        return ESP_ERR_INVALID_STATE;
    }

    /* Swap https:// → wss:// (or http → ws) for the WebSocket URI. */
    char uri[256];
    if (strncmp(base, "https://", 8) == 0) {
        snprintf(uri, sizeof uri, "wss://%s/chat?role=%s", base + 8, e->role[0] ? e->role : "default");
    } else if (strncmp(base, "http://", 7) == 0) {
        snprintf(uri, sizeof uri, "ws://%s/chat?role=%s", base + 7, e->role[0] ? e->role : "default");
    } else {
        snprintf(uri, sizeof uri, "%s/chat?role=%s", base, e->role[0] ? e->role : "default");
    }

    esp_websocket_client_config_t cfg = {
        .uri = uri,
        .reconnect_timeout_ms = 2000,
        .network_timeout_ms   = 5000,
    };
    s_ws = esp_websocket_client_init(&cfg);
    if (!s_ws) return ESP_FAIL;
    esp_websocket_register_events(s_ws, WEBSOCKET_EVENT_ANY, ws_event, NULL);
    return esp_websocket_client_start(s_ws);
}

static void close_ws(void) {
    if (s_ws) {
        esp_websocket_client_stop(s_ws);
        esp_websocket_client_destroy(s_ws);
        s_ws = NULL;
    }
}

/* ---------- SMF states ----------------------------------------------- */

static void idle_entry(void *obj) { (void)obj; }

static void connecting_entry(void *obj) {
    if (open_ws_for(&s_enter) != ESP_OK) {
        smf_set_state(SMF_CTX(obj), &s_states[P_ERROR]);
    }
}

static void streaming_entry(void *obj) {
    (void)obj;
    s_session_start_us = esp_timer_get_time();
    s_rx_stream = xStreamBufferCreate(PRO_RX_BUF_BYTES, PRO_FRAME_BYTES);
    s_mic_run = true;
    s_spk_run = true;
    xTaskCreatePinnedToCore(mic_task,     "pro_mic", 4096, NULL, 6, &s_mic_task,     1);
    xTaskCreatePinnedToCore(speaker_task, "pro_spk", 4096, NULL, 6, &s_speaker_task, 1);
    nadeem_events_post(NEV_SYS_PRO_LISTENING, NULL, 0, 0);
}

static void closing_entry(void *obj) {
    s_mic_run = false;
    s_spk_run = false;
    close_ws();
    if (s_rx_stream) { vStreamBufferDelete(s_rx_stream); s_rx_stream = NULL; }

    /* Record stats for this session. */
    char uid_hex[24];
    nadeem_uid_to_hex(&s_enter.uid, uid_hex, sizeof uid_hex);
    uint64_t dur_ms = (esp_timer_get_time() - s_session_start_us) / 1000;
    stats_record_pro_session(uid_hex, (uint32_t)dur_ms);

    smf_set_state(SMF_CTX(obj), &s_states[P_IDLE]);
}

static void error_entry(void *obj) {
    nev_error_t e = { .source = 3, .code = -1 };
    nadeem_events_post(NEV_SYS_ERROR, &e, sizeof e, 0);
    smf_set_state(SMF_CTX(obj), &s_states[P_CLOSING]);
}

static void common_run(void *obj) {
    pro_msg_t m;
    if (xQueueReceive(s_q, &m, 0) != pdTRUE) return;
    switch (m.cmd) {
        case EV_ENTER:
            s_enter = m.enter;
            smf_set_state(SMF_CTX(obj), &s_states[P_CONNECTING]);
            break;
        case EV_WS_CONNECTED:
            smf_set_state(SMF_CTX(obj), &s_states[P_STREAMING]);
            break;
        case EV_WS_DISCONNECTED:
        case EV_WS_ERROR:
        case EV_EXIT:
            smf_set_state(SMF_CTX(obj), &s_states[P_CLOSING]);
            break;
    }
}

static const struct smf_state s_states[] = {
    [P_IDLE]       = SMF_CREATE_STATE(idle_entry,       common_run, NULL, NULL, NULL),
    [P_CONNECTING] = SMF_CREATE_STATE(connecting_entry, common_run, NULL, NULL, NULL),
    [P_STREAMING]  = SMF_CREATE_STATE(streaming_entry,  common_run, NULL, NULL, NULL),
    [P_CLOSING]    = SMF_CREATE_STATE(closing_entry,    common_run, NULL, NULL, NULL),
    [P_ERROR]      = SMF_CREATE_STATE(error_entry,      common_run, NULL, NULL, NULL),
};

/* ---------- Event handlers ------------------------------------------- */

static void on_event(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg; (void)base;
    pro_msg_t m = {0};
    if (id == NEV_SYS_MODE_PRO_ENTER) {
        m.cmd = EV_ENTER;
        if (data) memcpy(&m.enter, data, sizeof m.enter);
    } else if (id == NEV_SYS_MODE_PRO_EXIT || id == NEV_HMI_SEMSEM_REMOVED) {
        m.cmd = EV_EXIT;
    } else {
        return;
    }
    xQueueSend(s_q, &m, 0);
}

static void pro_task(void *arg) {
    (void)arg;
    smf_set_initial(&s_ctx, &s_states[P_IDLE]);
    while (1) {
        smf_run_state(&s_ctx);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

esp_err_t semsem_pro_start(void) {
    s_q = xQueueCreate(8, sizeof(pro_msg_t));
    if (!s_q) return ESP_ERR_NO_MEM;

    nadeem_events_on(NEV_SYS_MODE_PRO_ENTER,  on_event, NULL);
    nadeem_events_on(NEV_SYS_MODE_PRO_EXIT,   on_event, NULL);
    nadeem_events_on(NEV_HMI_SEMSEM_REMOVED,  on_event, NULL);

    BaseType_t ok = xTaskCreatePinnedToCore(pro_task, "pro_sm", 4096, NULL, 5, NULL, 1);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
