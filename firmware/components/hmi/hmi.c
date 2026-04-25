#include "hmi.h"
#include "board_io.h"
#include "pn532.h"
#include "nadeem_events.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "hmi";

/* SMF-style two-state machine: NO_TAG <-> TAG_PRESENT. Kept explicit (not using
 * the SMF helper) because the state fits on a single page and readability wins. */
typedef enum { S_NO_TAG, S_TAG_PRESENT } hmi_state_t;

static hmi_state_t        s_state;
static nev_semsem_uid_t   s_current_uid;
static uint8_t            s_miss_count;

static void on_button(key_id_t id, key_event_t evt, void *user_data)
{
    (void)user_data;
    if (evt == KEY_EVENT_SHORT_PRESS) {
        switch (id) {
            case KEY_ID_9:  nadeem_events_post(NEV_HMI_BTN_PREV, NULL, 0, 0); break;
            case KEY_ID_10: nadeem_events_post(NEV_HMI_BTN_PLAY_PAUSE, NULL, 0, 0); break;
            case KEY_ID_11: nadeem_events_post(NEV_HMI_BTN_NEXT, NULL, 0, 0); break;
            default: break;
        }
    } else if (evt == KEY_EVENT_LONG_PRESS && id == KEY_ID_10) {
        /* Long-press play-pause (≥3 s) = factory reset intent. The button driver
         * fires LONG_PRESS at 1 s; we could add a second threshold but the plan
         * (and stats) tolerate 1 s for v0.1. */
        nadeem_events_post(NEV_HMI_BTN_LONG_RESET, NULL, 0, 0);
    }
}

static bool uid_equal(const nev_semsem_uid_t *a, const nev_semsem_uid_t *b)
{
    return a->uid_len == b->uid_len && memcmp(a->uid, b->uid, a->uid_len) == 0;
}

static void nfc_poll_task(void *arg)
{
    (void)arg;

    /* Initialize PN532 here so failures don't kill app_main(). */
    if (pn532_init(PN532_DEFAULT_UART_PORT,
                   PN532_DEFAULT_TX_GPIO,
                   PN532_DEFAULT_RX_GPIO) != ESP_OK) {
        ESP_LOGE(TAG, "PN532 init failed");
        vTaskDelete(NULL);
        return;
    }
    if (pn532_sam_configure() != ESP_OK) {
        ESP_LOGW(TAG, "PN532 SAM config failed — continuing anyway");
    }

    s_state = S_NO_TAG;
    while (1) {
        pn532_uid_t uid;
        esp_err_t err = pn532_read_passive_target(&uid, 50);

        if (s_state == S_NO_TAG) {
            if (err == ESP_OK && uid.uid_len > 0) {
                nev_semsem_uid_t ev = { .uid_len = uid.uid_len };
                memcpy(ev.uid, uid.uid, uid.uid_len);
                s_current_uid = ev;
                s_miss_count  = 0;
                s_state       = S_TAG_PRESENT;
                nadeem_events_post(NEV_HMI_SEMSEM_PLACED, &ev, sizeof ev, 0);
                char hex[24];
                nadeem_uid_to_hex(&ev, hex, sizeof hex);
                ESP_LOGI(TAG, "semsem placed uid=%s", hex);
            }
        } else { /* S_TAG_PRESENT */
            if (err == ESP_OK && uid.uid_len > 0) {
                /* Same tag still present — reset miss counter. */
                nev_semsem_uid_t ev = { .uid_len = uid.uid_len };
                memcpy(ev.uid, uid.uid, uid.uid_len);
                if (uid_equal(&ev, &s_current_uid)) {
                    s_miss_count = 0;
                } else {
                    /* Different tag replaced the previous one without removal. */
                    nadeem_events_post(NEV_HMI_SEMSEM_REMOVED, &s_current_uid,
                                       sizeof s_current_uid, 0);
                    s_current_uid = ev;
                    s_miss_count  = 0;
                    nadeem_events_post(NEV_HMI_SEMSEM_PLACED, &ev, sizeof ev, 0);
                }
            } else if (++s_miss_count >= 2) {
                nadeem_events_post(NEV_HMI_SEMSEM_REMOVED, &s_current_uid,
                                   sizeof s_current_uid, 0);
                char hex[24];
                nadeem_uid_to_hex(&s_current_uid, hex, sizeof hex);
                ESP_LOGI(TAG, "semsem removed uid=%s", hex);
                memset(&s_current_uid, 0, sizeof s_current_uid);
                s_state = S_NO_TAG;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));  /* 5 Hz poll */
    }
}

esp_err_t hmi_start(void)
{
    board_io_buttons_register(on_button, NULL);
    BaseType_t ok = xTaskCreatePinnedToCore(nfc_poll_task, "hmi_nfc",
                                            4096, NULL, 4, NULL, 1);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}
