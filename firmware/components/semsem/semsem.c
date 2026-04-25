#include "semsem.h"
#include "nadeem_events.h"
#include "backend_client.h"
#include "stats.h"
#include "smf/smf.h"
#include "cJSON.h"

#include "nvs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>

static const char *TAG = "semsem";

#define MAX_TRACKS 64
#define MAX_PATH_LEN 128
#define MAX_URL_LEN 192
#define NVS_PROGRESS_NS "nd_progress"

/* ---------- SMF states ------------------------------------------------ */

typedef enum { S_IDLE, S_LOOKUP, S_FETCHING, S_PLAYING, S_ERROR } semsem_state_t;

typedef struct {
    uint32_t track_idx;
    uint32_t pos_bytes;
} progress_t;

typedef struct {
    char name[32];
    char url[MAX_URL_LEN];
} track_t;

typedef struct {
    nev_semsem_uid_t uid;
    char             uid_hex[24];
    bool             is_pro;
    char             pro_role[32];
    uint16_t         track_count;
    track_t          tracks[MAX_TRACKS];
    progress_t       progress;
} context_t;

/* Commands posted to the SMF worker. */
typedef enum { C_PLACED, C_REMOVED, C_BTN_PREV, C_BTN_NEXT, C_BTN_PP, C_AUDIO_STATE, C_AUDIO_DONE } cmd_kind_t;
typedef struct {
    cmd_kind_t       kind;
    nev_semsem_uid_t uid;
    nev_audio_state_t audio;
} cmd_t;

static struct smf_ctx s_ctx;
static QueueHandle_t  s_q;
static context_t      s_cur;

enum { I_IDLE, I_LOOKUP, I_FETCHING, I_PLAYING, I_ERROR };
static const struct smf_state s_states[];

/* ---------- Helpers --------------------------------------------------- */

static void save_progress(const char *uid_hex, const progress_t *p) {
    nvs_handle_t h;
    if (nvs_open(NVS_PROGRESS_NS, NVS_READWRITE, &h) != ESP_OK) return;
    nvs_set_blob(h, uid_hex, p, sizeof *p);
    nvs_commit(h);
    nvs_close(h);
}

static void load_progress(const char *uid_hex, progress_t *p) {
    nvs_handle_t h;
    memset(p, 0, sizeof *p);
    if (nvs_open(NVS_PROGRESS_NS, NVS_READONLY, &h) != ESP_OK) return;
    size_t sz = sizeof *p;
    nvs_get_blob(h, uid_hex, p, &sz);
    nvs_close(h);
}

static bool file_exists(const char *path) {
    struct stat s;
    return stat(path, &s) == 0 && S_ISREG(s.st_mode);
}

static esp_err_t load_manifest_from_sd(const char *uid_hex, context_t *ctx) {
    char path[MAX_PATH_LEN];
    snprintf(path, sizeof path, "/sdcard/semsem/%s/manifest.json", uid_hex);
    FILE *f = fopen(path, "rb");
    if (!f) return ESP_ERR_NOT_FOUND;

    char buf[2048];
    size_t n = fread(buf, 1, sizeof buf - 1, f);
    fclose(f);
    if (n == 0) return ESP_FAIL;
    buf[n] = '\0';

    cJSON *root = cJSON_Parse(buf);
    if (!root) return ESP_ERR_INVALID_RESPONSE;

    cJSON *pro = cJSON_GetObjectItem(root, "pro");
    ctx->is_pro = cJSON_IsTrue(pro);
    cJSON *role = cJSON_GetObjectItem(root, "role");
    if (cJSON_IsString(role) && role->valuestring)
        strncpy(ctx->pro_role, role->valuestring, sizeof ctx->pro_role - 1);

    cJSON *tracks = cJSON_GetObjectItem(root, "tracks");
    ctx->track_count = 0;
    if (cJSON_IsArray(tracks)) {
        cJSON *t;
        cJSON_ArrayForEach(t, tracks) {
            if (ctx->track_count >= MAX_TRACKS) break;
            cJSON *name = cJSON_GetObjectItem(t, "name");
            if (cJSON_IsString(name) && name->valuestring) {
                strncpy(ctx->tracks[ctx->track_count].name,
                        name->valuestring,
                        sizeof ctx->tracks[0].name - 1);
                ctx->track_count++;
            }
        }
    }
    cJSON_Delete(root);
    return ESP_OK;
}

