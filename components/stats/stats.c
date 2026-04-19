#include "stats.h"
#include "nvs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <string.h>
#include <time.h>

static const char *TAG = "stats";
#define NS "nadeem_stat"
#define MAX_ENTRIES 64

typedef struct {
    char           uid[24];       /* hex string, null-terminated */
    stats_entry_t  entry;
    bool           used;
} slot_t;

static slot_t           s_slots[MAX_ENTRIES];
static SemaphoreHandle_t s_lock;

static slot_t *find_slot(const char *uid) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (s_slots[i].used && strncmp(s_slots[i].uid, uid, sizeof s_slots[i].uid) == 0) {
            return &s_slots[i];
        }
    }
    return NULL;
}

static slot_t *alloc_slot(const char *uid) {
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (!s_slots[i].used) {
            s_slots[i].used = true;
            strncpy(s_slots[i].uid, uid, sizeof s_slots[i].uid - 1);
            s_slots[i].uid[sizeof s_slots[i].uid - 1] = '\0';
            memset(&s_slots[i].entry, 0, sizeof s_slots[i].entry);
            return &s_slots[i];
        }
    }
    return NULL;
}

static esp_err_t save_slot(const slot_t *s) {
    nvs_handle_t h;
    esp_err_t err = nvs_open(NS, NVS_READWRITE, &h);
    if (err != ESP_OK) return err;
    err = nvs_set_blob(h, s->uid, &s->entry, sizeof s->entry);
    if (err == ESP_OK) err = nvs_commit(h);
    nvs_close(h);
    return err;
}

static esp_err_t load_all(void) {
    nvs_handle_t h;
    esp_err_t err = nvs_open(NS, NVS_READONLY, &h);
    if (err == ESP_ERR_NVS_NOT_FOUND) return ESP_OK;
    if (err != ESP_OK) return err;

    nvs_iterator_t it = NULL;
    err = nvs_entry_find("nvs", NS, NVS_TYPE_BLOB, &it);
    while (err == ESP_OK && it) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        slot_t *s = alloc_slot(info.key);
        if (s) {
            size_t sz = sizeof s->entry;
            nvs_get_blob(h, info.key, &s->entry, &sz);
            s->entry.dirty = false;
        }
        err = nvs_entry_next(&it);
    }
    nvs_release_iterator(it);
    nvs_close(h);
    return ESP_OK;
}

esp_err_t stats_init(void) {
    s_lock = xSemaphoreCreateMutex();
    memset(s_slots, 0, sizeof s_slots);
    esp_err_t err = load_all();
    ESP_LOGI(TAG, "loaded stats (err=%d)", (int)err);
    return ESP_OK;
}

esp_err_t stats_get(const char *uid_hex, stats_entry_t *out) {
    if (!uid_hex || !out) return ESP_ERR_INVALID_ARG;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    slot_t *s = find_slot(uid_hex);
    if (s) {
        *out = s->entry;
    } else {
        memset(out, 0, sizeof *out);
    }
    xSemaphoreGive(s_lock);
    return ESP_OK;
}

static slot_t *get_or_alloc(const char *uid_hex) {
    slot_t *s = find_slot(uid_hex);
    if (!s) s = alloc_slot(uid_hex);
    return s;
}

esp_err_t stats_record_play_start(const char *uid_hex) {
    xSemaphoreTake(s_lock, portMAX_DELAY);
    slot_t *s = get_or_alloc(uid_hex);
    esp_err_t err = ESP_ERR_NO_MEM;
    if (s) {
        s->entry.play_count++;
        s->entry.last_played_unix = (uint64_t)time(NULL);
        s->entry.dirty = true;
        err = save_slot(s);
    }
    xSemaphoreGive(s_lock);
    return err;
}

esp_err_t stats_record_play_ms(const char *uid_hex, uint32_t ms) {
    xSemaphoreTake(s_lock, portMAX_DELAY);
    slot_t *s = get_or_alloc(uid_hex);
    if (s) {
        s->entry.total_play_ms += ms;
        s->entry.dirty = true;
    }
    xSemaphoreGive(s_lock);
    return s ? ESP_OK : ESP_ERR_NO_MEM;
}

esp_err_t stats_record_pro_session(const char *uid_hex, uint32_t ms) {
    xSemaphoreTake(s_lock, portMAX_DELAY);
    slot_t *s = get_or_alloc(uid_hex);
    esp_err_t err = ESP_ERR_NO_MEM;
    if (s) {
        s->entry.pro_session_count++;
        s->entry.pro_total_ms += ms;
        s->entry.last_played_unix = (uint64_t)time(NULL);
        s->entry.dirty = true;
        err = save_slot(s);
    }
    xSemaphoreGive(s_lock);
    return err;
}

esp_err_t stats_for_each(stats_visit_fn cb, void *user) {
    if (!cb) return ESP_ERR_INVALID_ARG;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (s_slots[i].used) {
            if (!cb(s_slots[i].uid, &s_slots[i].entry, user)) break;
        }
    }
    xSemaphoreGive(s_lock);
    return ESP_OK;
}

esp_err_t stats_mark_uploaded(const char *uid_hex) {
    xSemaphoreTake(s_lock, portMAX_DELAY);
    slot_t *s = find_slot(uid_hex);
    esp_err_t err = ESP_ERR_NOT_FOUND;
    if (s) {
        s->entry.dirty = false;
        err = save_slot(s);
    }
    xSemaphoreGive(s_lock);
    return err;
}

bool stats_has_dirty(void) {
    bool dirty = false;
    xSemaphoreTake(s_lock, portMAX_DELAY);
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (s_slots[i].used && s_slots[i].entry.dirty) { dirty = true; break; }
    }
    xSemaphoreGive(s_lock);
    return dirty;
}

esp_err_t stats_flush(void) {
    xSemaphoreTake(s_lock, portMAX_DELAY);
    for (int i = 0; i < MAX_ENTRIES; i++) {
        if (s_slots[i].used) save_slot(&s_slots[i]);
    }
    xSemaphoreGive(s_lock);
    return ESP_OK;
}
