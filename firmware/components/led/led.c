#include "led.h"
#include "led_strip_hw.h"
#include "nadeem_events.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"

static const char *TAG = "led";

typedef void (*pattern_fn)(uint32_t, led_rgb_t[6]);

static const pattern_fn s_patterns[LED_MODE_COUNT] = {
    [LED_BOOT]            = led_pattern_boot,
    [LED_ONBOARDING]      = led_pattern_onboarding,
    [LED_IDLE]            = led_pattern_idle,
    [LED_SEMSEM_LOADING]  = led_pattern_semsem_loading,
    [LED_PLAYING]         = led_pattern_playing,
    [LED_PAUSED]          = led_pattern_paused,
    [LED_PRO_LISTENING]   = led_pattern_pro_listening,
    [LED_PRO_SPEAKING]    = led_pattern_pro_speaking,
    [LED_ERROR]           = led_pattern_error,
    [LED_WIFI_LOST]       = led_pattern_wifi_lost,
};

static led_strip_handle_t s_strip;
static led_mode_t         s_mode = LED_BOOT;

static void on_event(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg; (void)base;
    switch (id) {
        case NEV_SYS_ONBOARDING_ACTIVE: s_mode = LED_ONBOARDING; break;
        case NEV_SYS_ONBOARDING_DONE:   s_mode = LED_IDLE;       break;
        case NEV_SYS_WIFI_DOWN:         s_mode = LED_WIFI_LOST;  break;
        case NEV_SYS_WIFI_UP:
            if (s_mode == LED_WIFI_LOST) s_mode = LED_IDLE;
            break;
        case NEV_SYS_SEMSEM_LOADING:    s_mode = LED_SEMSEM_LOADING; break;
        case NEV_SYS_MODE_PRO_ENTER:
        case NEV_SYS_PRO_LISTENING:     s_mode = LED_PRO_LISTENING;  break;
        case NEV_SYS_PRO_SPEAKING:      s_mode = LED_PRO_SPEAKING;   break;
        case NEV_SYS_MODE_PRO_EXIT:     s_mode = LED_IDLE;           break;
        case NEV_SYS_ERROR:             s_mode = LED_ERROR;          break;
        case NEV_AUDIO_STATE: {
            if (!data) break;
            nev_audio_state_t *st = data;
            switch (st->state) {
                case 0 /*AUDIO_IDLE*/:    if (s_mode == LED_PLAYING || s_mode == LED_PAUSED) s_mode = LED_IDLE; break;
                case 1 /*AUDIO_LOADING*/: s_mode = LED_SEMSEM_LOADING; break;
                case 2 /*AUDIO_PLAYING*/: s_mode = LED_PLAYING; break;
                case 3 /*AUDIO_PAUSED*/:  s_mode = LED_PAUSED;  break;
            }
            break;
        }
        default: break;
    }
}

static void led_task(void *arg) {
    (void)arg;
    led_rgb_t frame[6];
    uint64_t start_us = esp_timer_get_time();
    while (1) {
        uint32_t tick_ms = (uint32_t)((esp_timer_get_time() - start_us) / 1000);
        led_mode_t m = s_mode;
        pattern_fn fn = (m < LED_MODE_COUNT && s_patterns[m]) ? s_patterns[m] : led_pattern_idle;
        fn(tick_ms, frame);

        for (int i = 0; i < NADEEM_LED_RING_COUNT; i++) {
            led_strip_set_pixel(s_strip, i, frame[i].r, frame[i].g, frame[i].b);
        }
        /* The 7th LED (index 6) is kept off per PRD. */
        led_strip_set_pixel(s_strip, 6, 0, 0, 0);
        led_strip_refresh(s_strip);

        vTaskDelay(pdMS_TO_TICKS(33));   /* ~30 Hz */
    }
}

esp_err_t led_start(void) {
    esp_err_t err = led_strip_hw_init(&s_strip);
    if (err != ESP_OK) return err;

    nadeem_events_on(NEV_SYS_WIFI_UP,           on_event, NULL);
    nadeem_events_on(NEV_SYS_WIFI_DOWN,         on_event, NULL);
    nadeem_events_on(NEV_SYS_SEMSEM_LOADING,    on_event, NULL);
    nadeem_events_on(NEV_SYS_MODE_PRO_ENTER,    on_event, NULL);
    nadeem_events_on(NEV_SYS_MODE_PRO_EXIT,     on_event, NULL);
    nadeem_events_on(NEV_SYS_PRO_LISTENING,     on_event, NULL);
    nadeem_events_on(NEV_SYS_PRO_SPEAKING,      on_event, NULL);
    nadeem_events_on(NEV_SYS_ERROR,             on_event, NULL);
    nadeem_events_on(NEV_SYS_ONBOARDING_ACTIVE, on_event, NULL);
    nadeem_events_on(NEV_SYS_ONBOARDING_DONE,   on_event, NULL);
    nadeem_events_on(NEV_AUDIO_STATE,           on_event, NULL);

    BaseType_t ok = xTaskCreatePinnedToCore(led_task, "led_anim", 3072, NULL, 3, NULL, 0);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

esp_err_t led_set_mode(led_mode_t mode) {
    if (mode >= LED_MODE_COUNT) return ESP_ERR_INVALID_ARG;
    s_mode = mode;
    return ESP_OK;
}

led_mode_t led_get_mode(void) { return s_mode; }
