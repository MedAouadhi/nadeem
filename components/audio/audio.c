#include "audio.h"
#include "bsp.h"
#include "board_io.h"
#include "nadeem_events.h"
#include "smf/smf.h"

#include "esp_audio_simple_player.h"
#include "esp_audio_simple_player_advance.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/timers.h"

#include <string.h>

static const char *TAG = "audio";

/* PA amplifier enable pin on the TCA9555. */
#define PA_ENABLE_PIN IO_EXPANDER_PIN_NUM_8

/* ---- SMF state definitions ------------------------------------------- */

typedef enum {
    EV_PLAY,
    EV_STOP,
    EV_PAUSE,
    EV_RESUME,
    EV_NEXT,
    EV_PREV,
    EV_TRACK_DONE,
} audio_cmd_t;

typedef struct {
    audio_cmd_t cmd;
    nev_audio_play_t payload;
} audio_msg_t;

static struct smf_ctx    s_smf_ctx;
static QueueHandle_t     s_cmd_q;
static audio_state_t     s_state;
static uint32_t          s_position_bytes;
static esp_asp_handle_t  s_player;
static uint8_t           s_volume = 60;
static char              s_current_path[128];

/* Forward-declared state array. */
enum { S_IDLE_IDX, S_LOADING_IDX, S_PLAYING_IDX, S_PAUSED_IDX, S_ERROR_IDX };
static const struct smf_state s_states[];

/* ---- PA control ------------------------------------------------------- */

static void pa_enable(bool on) {
    board_io_expander_set(PA_ENABLE_PIN, on ? 1 : 0);
    vTaskDelay(pdMS_TO_TICKS(10));
}

/* ---- ASP callbacks ---------------------------------------------------- */

static int asp_out_cb(uint8_t *data, int data_size, void *ctx) {
    (void)ctx;
    bsp_i2s_write((int16_t *)data, data_size, pdMS_TO_TICKS(500));
    s_position_bytes += data_size;
    return data_size;
}

static int asp_event_cb(esp_asp_event_pkt_t *event, void *ctx) {
    (void)ctx;
    if (event->type == ESP_ASP_EVENT_TYPE_STATE) {
        esp_asp_state_t st = 0;
        memcpy(&st, event->payload, event->payload_size);
        ESP_LOGI(TAG, "asp state → %d", (int)st);
        if (st == ESP_ASP_STATE_FINISHED || st == ESP_ASP_STATE_STOPPED) {
            audio_msg_t m = { .cmd = EV_TRACK_DONE };
            xQueueSend(s_cmd_q, &m, 0);
        }
    }
    return 0;
}

static void asp_init(void) {
    esp_asp_cfg_t cfg = {
        .in.cb       = NULL,
        .in.user_ctx = NULL,
        .out.cb      = asp_out_cb,
        .out.user_ctx = NULL,
    };
    esp_audio_simple_player_new(&cfg, &s_player);
    esp_audio_simple_player_set_event(s_player, asp_event_cb, NULL);
}

/* ---- SMF state handlers (obj is &s_smf_ctx) -------------------------- */

static void idle_entry(void *obj) {
    (void)obj;
    s_state = AUDIO_IDLE;
    s_position_bytes = 0;
    pa_enable(false);
    nev_audio_state_t st = { .state = AUDIO_IDLE, .pos_bytes = 0 };
    nadeem_events_post(NEV_AUDIO_STATE, &st, sizeof st, 0);
}

static void loading_entry(void *obj) {
    s_state = AUDIO_LOADING;
    pa_enable(true);
    esp_audio_simple_player_stop(s_player);
    esp_gmf_err_t err = esp_audio_simple_player_run(s_player, s_current_path, NULL);
    if (err != ESP_GMF_ERR_OK) {
        ESP_LOGE(TAG, "player run failed: %d", err);
        smf_set_state(SMF_CTX(obj), &s_states[S_ERROR_IDX]);
        return;
    }
    smf_set_state(SMF_CTX(obj), &s_states[S_PLAYING_IDX]);
}

static void playing_entry(void *obj) {
    (void)obj;
    s_state = AUDIO_PLAYING;
}

static void paused_entry(void *obj) {
    (void)obj;
    s_state = AUDIO_PAUSED;
    esp_audio_simple_player_pause(s_player);
    pa_enable(false);
}

static void error_entry(void *obj) {
    (void)obj;
    s_state = AUDIO_ERROR;
    pa_enable(false);
    nev_error_t err = { .source = 1, .code = -1 };
    nadeem_events_post(NEV_SYS_ERROR, &err, sizeof err, 0);
}

