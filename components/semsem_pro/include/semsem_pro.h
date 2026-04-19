#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Start the semsem-pro listener. On NEV_SYS_MODE_PRO_ENTER it connects a
 *  WebSocket to the backend and streams mic audio up / TTS audio down. */
esp_err_t semsem_pro_start(void);

#ifdef __cplusplus
}
#endif
