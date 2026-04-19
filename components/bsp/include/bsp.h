#pragma once

#include "driver/gpio.h"
#include "driver/i2s_std.h"
#include "driver/i2c_master.h"
#include "soc/soc_caps.h"
#include "esp_idf_version.h"
#include "esp_codec_dev.h"
#include "esp_codec_dev_defaults.h"
#include "esp_codec_dev_os.h"
#include "sdkconfig.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Board pinout (Waveshare ESP32-S3-AUDIO-Board) ------------------------- */
#define I2C_NUM         (0)
#define GPIO_I2C_SCL    (GPIO_NUM_10)
#define GPIO_I2C_SDA    (GPIO_NUM_11)

#define FUNC_SDMMC_EN   (1)
#define SDMMC_BUS_WIDTH (1)
#define GPIO_SDMMC_CLK  (GPIO_NUM_40)
#define GPIO_SDMMC_CMD  (GPIO_NUM_42)
#define GPIO_SDMMC_D0   (GPIO_NUM_41)
#define GPIO_SDMMC_D1   (GPIO_NUM_NC)
#define GPIO_SDMMC_D2   (GPIO_NUM_NC)
#define GPIO_SDMMC_D3   (GPIO_NUM_NC)
#define GPIO_SDMMC_DET  (GPIO_NUM_NC)

#define GPIO_I2S_LRCK   (GPIO_NUM_14)
#define GPIO_I2S_MCLK   (GPIO_NUM_12)
#define GPIO_I2S_SCLK   (GPIO_NUM_13)
#define GPIO_I2S_SDIN   (GPIO_NUM_15)
#define GPIO_I2S_DOUT   (GPIO_NUM_16)

#define RECORD_VOLUME   (30.0)
#define PLAYER_VOLUME   (60)
#define GPIO_PWR_CTRL   (-1)

#define MAX_FILE_NAME_SIZE 100
#define MAX_PATH_SIZE      512

#define I2S_CONFIG_DEFAULT(sample_rate, channel_fmt, bits_per_chan) { \
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(16000), \
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(32, I2S_SLOT_MODE_STEREO), \
        .gpio_cfg = { \
            .mclk = GPIO_I2S_MCLK, \
            .bclk = GPIO_I2S_SCLK, \
            .ws   = GPIO_I2S_LRCK, \
            .dout = GPIO_I2S_DOUT, \
            .din  = GPIO_I2S_SDIN, \
        }, \
    }

/* LED strip pins — used by led_strip_hw */
#define LED_STRIP_GPIO_PIN  38
#define LED_STRIP_LED_COUNT 7

/* Public API ------------------------------------------------------------- */

/** Initialize I2C bus, I2S, ES7210 (mic) and ES8311 (DAC) codecs. */
esp_err_t bsp_init(uint32_t sample_rate, int channel_format, int bits_per_chan);

/** Mount SD card via SDMMC at mount_point with room for max_files open files. */
esp_err_t bsp_sdcard_mount(const char *mount_point, size_t max_files);
esp_err_t bsp_sdcard_unmount(const char *mount_point);

/** Write PCM samples to the DAC. Handles 16→32 bit widening if configured. */
esp_err_t bsp_i2s_write(const int16_t *data, int length_bytes, uint32_t ticks_to_wait);

/** Read a frame from the mic. buffer_len is in bytes. If reorder==true, the 4-ch
 *  raw interleaved data is packed into 3-ch AFE-style layout. */
esp_err_t bsp_i2s_read_mic(bool reorder, int16_t *buffer, int buffer_len);

int         bsp_mic_channels(void);
const char *bsp_mic_format(void);

esp_err_t bsp_set_volume(int volume);
esp_err_t bsp_get_volume(int *volume);

i2c_master_bus_handle_t bsp_i2c_handle(void);
esp_codec_dev_handle_t  bsp_play_codec(void);
uint32_t                bsp_sd_capacity_mb(void);

/** Enumerate files in a directory matching an extension (case-insensitive).
 *  Returns the number of files written into file_names. */
uint16_t bsp_list_files(const char *directory, const char *extension,
                        char file_names[][MAX_FILE_NAME_SIZE], uint16_t max_files);

#ifdef __cplusplus
}
#endif