static esp_err_t ensure_manifest_and_files(context_t *ctx) {
    char dir[MAX_PATH_LEN];
    snprintf(dir, sizeof dir, "/sdcard/semsem/%s", ctx->uid_hex);
    mkdir("/sdcard/semsem", 0755);
    mkdir(dir, 0755);

    /* Try local manifest first. */
    if (load_manifest_from_sd(ctx->uid_hex, ctx) == ESP_OK && ctx->track_count > 0) {
        return ESP_OK;
    }

    /* Fetch from backend. Emit a loading event so the LED reacts. */
    nadeem_events_post(NEV_SYS_SEMSEM_LOADING, &ctx->uid, sizeof ctx->uid, 0);

    char json[2048];
    esp_err_t err = backend_client_fetch_semsem_manifest(ctx->uid_hex, json, sizeof json);
    if (err != ESP_OK) return err;

    char path[MAX_PATH_LEN];
    snprintf(path, sizeof path, "%s/manifest.json", dir);
    FILE *f = fopen(path, "wb");
    if (f) { fwrite(json, 1, strlen(json), f); fclose(f); }

    cJSON *root = cJSON_Parse(json);
    if (!root) return ESP_ERR_INVALID_RESPONSE;

    cJSON *pro = cJSON_GetObjectItem(root, "pro");
    ctx->is_pro = cJSON_IsTrue(pro);
    cJSON *role = cJSON_GetObjectItem(root, "role");
    if (cJSON_IsString(role) && role->valuestring)
        strncpy(ctx->pro_role, role->valuestring, sizeof ctx->pro_role - 1);

    cJSON *tracks = cJSON_GetObjectItem(root, "tracks");
    ctx->track_count = 0;
    if (cJSON_IsArray(tracks)) {
        cJSON *t;
        cJSON_ArrayForEach(t, tracks) {
            if (ctx->track_count >= MAX_TRACKS) break;
            cJSON *name = cJSON_GetObjectItem(t, "name");
            cJSON *url  = cJSON_GetObjectItem(t, "url");
            if (cJSON_IsString(name) && cJSON_IsString(url)) {
                strncpy(ctx->tracks[ctx->track_count].name,
                        name->valuestring, sizeof ctx->tracks[0].name - 1);
                strncpy(ctx->tracks[ctx->track_count].url,
                        url->valuestring,  sizeof ctx->tracks[0].url  - 1);

                char fpath[MAX_PATH_LEN];
                snprintf(fpath, sizeof fpath, "%s/%s", dir, name->valuestring);
                if (!file_exists(fpath)) {
                    if (backend_client_download_file(ctx->tracks[ctx->track_count].url,
                                                     fpath) != ESP_OK) {
                        ESP_LOGW(TAG, "download failed: %s", ctx->tracks[ctx->track_count].url);
                    }
                }
                ctx->track_count++;
            }
        }
    }
    cJSON_Delete(root);
    return ctx->track_count ? ESP_OK : ESP_FAIL;
}

static void post_play_for(uint32_t idx, uint32_t start_byte) {
    if (idx >= s_cur.track_count) return;
    nev_audio_play_t p = { .start_byte = start_byte };
    snprintf(p.path, sizeof p.path, "/sdcard/semsem/%s/%s",
             s_cur.uid_hex, s_cur.tracks[idx].name);
    s_cur.progress.track_idx = idx;
    s_cur.progress.pos_bytes = start_byte;
    save_progress(s_cur.uid_hex, &s_cur.progress);
    nadeem_events_post(NEV_AUDIO_PLAY, &p, sizeof p, 0);
}

/* ---------- SMF handlers --------------------------------------------- */

