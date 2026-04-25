#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Bring up a SoftAP "Nadeem-XXXX" with an HTTP server on 192.168.4.1.
 *  Blocks in the background; on successful provisioning it writes Wi-Fi and
 *  backend credentials to NVS and reboots the device. */
esp_err_t onboarding_start(void);

/** Exposed for unit tests: parse a `key=value&key2=value2` body. Writes
 *  null-terminated values into out buffers. Returns number of fields decoded. */
int onboarding_parse_form(const char *body, size_t len,
                          char *ssid, size_t ssid_cap,
                          char *pass, size_t pass_cap,
                          char *token, size_t token_cap);

#ifdef __cplusplus
}
#endif
