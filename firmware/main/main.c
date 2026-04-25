#include "esp_log.h"
#include "esp_event.h"
#include "nvs.h"
#include "nvs_flash.h"

#include "nadeem_events.h"
#include "nadeem_config.h"
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
#include "backend_client.h"

static const char *TAG = "app";

/* Runs once Wi-Fi comes up. Completes backend bootstrap if we only have a
 * provisioning token (fresh post-onboarding boot), then initializes the client. */
static void on_wifi_up(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg; (void)base; (void)id; (void)data;

    char url[128]   = {0};
    char token[96]  = {0};
    nadeem_config_get_backend_url(url, sizeof url);
    if (url[0] == '\0') {
        /* No backend URL provisioned yet — nothing to do. Device still plays
         * cached semsems from SD without a backend. */
        ESP_LOGW(TAG, "no backend URL configured");
        return;
    }

    if (nadeem_config_get_device_token(token, sizeof token) != ESP_OK || token[0] == '\0') {
        /* Still have the one-shot provisioning token from onboarding. */
        nvs_handle_t h;
        char prov[96] = {0};
        if (nvs_open("nadeem", NVS_READWRITE, &h) == ESP_OK) {
            size_t sz = sizeof prov;
            nvs_get_str(h, "prov_token", prov, &sz);
            nvs_close(h);
        }
        backend_client_init(url, NULL);
        if (prov[0] != '\0' &&
            backend_client_bootstrap(prov, token, sizeof token) == ESP_OK) {
            nadeem_config_set_device_token(token);
            /* Clear the provisioning token; it's single-use. */
            if (nvs_open("nadeem", NVS_READWRITE, &h) == ESP_OK) {
                nvs_erase_key(h, "prov_token");
                nvs_commit(h);
                nvs_close(h);
            }
        }
    }
    backend_client_init(url, token);
    nadeem_events_post(NEV_SYS_BACKEND_UP, NULL, 0, 0);
}

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
        nadeem_events_on(NEV_SYS_WIFI_UP, on_wifi_up, NULL);
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
