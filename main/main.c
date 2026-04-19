#include "esp_log.h"
#include "esp_event.h"
#include "nvs_flash.h"

#include "nadeem_events.h"
#include "bsp.h"
#include "board_io.h"
#include "hmi.h"
#include "audio.h"
#include "semsem.h"
#include "semsem_pro.h"
#include "led.h"
#include "stats.h"
#include "onboarding.h"
#include "console_cmds.h"
#include "nadeem_config.h"

static const char *TAG = "app";

void app_main(void) {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(nadeem_events_init());

    ESP_ERROR_CHECK(bsp_init(16000, 2, 16));
    ESP_ERROR_CHECK(bsp_sdcard_mount("/sdcard", 10));
    ESP_ERROR_CHECK(board_io_init());

    ESP_ERROR_CHECK(stats_init());
    ESP_ERROR_CHECK(led_start());
    ESP_ERROR_CHECK(console_cmds_start());

    if (nadeem_config_has_wifi()) {
        ESP_LOGI(TAG, "Wi-Fi credentials present — entering normal mode");
        ESP_ERROR_CHECK(nadeem_wifi_sta_start_from_nvs());
        ESP_ERROR_CHECK(hmi_start());
        ESP_ERROR_CHECK(audio_start());
        ESP_ERROR_CHECK(semsem_start());
        ESP_ERROR_CHECK(semsem_pro_start());
    } else {
        ESP_LOGI(TAG, "No Wi-Fi credentials — entering onboarding mode");
        ESP_ERROR_CHECK(onboarding_start());
    }

    ESP_LOGI(TAG, "Nadeem boot complete");
}