static void idle_entry(void *obj) { (void)obj; }

static void lookup_entry(void *obj) {
    nadeem_uid_to_hex(&s_cur.uid, s_cur.uid_hex, sizeof s_cur.uid_hex);
    stats_record_play_start(s_cur.uid_hex);

    esp_err_t err = ensure_manifest_and_files(&s_cur);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "manifest unavailable for %s", s_cur.uid_hex);
        smf_set_state(SMF_CTX(obj), &s_states[I_ERROR]);
        return;
    }
    if (s_cur.is_pro) {
        nev_pro_enter_t ev = { .uid = s_cur.uid };
        strncpy(ev.role, s_cur.pro_role, sizeof ev.role - 1);
        nadeem_events_post(NEV_SYS_MODE_PRO_ENTER, &ev, sizeof ev, 0);
        smf_set_state(SMF_CTX(obj), &s_states[I_IDLE]);
        return;
    }
    smf_set_state(SMF_CTX(obj), &s_states[I_PLAYING]);
}

static void playing_entry(void *obj) {
    (void)obj;
    load_progress(s_cur.uid_hex, &s_cur.progress);
    if (s_cur.progress.track_idx >= s_cur.track_count) s_cur.progress.track_idx = 0;
    post_play_for(s_cur.progress.track_idx, s_cur.progress.pos_bytes);
}

static void error_entry(void *obj) {
    (void)obj;
    nev_error_t e = { .source = 2, .code = -1 };
    nadeem_events_post(NEV_SYS_ERROR, &e, sizeof e, 0);
}

static void common_run(void *obj) {
    cmd_t c;
    if (xQueueReceive(s_q, &c, 0) != pdTRUE) return;

    switch (c.kind) {
        case C_PLACED:
            s_cur.uid = c.uid;
            memset(&s_cur.progress, 0, sizeof s_cur.progress);
            smf_set_state(SMF_CTX(obj), &s_states[I_LOOKUP]);
            break;

        case C_REMOVED:
            nadeem_events_post(NEV_AUDIO_STOP, NULL, 0, 0);
            save_progress(s_cur.uid_hex, &s_cur.progress);
            if (s_cur.is_pro) {
                nadeem_events_post(NEV_SYS_MODE_PRO_EXIT, NULL, 0, 0);
            }
            stats_record_play_ms(s_cur.uid_hex, s_cur.progress.pos_bytes / 32);  /* rough estimate */
            memset(&s_cur, 0, sizeof s_cur);
            smf_set_state(SMF_CTX(obj), &s_states[I_IDLE]);
            break;

        case C_BTN_NEXT:
            if (s_cur.track_count && s_cur.progress.track_idx + 1 < s_cur.track_count) {
                post_play_for(s_cur.progress.track_idx + 1, 0);
            }
            break;

        case C_BTN_PREV:
            if (s_cur.progress.track_idx > 0) {
                post_play_for(s_cur.progress.track_idx - 1, 0);
            } else if (s_cur.track_count) {
                post_play_for(0, 0);
            }
            break;

        case C_BTN_PP: {
            /* Toggle pause/resume via audio module (it handles its own state). */
            nadeem_events_post(NEV_AUDIO_PAUSE, NULL, 0, 0);
            /* Audio module ignores PAUSE if not playing and RESUME if not paused,
             * so posting both in sequence would race. Simpler: always post PAUSE
             * when currently PLAYING, else RESUME. Semsem tracks audio state from
             * NEV_AUDIO_STATE events below. */
            break;
        }

        case C_AUDIO_STATE:
            /* Throttled progress-save: when bytes moved, stash them. */
            s_cur.progress.pos_bytes = c.audio.pos_bytes;
            if ((c.audio.pos_bytes & 0xFFFF) == 0) {
                save_progress(s_cur.uid_hex, &s_cur.progress);
            }
            break;

        case C_AUDIO_DONE:
            if (s_cur.progress.track_idx + 1 < s_cur.track_count) {
                post_play_for(s_cur.progress.track_idx + 1, 0);
            } else {
                /* End of playlist: wrap to beginning and stop. */
                s_cur.progress.track_idx = 0;
                s_cur.progress.pos_bytes = 0;
                save_progress(s_cur.uid_hex, &s_cur.progress);
            }
            break;
    }
}

