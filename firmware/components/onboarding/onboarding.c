#include "onboarding.h"
#include "nadeem_events.h"
#include "nadeem_config.h"
#include "backend_client.h"

#include "esp_http_server.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_mac.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs.h"
#include "cJSON.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <string.h>
#include <stdio.h>
#include <ctype.h>

static const char *TAG = "onboard";

static httpd_handle_t s_server;

/* ---- tiny form-urlencoded parser ------------------------------------ */

static int hexval(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
    if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
    return -1;
}

static void url_decode(const char *src, size_t src_len, char *dst, size_t dst_cap) {
    size_t di = 0;
    for (size_t i = 0; i < src_len && di + 1 < dst_cap; i++) {
        if (src[i] == '+') {
            dst[di++] = ' ';
        } else if (src[i] == '%' && i + 2 < src_len) {
            int h = hexval(src[i+1]), l = hexval(src[i+2]);
            if (h >= 0 && l >= 0) { dst[di++] = (char)((h << 4) | l); i += 2; }
            else dst[di++] = src[i];
        } else {
            dst[di++] = src[i];
        }
    }
    dst[di] = '\0';
}

int onboarding_parse_form(const char *body, size_t len,
                          char *ssid, size_t ssid_cap,
                          char *pass, size_t pass_cap,
                          char *token, size_t token_cap) {
    if (ssid_cap) ssid[0] = '\0';
    if (pass_cap) pass[0] = '\0';
    if (token_cap) token[0] = '\0';
    int found = 0;

    size_t i = 0;
    while (i < len) {
        size_t k_start = i;
        while (i < len && body[i] != '=' && body[i] != '&') i++;
        size_t k_len = i - k_start;
        size_t v_start = 0, v_len = 0;
        if (i < len && body[i] == '=') {
            i++;
            v_start = i;
            while (i < len && body[i] != '&') i++;
            v_len = i - v_start;
        }
        if (i < len && body[i] == '&') i++;

        if (k_len == 4 && strncmp(&body[k_start], "ssid", 4) == 0) {
            url_decode(&body[v_start], v_len, ssid, ssid_cap); found++;
        } else if (k_len == 8 && strncmp(&body[k_start], "password", 8) == 0) {
            url_decode(&body[v_start], v_len, pass, pass_cap); found++;
        } else if (k_len == 15 && strncmp(&body[k_start], "provision_token", 15) == 0) {
            url_decode(&body[v_start], v_len, token, token_cap); found++;
        }
    }
    return found;
}

/* ---- HTTP handlers -------------------------------------------------- */

static esp_err_t h_info(httpd_req_t *req) {
    uint8_t mac[6] = {0};
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    char json[128];
    snprintf(json, sizeof json,
             "{\"device_id\":\"%02x%02x%02x%02x%02x%02x\",\"fw\":\"0.1.0\",\"status\":\"onboarding\"}",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    httpd_resp_set_type(req, "application/json");
    return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t h_scan(httpd_req_t *req) {
    wifi_scan_config_t cfg = {0};
    esp_wifi_scan_start(&cfg, true);
    uint16_t n = 0;
    esp_wifi_scan_get_ap_num(&n);
    if (n > 16) n = 16;
    wifi_ap_record_t recs[16];
    esp_wifi_scan_get_ap_records(&n, recs);

    cJSON *arr = cJSON_CreateArray();
    for (int i = 0; i < n; i++) {
        cJSON *o = cJSON_CreateObject();
        cJSON_AddStringToObject(o, "ssid", (char *)recs[i].ssid);
        cJSON_AddNumberToObject(o, "rssi", recs[i].rssi);
        cJSON_AddNumberToObject(o, "auth", recs[i].authmode);
        cJSON_AddItemToArray(arr, o);
    }
    char *s = cJSON_PrintUnformatted(arr);
    cJSON_Delete(arr);
    httpd_resp_set_type(req, "application/json");
    esp_err_t err = httpd_resp_send(req, s, HTTPD_RESP_USE_STRLEN);
    free(s);
    return err;
}

static esp_err_t h_provision(httpd_req_t *req) {
    char buf[512];
    int n = httpd_req_recv(req, buf, sizeof buf - 1);
    if (n <= 0) return ESP_FAIL;
    buf[n] = '\0';

    char ssid[64] = {0}, pass[64] = {0}, token[96] = {0};
    onboarding_parse_form(buf, n, ssid, sizeof ssid,
                          pass, sizeof pass, token, sizeof token);
    if (ssid[0] == '\0') {
        httpd_resp_set_status(req, "400 Bad Request");
        return httpd_resp_send(req, "missing ssid", HTTPD_RESP_USE_STRLEN);
    }

    /* Persist provisionally; onboarding task reboots on success. */
    nadeem_config_set_wifi(ssid, pass);
    /* Store token under a temp key — bootstrap uses it after connecting. */
    {
        nvs_handle_t h;
        if (nvs_open("nadeem", NVS_READWRITE, &h) == ESP_OK) {
            nvs_set_str(h, "prov_token", token);
            nvs_commit(h);
            nvs_close(h);
        }
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, "{\"ok\":true}", HTTPD_RESP_USE_STRLEN);

    /* Reboot after a short grace period so the client receives the response. */
    vTaskDelay(pdMS_TO_TICKS(500));
    esp_restart();
    return ESP_OK;
}

static esp_err_t h_reset(httpd_req_t *req) {
    nadeem_config_clear();
    httpd_resp_send(req, "{\"ok\":true}", HTTPD_RESP_USE_STRLEN);
    vTaskDelay(pdMS_TO_TICKS(500));
    esp_restart();
    return ESP_OK;
}

static esp_err_t start_http(void) {
    httpd_config_t cfg = HTTPD_DEFAULT_CONFIG();
    cfg.server_port = 80;
    ESP_ERROR_CHECK(httpd_start(&s_server, &cfg));

    httpd_uri_t u_info      = { .uri = "/info",      .method = HTTP_GET,  .handler = h_info };
    httpd_uri_t u_scan      = { .uri = "/scan",      .method = HTTP_GET,  .handler = h_scan };
    httpd_uri_t u_provision = { .uri = "/provision", .method = HTTP_POST, .handler = h_provision };
    httpd_uri_t u_reset     = { .uri = "/reset",     .method = HTTP_POST, .handler = h_reset };
    httpd_register_uri_handler(s_server, &u_info);
    httpd_register_uri_handler(s_server, &u_scan);
    httpd_register_uri_handler(s_server, &u_provision);
    httpd_register_uri_handler(s_server, &u_reset);
    return ESP_OK;
}

static esp_err_t start_softap(void) {
    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_ap();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    uint8_t mac[6] = {0};
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    wifi_config_t ap = {0};
    snprintf((char *)ap.ap.ssid, sizeof ap.ap.ssid, "Nadeem-%02X%02X", mac[4], mac[5]);
    ap.ap.ssid_len = strlen((char *)ap.ap.ssid);
    ap.ap.channel = 1;
    ap.ap.max_connection = 4;
    ap.ap.authmode = WIFI_AUTH_OPEN;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "SoftAP SSID: %s", (char *)ap.ap.ssid);
    return ESP_OK;
}

esp_err_t onboarding_start(void) {
    /* Default event loop is already created by app_main(). */
    nadeem_events_post(NEV_SYS_ONBOARDING_ACTIVE, NULL, 0, 0);
    start_softap();
    start_http();
    return ESP_OK;
}
