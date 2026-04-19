#pragma once

#include <stdbool.h>
#include "esp_err.h"
#include "esp_io_expander_tca95xx_16bit.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- IO expander (TCA9555) ------------------------------------------- */

/** Global handle used by the rest of the firmware. Populated by board_io_init(). */
extern esp_io_expander_handle_t g_io_expander;

/** Initialize the TCA9555 over I2C (uses the bus exposed by bsp_i2c_handle()).
 *  Configures default pin directions: pins 0,1,5,6,8 as output, 2,9,10,11 as input.
 *  Also starts the 3-button scanner task. */
esp_err_t board_io_init(void);

/** Set one or more output pins (pin is a mask e.g. IO_EXPANDER_PIN_NUM_8). */
esp_err_t board_io_expander_set(uint32_t pin_mask, uint8_t state);
bool      board_io_expander_read(uint32_t pin_mask);

/* ---- Buttons --------------------------------------------------------- */

typedef enum {
    KEY_ID_9  = 9,
    KEY_ID_10 = 10,
    KEY_ID_11 = 11,
} key_id_t;

typedef enum {
    KEY_EVENT_SHORT_PRESS,
    KEY_EVENT_LONG_PRESS,
} key_event_t;

typedef void (*key_callback_t)(key_id_t key_id, key_event_t event, void *user_data);

/** Install a single button callback. Only one is supported. */
esp_err_t board_io_buttons_register(key_callback_t cb, void *user_data);

#ifdef __cplusplus
}
#endif
