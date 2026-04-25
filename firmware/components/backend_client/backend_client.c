#include "backend_client.h"
#include "stats.h"

#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"

#include <stdio.h>
#include <string.h>

static const char *TAG = "backend";

#define MAX_URL 256

static char s_base_url[128];
static char s_token[96];

static esp_err_t set_auth_header(esp_http_client_handle_t c) {
    if (s_token[0] == '\0') return ESP_OK;
    char bearer[128];
    snprintf(bearer, sizeof bearer, "Bearer %s", s_token);
    return esp_http_client_set_header(c, "Authorization", bearer);
}

esp_err_t backend_client_init(const char *base_url, const char *device_token) {
    if (!base_url) return ESP_ERR_INVALID_ARG;
    strncpy(s_base_url, base_url, sizeof s_base_url - 1);
    s_base_url[sizeof s_base_url - 1] = '\0';
    if (device_token) {
        strncpy(s_token, device_token, sizeof s_token - 1);
        s_token[sizeof s_token - 1] = '\0';
    } else {
        s_token[0] = '\0';
    }
    ESP_LOGI(TAG, "backend configured: %s", s_base_url);
    return ESP_OK;
}

esp_err_t backend_client_fetch_semsem_manifest(const char *uid_hex,
                                               char *json_out, size_t cap) {
    if (!uid_hex || !json_out || cap < 8) return ESP_ERR_INVALID_ARG;
    char url[MAX_URL];
    snprintf(url, sizeof url, "%s/semsem/%s/manifest", s_base_url, uid_hex);

    esp_http_client_config_t cfg = {
        .url = url,
        .timeout_ms = 5000,
        .crt_bundle_attach = NULL, /* caller must menuconfig ESP_TLS_USE_SECURE_ELEMENT if needed */
    };
    esp_http_client_handle_t c = esp_http_client_init(&cfg);
    if (!c) return ESP_FAIL;
    set_auth_header(c);

    esp_err_t err = esp_http_client_open(c, 0);
    if (err != ESP_OK) { esp_http_client_cleanup(c); return err; }
    esp_http_client_fetch_headers(c);
    int status = esp_http_client_get_status_code(c);
    if (status != 200) {
        esp_http_client_close(c);
        esp_http_client_cleanup(c);
        return ESP_FAIL;
    }
    int read = esp_http_client_read(c, json_out, cap - 1);
    esp_http_client_close(c);
    esp_http_client_cleanup(c);
    if (read <= 0) return ESP_FAIL;
    json_out[read] = '\0';
    return ESP_OK;
}

esp_err_t backend_client_download_file(const char *url, const char *local_path) {
    if (!url || !local_path) return ESP_ERR_INVALID_ARG;
    FILE *f = fopen(local_path, "wb");
    if (!f) {
        ESP_LOGE(TAG, "fopen('%s') failed", local_path);
        return ESP_FAIL;
    }

    esp_http_client_config_t cfg = {
        .url = url,
        .timeout_ms = 10000,
    };
    esp_http_client_handle_t c = esp_http_client_init(&cfg);
    if (!c) { fclose(f); return ESP_FAIL; }
    set_auth_header(c);

    esp_err_t err = esp_http_client_open(c, 0);
    if (err != ESP_OK) goto cleanup;
    esp_http_client_fetch_headers(c);
    int status = esp_http_client_get_status_code(c);
    if (status != 200) { err = ESP_FAIL; goto cleanup; }

    char buf[4096];
    while (1) {
        int n = esp_http_client_read(c, buf, sizeof buf);
        if (n < 0) { err = ESP_FAIL; break; }
        if (n == 0) break;
        if (fwrite(buf, 1, n, f) != (size_t)n) { err = ESP_FAIL; break; }
    }

cleanup:
    esp_http_client_close(c);
    esp_http_client_cleanup(c);
    fclose(f);
    return err;
}

static esp_http_client_handle_t open_json_post(const char *url, const char *body) {
    esp_http_client_config_t cfg = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .timeout_ms = 5000,
    };
    esp_http_client_handle_t c = esp_http_client_init(&cfg);
    if (!c) return NULL;
    set_auth_header(c);
    esp_http_client_set_header(c, "Content-Type", "application/json");
    esp_http_client_set_post_field(c, body, (int)strlen(body));
    return c;
}

static bool upload_one(const char *uid, const stats_entry_t *e, void *user) {
    (void)user;
    if (!e->dirty) return true;

    char url[MAX_URL];
    snprintf(url, sizeof url, "%s/stats", s_base_url);

    cJSON *o = cJSON_CreateObject();
    cJSON_AddStringToObject(o, "uid", uid);
    cJSON_AddNumberToObject(o, "play_count", e->play_count);
    cJSON_AddNumberToObject(o, "total_play_ms", (double)e->total_play_ms);
    cJSON_AddNumberToObject(o, "last_played_unix", (double)e->last_played_unix);
    cJSON_AddNumberToObject(o, "pro_session_count", e->pro_session_count);
    cJSON_AddNumberToObject(o, "pro_total_ms", (double)e->pro_total_ms);
    char *body = cJSON_PrintUnformatted(o);
    cJSON_Delete(o);

    esp_http_client_handle_t c = open_json_post(url, body);
    esp_err_t err = c ? esp_http_client_perform(c) : ESP_FAIL;
    int status = c ? esp_http_client_get_status_code(c) : 0;
    if (c) esp_http_client_cleanup(c);
    free(body);

    if (err == ESP_OK && status >= 200 && status < 300) {
        stats_mark_uploaded(uid);
    } else {
        ESP_LOGW(TAG, "stats upload uid=%s failed (err=%d status=%d)", uid, (int)err, status);
    }
    return true;
}

esp_err_t backend_client_upload_stats(void) {
    if (!stats_has_dirty()) return ESP_OK;
    return stats_for_each(upload_one, NULL);
}

esp_err_t backend_client_bootstrap(const char *provision_token,
                                   char *out_token, size_t cap) {
    if (!provision_token || !out_token || cap < 16) return ESP_ERR_INVALID_ARG;
    char url[MAX_URL];
    snprintf(url, sizeof url, "%s/bootstrap", s_base_url);

    cJSON *o = cJSON_CreateObject();
    cJSON_AddStringToObject(o, "provision_token", provision_token);
    char *body = cJSON_PrintUnformatted(o);
    cJSON_Delete(o);

    esp_http_client_handle_t c = open_json_post(url, body);
    if (!c) { free(body); return ESP_FAIL; }

    esp_err_t err = esp_http_client_open(c, (int)strlen(body));
    if (err != ESP_OK) goto cleanup;
    esp_http_client_write(c, body, (int)strlen(body));
    esp_http_client_fetch_headers(c);
    if (esp_http_client_get_status_code(c) != 200) { err = ESP_FAIL; goto cleanup; }

    char resp[256];
    int n = esp_http_client_read(c, resp, sizeof resp - 1);
    if (n <= 0) { err = ESP_FAIL; goto cleanup; }
    resp[n] = '\0';

    cJSON *root = cJSON_Parse(resp);
    cJSON *tok  = root ? cJSON_GetObjectItem(root, "device_token") : NULL;
    if (cJSON_IsString(tok) && tok->valuestring) {
        strncpy(out_token, tok->valuestring, cap - 1);
        out_token[cap - 1] = '\0';
        err = ESP_OK;
    } else {
        err = ESP_ERR_INVALID_RESPONSE;
    }
    cJSON_Delete(root);

cleanup:
    free(body);
    esp_http_client_close(c);
    esp_http_client_cleanup(c);
    return err;
}
