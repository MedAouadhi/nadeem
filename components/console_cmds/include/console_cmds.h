#pragma once
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Start the esp_console REPL on UART0 and register all Nadeem commands. */
esp_err_t console_cmds_start(void);

#ifdef __cplusplus
}
#endif
