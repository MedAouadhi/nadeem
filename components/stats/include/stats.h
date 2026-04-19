#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t play_count;
    uint64_t total_play_ms;
    uint64_t last_played_unix;
    uint32_t pro_session_count;
    uint64_t pro_total_ms;
    bool     dirty;
} stats_entry_t;

/** Initialize NVS namespace "nadeem_stat". Must be called after nvs_flash_init(). */
esp_err_t stats_init(void);

/** Read the current counters for uid_hex. Missing entries return zeroed struct. */
esp_err_t stats_get(const char *uid_hex, stats_entry_t *out);

/** Bump play_count and refresh last_played_unix. */
esp_err_t stats_record_play_start(const char *uid_hex);

/** Add ms to total_play_ms. Cheap — only writes NVS on flush. */
esp_err_t stats_record_play_ms(const char *uid_hex, uint32_t ms);

/** Bump pro_session_count and pro_total_ms. */
esp_err_t stats_record_pro_session(const char *uid_hex, uint32_t ms);

/** Visit every entry. Return false from cb to abort. */
typedef bool (*stats_visit_fn)(const char *uid_hex, const stats_entry_t *e, void *user);
esp_err_t stats_for_each(stats_visit_fn cb, void *user);

/** Clear the dirty flag after successful backend upload. */
esp_err_t stats_mark_uploaded(const char *uid_hex);

/** True if any entry has dirty=true. */
bool stats_has_dirty(void);

/** Force a flush of in-RAM cache to NVS. */
esp_err_t stats_flush(void);

#ifdef __cplusplus
}
#endif
