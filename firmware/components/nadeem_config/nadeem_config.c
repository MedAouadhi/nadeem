#include "nadeem_config.h"
#include "nadeem_events.h"

#include "nvs.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "esp_log.h"

#include <string.h>

static const char *TAG = "ncfg";
#define NS "nadeem"

static esp_err_t read_str(const char *key, char *out, size_t cap) {
    nvs_handle_t h;
    esp_err_t err = nvs_open(NS, NVS_READONLY, &h);
    if (err != ESP_OK) return err;
    size_t len = cap;
    err = nvs_get_str(h, key, out, &len);
    nvs_close(h);
    return err;
}

static esp_err_t write_str(const char *key, const char *value) {
    nvs_handle_t h;
    esp_err_t err = nvs_open(NS, NVS_READWRITE, &h);
    if (err != ESP_OK) return err;
    err = nvs_set_str(h, key, value);
    if (err == ESP_OK) err = nvs_commit(h);
    nvs_close(h);
    return err;
}

bool nadeem_config_has_wifi(void) {
    char ssid[64];
    return read_str("wifi_ssid", ssid, sizeof ssid) == ESP_OK && ssid[0] != '\0';
}

esp_err_t nadeem_config_get_wifi(char *ssid_out, size_t ssid_cap,
                                 char *pass_out, size_t pass_cap) {
    esp_err_t err = read_str("wifi_ssid", ssid_out, ssid_cap);
    if (err != ESP_OK) return err;
    if (pass_out && pass_cap) {
        err = read_str("wifi_pass", pass_out, pass_cap);
        if (err == ESP_ERR_NVS_NOT_FOUND) { pass_out[0] = '\0'; err = ESP_OK; }
    }
    return err;
}

esp_err_t nadeem_config_set_wifi(const char *ssid, const char *password) {
    esp_err_t err = write_str("wifi_ssid", ssid ? ssid : "");
    if (err == ESP_OK) err = write_str("wifi_pass", password ? password : "");
    return err;
}

esp_err_t nadeem_config_clear(void) {
    nvs_handle_t h;
    esp_err_t err = nvs_open(NS, NVS_READWRITE, &h);
    if (err != ESP_OK) return err;
    nvs_erase_all(h);
    nvs_commit(h);
    nvs_close(h);
    return ESP_OK;
}

esp_err_t nadeem_config_get_backend_url(char *out, size_t cap) {
    return read_str("backend_url", out, cap);
}
esp_err_t nadeem_config_set_backend_url(const char *url) {
    return write_str("backend_url", url);
}
esp_err_t nadeem_config_get_device_token(char *out, size_t cap) {
    return read_str("dev_token", out, cap);
}
esp_err_t nadeem_config_set_device_token(const char *token) {
    return write_str("dev_token", token);
}

/* ---- Wi-Fi STA bringup ------------------------------------------------ */

static void on_wifi(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg; (void)data;
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        nadeem_events_post(NEV_SYS_WIFI_DOWN, NULL, 0, 0);
        esp_wifi_connect();
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        nadeem_events_post(NEV_SYS_WIFI_UP, NULL, 0, 0);
        ESP_LOGI(TAG, "got IP");
    }
}

esp_err_t nadeem_wifi_sta_start_from_nvs(void) {
    char ssid[64] = {0}, pass[64] = {0};
    esp_err_t err = nadeem_config_get_wifi(ssid, sizeof ssid, pass, sizeof pass);
    if (err != ESP_OK || ssid[0] == '\0') return ESP_ERR_NOT_FOUND;

    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, on_wifi, NULL);
    esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, on_wifi, NULL);

    wifi_config_t wc = {0};
    strncpy((char *)wc.sta.ssid, ssid, sizeof wc.sta.ssid - 1);
    strncpy((char *)wc.sta.password, pass, sizeof wc.sta.password - 1);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wc));
    ESP_ERROR_CHECK(esp_wifi_start());
    return ESP_OK;
}
