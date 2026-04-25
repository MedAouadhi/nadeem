#pragma once

#include <stdint.h>
#include "esp_err.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"

#ifdef __cplusplus
extern "C" {
#endif

ESP_EVENT_DECLARE_BASE(NADEEM_EVENTS);

/**
 * System-wide event IDs. Handlers must treat payload pointers as read-only and
 * copy whatever they need before returning.
 */
typedef enum {
    /* HMI → system */
    NEV_HMI_SEMSEM_PLACED = 1,  /* payload: nev_semsem_uid_t */
    NEV_HMI_SEMSEM_REMOVED,     /* payload: nev_semsem_uid_t */
    NEV_HMI_BTN_PREV,
    NEV_HMI_BTN_PLAY_PAUSE,
    NEV_HMI_BTN_NEXT,
    NEV_HMI_BTN_LONG_RESET,

    /* Control → audio */
    NEV_AUDIO_PLAY,             /* payload: nev_audio_play_t */
    NEV_AUDIO_STOP,
    NEV_AUDIO_PAUSE,
    NEV_AUDIO_RESUME,
    NEV_AUDIO_NEXT,
    NEV_AUDIO_PREV,

    /* Audio → listeners */
    NEV_AUDIO_STATE,            /* payload: nev_audio_state_t */
    NEV_AUDIO_TRACK_DONE,

    /* System */
    NEV_SYS_WIFI_UP,
    NEV_SYS_WIFI_DOWN,
    NEV_SYS_BACKEND_UP,
    NEV_SYS_BACKEND_DOWN,
    NEV_SYS_SEMSEM_LOADING,     /* payload: nev_semsem_uid_t */
    NEV_SYS_MODE_PRO_ENTER,     /* payload: nev_pro_enter_t */
    NEV_SYS_MODE_PRO_EXIT,
    NEV_SYS_PRO_LISTENING,
    NEV_SYS_PRO_SPEAKING,
    NEV_SYS_ERROR,              /* payload: nev_error_t */
    NEV_SYS_ONBOARDING_ACTIVE,
    NEV_SYS_ONBOARDING_DONE,
} nev_id_t;

typedef struct {
    uint8_t uid[10];
    uint8_t uid_len;
} nev_semsem_uid_t;

typedef struct {
    char     path[128];
    uint32_t start_byte;
} nev_audio_play_t;

typedef struct {
    uint8_t  state;       /* audio_state_t value */
    uint32_t pos_bytes;
} nev_audio_state_t;

typedef struct {
    nev_semsem_uid_t uid;
    char             role[32];   /* e.g. "doctor", "engineer" */
} nev_pro_enter_t;

typedef struct {
    uint8_t source;
    int32_t code;
} nev_error_t;

/** Create the shared event loop. Must be called once, after esp_event_loop_create_default(). */
esp_err_t nadeem_events_init(void);

/** Returns the shared loop handle used by all Nadeem modules. */
esp_event_loop_handle_t nadeem_events_loop(void);

/** Post an event to the shared loop. Thin wrapper around esp_event_post_to. */
esp_err_t nadeem_events_post(nev_id_t id, const void *data, size_t data_sz, TickType_t ticks_to_wait);

/** Register a handler on the shared loop. */
esp_err_t nadeem_events_on(nev_id_t id, esp_event_handler_t cb, void *arg);

/** Convert UID bytes to a lowercase hex string (null-terminated). buf_len ≥ 2*uid_len+1. */
void nadeem_uid_to_hex(const nev_semsem_uid_t *uid, char *buf, size_t buf_len);

#ifdef __cplusplus
}
#endif
