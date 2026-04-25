#pragma once
#include "esp_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LED_BOOT = 0,
    LED_ONBOARDING,
    LED_IDLE,
    LED_SEMSEM_LOADING,
    LED_PLAYING,
    LED_PAUSED,
    LED_PRO_LISTENING,
    LED_PRO_SPEAKING,
    LED_ERROR,
    LED_WIFI_LOST,
    LED_MODE_COUNT,
} led_mode_t;

typedef struct { uint8_t r, g, b; } led_rgb_t;

/** Start the LED animation task and subscribe to NEV events. */
esp_err_t led_start(void);

/** Force a mode (used by the console `led mode` command). */
esp_err_t led_set_mode(led_mode_t mode);
led_mode_t led_get_mode(void);

/** Pure pattern functions (exposed for unit testing). They paint indices 0..5. */
void led_pattern_boot(uint32_t tick_ms, led_rgb_t out[6]);
void led_pattern_onboarding(uint32_t tick_ms, led_rgb_t out[6]);
void led_pattern_idle(uint32_t tick_ms, led_rgb_t out[6]);
void led_pattern_semsem_loading(uint32_t tick_ms, led_rgb_t out[6]);
void led_pattern_playing(uint32_t tick_ms, led_rgb_t out[6]);
void led_pattern_paused(uint32_t tick_ms, led_rgb_t out[6]);
void led_pattern_pro_listening(uint32_t tick_ms, led_rgb_t out[6]);
void led_pattern_pro_speaking(uint32_t tick_ms, led_rgb_t out[6]);
void led_pattern_error(uint32_t tick_ms, led_rgb_t out[6]);
void led_pattern_wifi_lost(uint32_t tick_ms, led_rgb_t out[6]);

#ifdef __cplusplus
}
#endif
