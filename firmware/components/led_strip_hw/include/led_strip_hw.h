#pragma once

#include "led_strip.h"
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Physical 7-LED strip on the devkit; we use indices 0..5 as a 6-LED ring. */
#define NADEEM_LED_STRIP_GPIO  38
#define NADEEM_LED_STRIP_COUNT 7
#define NADEEM_LED_RING_COUNT  6

/** Initialize the WS2812 RMT driver. Returns the handle via *out. */
esp_err_t led_strip_hw_init(led_strip_handle_t *out);

#ifdef __cplusplus
}
#endif
