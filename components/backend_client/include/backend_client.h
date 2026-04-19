#pragma once

#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize with the backend base URL (e.g. "https://api.nadeem.example")
 *  and a device bearer token (obtained during onboarding bootstrap). */
esp_err_t backend_client_init(const char *base_url, const char *device_token);

/** Fetch the manifest JSON for a semsem UID. Caller provides the output buffer;
 *  json_out will be null-terminated on success. */
esp_err_t backend_client_fetch_semsem_manifest(const char *uid_hex,
                                               char *json_out, size_t cap);

/** Download an absolute URL to a local SD path. Streams to disk; no full-file
 *  RAM buffering. */
esp_err_t backend_client_download_file(const char *url, const char *local_path);

/** Upload dirty stats entries to POST /stats. Marks uploaded ones clean. */
esp_err_t backend_client_upload_stats(void);

/** Exchange a provisioning token (from the app) for a persistent device token.
 *  Writes the response token into out_token (null-terminated). */
esp_err_t backend_client_bootstrap(const char *provision_token,
                                   char *out_token, size_t cap);

#ifdef __cplusplus
}
#endif