static const struct smf_state s_states[] = {
    [I_IDLE]     = SMF_CREATE_STATE(idle_entry,    common_run, NULL, NULL, NULL),
    [I_LOOKUP]   = SMF_CREATE_STATE(lookup_entry,  common_run, NULL, NULL, NULL),
    [I_FETCHING] = SMF_CREATE_STATE(NULL,          common_run, NULL, NULL, NULL),
    [I_PLAYING]  = SMF_CREATE_STATE(playing_entry, common_run, NULL, NULL, NULL),
    [I_ERROR]    = SMF_CREATE_STATE(error_entry,   common_run, NULL, NULL, NULL),
};

/* ---------- Event bus handlers --------------------------------------- */

static void on_event(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg; (void)base;
    cmd_t c = {0};
    switch (id) {
        case NEV_HMI_SEMSEM_PLACED:
            c.kind = C_PLACED;
            if (data) memcpy(&c.uid, data, sizeof c.uid);
            break;
        case NEV_HMI_SEMSEM_REMOVED:
            c.kind = C_REMOVED;
            break;
        case NEV_HMI_BTN_PREV:       c.kind = C_BTN_PREV; break;
        case NEV_HMI_BTN_NEXT:       c.kind = C_BTN_NEXT; break;
        case NEV_HMI_BTN_PLAY_PAUSE: c.kind = C_BTN_PP;   break;
        case NEV_AUDIO_STATE:
            c.kind = C_AUDIO_STATE;
            if (data) memcpy(&c.audio, data, sizeof c.audio);
            break;
        case NEV_AUDIO_TRACK_DONE:   c.kind = C_AUDIO_DONE; break;
        default: return;
    }
    xQueueSend(s_q, &c, 0);
}

static void semsem_task(void *arg) {
    (void)arg;
    smf_set_initial(&s_ctx, &s_states[I_IDLE]);
    while (1) {
        smf_run_state(&s_ctx);
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

esp_err_t semsem_start(void) {
    s_q = xQueueCreate(16, sizeof(cmd_t));
    if (!s_q) return ESP_ERR_NO_MEM;

    nadeem_events_on(NEV_HMI_SEMSEM_PLACED,  on_event, NULL);
    nadeem_events_on(NEV_HMI_SEMSEM_REMOVED, on_event, NULL);
    nadeem_events_on(NEV_HMI_BTN_PREV,       on_event, NULL);
    nadeem_events_on(NEV_HMI_BTN_NEXT,       on_event, NULL);
    nadeem_events_on(NEV_HMI_BTN_PLAY_PAUSE, on_event, NULL);
    nadeem_events_on(NEV_AUDIO_STATE,        on_event, NULL);
    nadeem_events_on(NEV_AUDIO_TRACK_DONE,   on_event, NULL);

    BaseType_t ok = xTaskCreatePinnedToCore(semsem_task, "semsem_sm",
                                            6144, NULL, 4, NULL, 1);
    return ok == pdPASS ? ESP_OK : ESP_ERR_NO_MEM;
}

esp_err_t semsem_trigger_uid(const char *uid_hex) {
    if (!uid_hex) return ESP_ERR_INVALID_ARG;
    cmd_t c = { .kind = C_PLACED };
    /* UID hex → bytes */
    size_t hex_len = strlen(uid_hex);
    if (hex_len > 20) hex_len = 20;
    c.uid.uid_len = (uint8_t)(hex_len / 2);
    for (size_t i = 0; i < c.uid.uid_len; i++) {
        unsigned b;
        if (sscanf(&uid_hex[i * 2], "%2x", &b) != 1) return ESP_ERR_INVALID_ARG;
        c.uid.uid[i] = (uint8_t)b;
    }
    xQueueSend(s_q, &c, 0);
    return ESP_OK;
}
