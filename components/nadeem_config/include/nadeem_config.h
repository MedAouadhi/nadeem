#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Persistent Nadeem configuration, kept in NVS namespace "nadeem". */

bool       nadeem_config_has_wifi(void);
esp_err_t  nadeem_config_get_wifi(char *ssid_out, size_t ssid_cap,
                                  char *pass_out, size_t pass_cap);
esp_err_t  nadeem_config_set_wifi(const char *ssid, const char *password);
esp_err_t  nadeem_config_clear(void);

esp_err_t  nadeem_config_get_backend_url(char *out, size_t cap);
esp_err_t  nadeem_config_set_backend_url(const char *url);

esp_err_t  nadeem_config_get_device_token(char *out, size_t cap);
esp_err_t  nadeem_config_set_device_token(const char *token);

/** Start Wi-Fi in STA mode using credentials from NVS. */
esp_err_t  nadeem_wifi_sta_start_from_nvs(void);

#ifdef __cplusplus
}
#endif
