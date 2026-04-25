#include "nadeem_events.h"
#include <stdio.h>

ESP_EVENT_DEFINE_BASE(NADEEM_EVENTS);

static esp_event_loop_handle_t s_loop;

esp_err_t nadeem_events_init(void) {
    if (s_loop) return ESP_OK;
    esp_event_loop_args_t args = {
        .queue_size      = 32,
        .task_name       = "nev_loop",
        .task_priority   = 5,
        .task_stack_size = 4096,
        .task_core_id    = 0,
    };
    return esp_event_loop_create(&args, &s_loop);
}

esp_event_loop_handle_t nadeem_events_loop(void) {
    return s_loop;
}

esp_err_t nadeem_events_post(nev_id_t id, const void *data, size_t data_sz, TickType_t ticks) {
    return esp_event_post_to(s_loop, NADEEM_EVENTS, (int32_t)id, (void *)data, data_sz, ticks);
}

esp_err_t nadeem_events_on(nev_id_t id, esp_event_handler_t cb, void *arg) {
    return esp_event_handler_register_with(s_loop, NADEEM_EVENTS, (int32_t)id, cb, arg);
}

void nadeem_uid_to_hex(const nev_semsem_uid_t *uid, char *buf, size_t buf_len) {
    static const char hex[] = "0123456789abcdef";
    size_t n = uid->uid_len;
    if (buf_len < 2 * n + 1) { if (buf_len) buf[0] = '\0'; return; }
    for (size_t i = 0; i < n; i++) {
        buf[2 * i]     = hex[(uid->uid[i] >> 4) & 0xF];
        buf[2 * i + 1] = hex[uid->uid[i] & 0xF];
    }
    buf[2 * n] = '\0';
}