/* Common run step: drain one command and apply state-specific transitions. */
static void common_run(void *obj) {
    audio_msg_t m;
    if (xQueueReceive(s_cmd_q, &m, 0) != pdTRUE) return;

    switch (m.cmd) {
        case EV_PLAY:
            strncpy(s_current_path, m.payload.path, sizeof s_current_path - 1);
            s_current_path[sizeof s_current_path - 1] = '\0';
            s_position_bytes = m.payload.start_byte;
            smf_set_state(SMF_CTX(obj), &s_states[S_LOADING_IDX]);
            break;
        case EV_STOP:
            esp_audio_simple_player_stop(s_player);
            smf_set_state(SMF_CTX(obj), &s_states[S_IDLE_IDX]);
            break;
        case EV_PAUSE:
            if (s_state == AUDIO_PLAYING)
                smf_set_state(SMF_CTX(obj), &s_states[S_PAUSED_IDX]);
            break;
        case EV_RESUME:
            if (s_state == AUDIO_PAUSED) {
                pa_enable(true);
                esp_audio_simple_player_resume(s_player);
                smf_set_state(SMF_CTX(obj), &s_states[S_PLAYING_IDX]);
            }
            break;
        case EV_NEXT:
        case EV_PREV:
            /* Track change is handled by the semsem module which will post a
             * new EV_PLAY with the next path. Nothing else to do here. */
            break;
        case EV_TRACK_DONE:
            nadeem_events_post(NEV_AUDIO_TRACK_DONE, NULL, 0, 0);
            smf_set_state(SMF_CTX(obj), &s_states[S_IDLE_IDX]);
            break;
    }
}

static const struct smf_state s_states[] = {
    [S_IDLE_IDX]    = SMF_CREATE_STATE(idle_entry,    common_run, NULL, NULL, NULL),
    [S_LOADING_IDX] = SMF_CREATE_STATE(loading_entry, common_run, NULL, NULL, NULL),
    [S_PLAYING_IDX] = SMF_CREATE_STATE(playing_entry, common_run, NULL, NULL, NULL),
    [S_PAUSED_IDX]  = SMF_CREATE_STATE(paused_entry,  common_run, NULL, NULL, NULL),
    [S_ERROR_IDX]   = SMF_CREATE_STATE(error_entry,   common_run, NULL, NULL, NULL),
};

/* ---- Event-bus → queue bridge ---------------------------------------- */

static void on_audio_event(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg; (void)base;
    audio_msg_t m = {0};
    switch (id) {
        case NEV_AUDIO_PLAY:
            m.cmd = EV_PLAY;
            if (data) memcpy(&m.payload, data, sizeof m.payload);
            break;
        case NEV_AUDIO_STOP:   m.cmd = EV_STOP;   break;
        case NEV_AUDIO_PAUSE:  m.cmd = EV_PAUSE;  break;
        case NEV_AUDIO_RESUME: m.cmd = EV_RESUME; break;
        case NEV_AUDIO_NEXT:   m.cmd = EV_NEXT;   break;
        case NEV_AUDIO_PREV:   m.cmd = EV_PREV;   break;
        default: return;
    }
    xQueueSend(s_cmd_q, &m, 0);
}

/* Periodic state broadcast to whoever cares (led, semsem). */
static void state_tick(TimerHandle_t t) {
    (void)t;
    nev_audio_state_t st = {
        .state     = (uint8_t)s_state,
        .pos_bytes = s_position_bytes,
    };
    nadeem_events_post(NEV_AUDIO_STATE, &st, sizeof st, 0);
}

static void audio_task(void *arg) {
    (void)arg;
    smf_set_initial(&s_smf_ctx, &s_states[S_IDLE_IDX]);
    while (1) {
        smf_run_state(&s_smf_ctx);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/* ---- Public API ------------------------------------------------------- */

esp_err_t audio_start(void)
{
    s_cmd_q = xQueueCreate(16, sizeof(audio_msg_t));
    if (!s_cmd_q) return ESP_ERR_NO_MEM;
    asp_init();
    audio_set_volume(s_volume);

    nadeem_events_on(NEV_AUDIO_PLAY,   on_audio_event, NULL);
    nadeem_events_on(NEV_AUDIO_STOP,   on_audio_event, NULL);
    nadeem_events_on(NEV_AUDIO_PAUSE,  on_audio_event, NULL);
    nadeem_events_on(NEV_AUDIO_RESUME, on_audio_event, NULL);
    nadeem_events_on(NEV_AUDIO_NEXT,   on_audio_event, NULL);
    nadeem_events_on(NEV_AUDIO_PREV,   on_audio_event, NULL);

    TimerHandle_t t = xTimerCreate("audio_tick", pdMS_TO_TICKS(2000),
                                   pdTRUE, NULL, state_tick);
    xTimerStart(t, 0);

    BaseType_t ok = xTaskCreatePinnedToCore(audio_task, "audio_sm",
                                            4096, NULL, 5, NULL, 1);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

audio_state_t audio_current_state(void) { return s_state; }
uint32_t      audio_current_position(void) { return s_position_bytes; }

esp_err_t audio_set_volume(uint8_t vol)
{
    if (vol > 100) vol = 100;
    s_volume = vol;
    return bsp_set_volume(vol);
}

uint8_t audio_get_volume(void) { return s_volume; }
