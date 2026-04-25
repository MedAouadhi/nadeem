#include "board_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_log.h"

static const char *TAG = "buttons";

typedef struct {
    bool     is_pressed;
    bool     last_pressed;
    uint64_t press_start_ms;
    bool     long_triggered;
} key_state_t;

static key_state_t    s_keys[3];
static key_callback_t s_cb;
static void          *s_user_data;
static const uint32_t LONG_PRESS_MS = 1000;

static bool level_to_pressed(uint32_t mask, key_id_t id)
{
    /* Active-low button wired through TCA9555 */
    return !(mask & (1ULL << id));
}

static void button_scan_task(void *arg)
{
    (void)arg;
    while (1) {
        uint32_t mask = 0;
        esp_err_t ret = esp_io_expander_get_level(
            g_io_expander,
            (1ULL << KEY_ID_9) | (1ULL << KEY_ID_10) | (1ULL << KEY_ID_11),
            &mask);
        if (ret != ESP_OK) {
            ESP_LOGE(TAG, "io expander read failed: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(500));
            continue;
        }

        for (int i = 0; i < 3; i++) {
            key_id_t id = (key_id_t)(KEY_ID_9 + i);
            key_state_t *k = &s_keys[i];
            k->is_pressed = level_to_pressed(mask, id);
            uint64_t now_ms = esp_timer_get_time() / 1000;

            if (k->is_pressed && !k->last_pressed) {
                k->press_start_ms = now_ms;
                k->long_triggered = false;
            } else if (!k->is_pressed && k->last_pressed) {
                uint32_t dur = (uint32_t)(now_ms - k->press_start_ms);
                if (dur < LONG_PRESS_MS && !k->long_triggered && s_cb) {
                    s_cb(id, KEY_EVENT_SHORT_PRESS, s_user_data);
                }
            } else if (k->is_pressed && !k->long_triggered) {
                if (now_ms - k->press_start_ms >= LONG_PRESS_MS) {
                    if (s_cb) s_cb(id, KEY_EVENT_LONG_PRESS, s_user_data);
                    k->long_triggered = true;
                }
            }
            k->last_pressed = k->is_pressed;
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

esp_err_t board_io_buttons_start(void)
{
    for (int i = 0; i < 3; i++) {
        s_keys[i] = (key_state_t){0};
    }
    BaseType_t ok = xTaskCreatePinnedToCore(button_scan_task, "btn_scan",
                                            3072, NULL, 3, NULL, 0);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

esp_err_t board_io_buttons_register(key_callback_t cb, void *user_data)
{
    if (!cb) return ESP_ERR_INVALID_ARG;
    s_cb = cb;
    s_user_data = user_data;
    return ESP_OK;
}
