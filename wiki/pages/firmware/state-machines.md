# State Machines

> **Type:** Reference
> **Status:** implemented (SMF pattern)

Modules use Espressif SMF. Public state enums shown; internal sub-states elided. Source: each module's `*.c` plus `include/*.h`.

## Audio (`audio_state_t`)

```
AUDIO_IDLE → AUDIO_LOADING → AUDIO_PLAYING ⇄ AUDIO_PAUSED
                                    ↓
                               AUDIO_ERROR
```

Triggers:
- `NEV_AUDIO_PLAY` → LOADING → PLAYING
- `NEV_AUDIO_PAUSE` from PLAYING → PAUSED
- `NEV_AUDIO_RESUME` from PAUSED → PLAYING
- `NEV_AUDIO_STOP` → IDLE
- `NEV_AUDIO_NEXT/PREV` → re-LOAD next track
- `NEV_AUDIO_TRACK_DONE` emitted when track ends; `semsem` advances playlist.

## LED (`led_mode_t`)

10 modes, each with a pure pattern function (host-testable):

| Mode | Trigger | Visual |
|------|---------|--------|
| `LED_BOOT` | startup | boot animation |
| `LED_ONBOARDING` | `NEV_SYS_ONBOARDING_ACTIVE` | blue pulse |
| `LED_IDLE` | default | low ambient |
| `LED_SEMSEM_LOADING` | `NEV_SYS_SEMSEM_LOADING` | rotating head |
| `LED_PLAYING` | `NEV_AUDIO_STATE` PLAYING | breathing green |
| `LED_PAUSED` | `NEV_AUDIO_STATE` PAUSED | dim hold |
| `LED_PRO_LISTENING` | `NEV_SYS_PRO_LISTENING` | purple breath |
| `LED_PRO_SPEAKING` | `NEV_SYS_PRO_SPEAKING` | purple rotating |
| `LED_ERROR` | `NEV_SYS_ERROR` | red blink |
| `LED_WIFI_LOST` | `NEV_SYS_WIFI_DOWN` | amber |

## Semsem

State boundary: idle ↔ loading ↔ playing ↔ pro-active.

Transitions:
- `NEV_HMI_SEMSEM_PLACED {uid}`:
  - check `/sdcard/semsem/<uid>/manifest.json`
  - cache miss → `NEV_SYS_SEMSEM_LOADING`, `backend_client_fetch_semsem_manifest`, download tracks, save manifest
  - `pro=true` → emit `NEV_SYS_MODE_PRO_ENTER {uid, role}` (does NOT play tracks)
  - `pro=false` → emit `NEV_AUDIO_PLAY {path, start_byte}` for first track at saved progress
- `NEV_HMI_SEMSEM_REMOVED` → `NEV_AUDIO_STOP` and (if pro) close socket via `NEV_SYS_MODE_PRO_EXIT`

## Semsem-Pro

- Idle until `NEV_SYS_MODE_PRO_ENTER`.
- Connects WebSocket, spawns mic-stream task and downstream-audio task.
- On binary frame: write to DAC via `bsp_i2s_write`.
- On text frame: scan for substrings `"listening"` / `"speaking"`, emit `NEV_SYS_PRO_*` (which `led` consumes).
- Closes on `NEV_SYS_MODE_PRO_EXIT`, `WEBSOCKET_EVENT_DISCONNECTED`, `WEBSOCKET_EVENT_ERROR`.

## Onboarding

- Active only when `nadeem_config_has_wifi() == false` at boot.
- Brings up SoftAP, HTTP server.
- On valid `POST /provision` → write Wi-Fi + `prov_token` to NVS → `NEV_SYS_ONBOARDING_DONE` → reboot.

---
confidence: 0.9
sources: [S2]
last_confirmed: 2026-04-24
status: implemented
