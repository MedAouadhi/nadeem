#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize PN532, register the button callback, spawn the NFC poll task.
 *  Must be called after board_io_init() and bsp_init(). */
esp_err_t hmi_start(void);

#ifdef __cplusplus
}
#endif
