# Event Bus

> **Type:** Reference
> **Status:** implemented

Single shared `esp_event` loop. Base = `NADEEM_EVENTS`. Header: `firmware/components/nadeem_events/include/nadeem_events.h`.

Handlers must treat payload pointers as read-only and copy what they need before returning.

## Event IDs

### HMI → system

| ID | Payload | Producer | Consumers |
|----|---------|----------|-----------|
| `NEV_HMI_SEMSEM_PLACED` | `nev_semsem_uid_t` | `hmi` (NFC poll) | `semsem`, `led` |
| `NEV_HMI_SEMSEM_REMOVED` | `nev_semsem_uid_t` | `hmi` | `semsem`, `audio`, `semsem_pro` |
| `NEV_HMI_BTN_PREV` | none | `board_io` | `audio` |
| `NEV_HMI_BTN_PLAY_PAUSE` | none | `board_io` | `audio` |
| `NEV_HMI_BTN_NEXT` | none | `board_io` | `audio` |
| `NEV_HMI_BTN_LONG_RESET` | none | `board_io` | factory reset path |

### Control → audio

| ID | Payload |
|----|---------|
| `NEV_AUDIO_PLAY` | `nev_audio_play_t {path[128], start_byte}` |
| `NEV_AUDIO_STOP` | none |
| `NEV_AUDIO_PAUSE` | none |
| `NEV_AUDIO_RESUME` | none |
| `NEV_AUDIO_NEXT` | none |
| `NEV_AUDIO_PREV` | none |

### Audio → listeners

| ID | Payload |
|----|---------|
| `NEV_AUDIO_STATE` | `nev_audio_state_t {state, pos_bytes}` |
| `NEV_AUDIO_TRACK_DONE` | none |

### System

| ID | Payload | Producer |
|----|---------|----------|
| `NEV_SYS_WIFI_UP` | none | wifi event handler in `main.c` |
| `NEV_SYS_WIFI_DOWN` | none | wifi event handler |
| `NEV_SYS_BACKEND_UP` | none | `main.c::on_wifi_up` |
| `NEV_SYS_BACKEND_DOWN` | none | `backend_client` (on repeated failures) |
| `NEV_SYS_SEMSEM_LOADING` | `nev_semsem_uid_t` | `semsem` |
| `NEV_SYS_MODE_PRO_ENTER` | `nev_pro_enter_t {uid, role[32]}` | `semsem` |
| `NEV_SYS_MODE_PRO_EXIT` | none | `semsem_pro` |
| `NEV_SYS_PRO_LISTENING` | none | `semsem_pro` (from backend control frame) |
| `NEV_SYS_PRO_SPEAKING` | none | `semsem_pro` |
| `NEV_SYS_ERROR` | `nev_error_t {source, code}` | any |
| `NEV_SYS_ONBOARDING_ACTIVE` | none | `onboarding` |
| `NEV_SYS_ONBOARDING_DONE` | none | `onboarding` |

## Helpers

```c
void nadeem_uid_to_hex(const nev_semsem_uid_t *uid, char *buf, size_t buf_len);
// Writes lowercase hex, null-terminated. buf_len ≥ 2*uid_len + 1.
```

---
confidence: 0.97
sources: [S2]
last_confirmed: 2026-04-24
status: implemented
