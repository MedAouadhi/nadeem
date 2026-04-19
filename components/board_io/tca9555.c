#include "board_io.h"
#include "bsp.h"
#include "esp_log.h"

static const char *TAG = "board_io";

esp_io_expander_handle_t g_io_expander = NULL;

/* Forward declaration from buttons.c */
esp_err_t board_io_buttons_start(void);

esp_err_t board_io_init(void)
{
    i2c_master_bus_handle_t bus = bsp_i2c_handle();
    if (!bus) {
        ESP_LOGE(TAG, "bsp I2C bus not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = esp_io_expander_new_i2c_tca95xx_16bit(
        bus, ESP_IO_EXPANDER_I2C_TCA9555_ADDRESS_000, &g_io_expander);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "tca9555 init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_io_expander_set_dir(g_io_expander,
        IO_EXPANDER_PIN_NUM_0 | IO_EXPANDER_PIN_NUM_1 |
        IO_EXPANDER_PIN_NUM_5 | IO_EXPANDER_PIN_NUM_6 |
        IO_EXPANDER_PIN_NUM_8, IO_EXPANDER_OUTPUT);
    if (ret != ESP_OK) ESP_LOGE(TAG, "set_dir out failed");

    ret = esp_io_expander_set_dir(g_io_expander,
        IO_EXPANDER_PIN_NUM_2 | IO_EXPANDER_PIN_NUM_9 |
        IO_EXPANDER_PIN_NUM_10 | IO_EXPANDER_PIN_NUM_11, IO_EXPANDER_INPUT);
    if (ret != ESP_OK) ESP_LOGE(TAG, "set_dir in failed");

    return board_io_buttons_start();
}

esp_err_t board_io_expander_set(uint32_t pin_mask, uint8_t state)
{
    return esp_io_expander_set_level(g_io_expander, pin_mask, state);
}

bool board_io_expander_read(uint32_t pin_mask)
{
    uint32_t lvl = 0;
    esp_io_expander_get_level(g_io_expander, pin_mask, &lvl);
    return (lvl & pin_mask) != 0;
}
