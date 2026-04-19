#pragma once
#include <stdint.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_IDLE = 0,
    AUDIO_LOADING,
    AUDIO_PLAYING,
    AUDIO_PAUSED,
    AUDIO_ERROR,
} audio_state_t;

/** Start the audio SMF task and subscribe to NEV_AUDIO_* events. */
esp_err_t audio_start(void);

audio_state_t audio_current_state(void);
uint32_t      audio_current_position(void);

/** Set playback volume (0..100). */
esp_err_t audio_set_volume(uint8_t vol);
uint8_t   audio_get_volume(void);

#ifdef __cplusplus
}
#endif
