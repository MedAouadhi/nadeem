#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Start the semsem SMF task. Subscribes to HMI and audio events. */
esp_err_t semsem_start(void);

/** Directly trigger playback for a UID (used by console). */
esp_err_t semsem_trigger_uid(const char *uid_hex);

#ifdef __cplusplus
}
#endif
