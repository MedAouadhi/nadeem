#include "led_strip_hw.h"
#include "esp_log.h"

static const char *TAG = "led_hw";

esp_err_t led_strip_hw_init(led_strip_handle_t *out)
{
    if (!out) return ESP_ERR_INVALID_ARG;

    led_strip_config_t strip_config = {
        .strip_gpio_num        = NADEEM_LED_STRIP_GPIO,
        .max_leds              = NADEEM_LED_STRIP_COUNT,
        .led_model             = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_RGB,
        .flags = { .invert_out = false },
    };
    led_strip_rmt_config_t rmt_config = {
        .clk_src           = RMT_CLK_SRC_DEFAULT,
        .resolution_hz     = 10 * 1000 * 1000,
        .mem_block_symbols = 0,
        .flags             = { .with_dma = 0 },
    };

    esp_err_t ret = led_strip_new_rmt_device(&strip_config, &rmt_config, out);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "led_strip init failed: %s", esp_err_to_name(ret));
        return ret;
    }
    ESP_LOGI(TAG, "WS2812 strip ready (%d LEDs, GPIO %d)",
             NADEEM_LED_STRIP_COUNT, NADEEM_LED_STRIP_GPIO);
    return ESP_OK;
}
