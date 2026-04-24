# Firmware Overview

> **Type:** Component-bundle
> **Status:** implemented (v0.1)

## What

ESP-IDF (≥ 5.0) firmware for ESP32-S3. Two boot paths: **onboarding** (no Wi-Fi creds in NVS) or **normal**. All cross-module communication goes through a single shared event bus (`nadeem_events`); modules use the SMF state machine framework.

## Boot path

From `firmware/main/main.c`:

1. `nvs_flash_init`, default event loop, `nadeem_events_init`.
2. `bsp_init(16000, 2, 16)` — codecs at 16 kHz, 2 ch capture, 16-bit.
3. `bsp_sdcard_mount("/sdcard", 10)`.
4. `board_io_init` — TCA9555 + button scanner.
5. `stats_init`, `led_start`, `console_cmds_start` — always on.
6. Branch on `nadeem_config_has_wifi()`:
   - **Yes:** subscribe `on_wifi_up`, start STA, start `hmi`, `audio`, `semsem`, `semsem_pro`.
   - **No:** start `onboarding` (SoftAP + HTTP server).
7. `on_wifi_up`: if no `device_token` but a `prov_token` exists in NVS, init backend client, redeem the prov token for a `device_token`, persist it, erase the prov token. Then init backend client with the token and post `NEV_SYS_BACKEND_UP`.

## Module map

| Layer | Modules |
|-------|---------|
| Hardware drivers | `bsp`, `board_io`, `nfc_pn532`, `led_strip_hw` |
| Persistence | `nadeem_config`, `stats` |
| Bus | `nadeem_events` |
| Domain | `audio`, `semsem`, `semsem_pro`, `hmi`, `led`, `onboarding`, `backend_client` |
| Dev | `console_cmds` |
| Framework | `smf` (vendored) |

## State machine pattern

Every domain module runs as an SMF state machine on its own task. Events from `nadeem_events` drive transitions. Per S6, this was an explicit architectural choice for readability/maintenance — see [decisions/0003-smf-state-machines.md](../../decisions/0003-smf-state-machines.md).

## Resilience

- Backend can be down: device plays cached semsems from `/sdcard/semsem/<uid>/`.
- Wi-Fi can drop: cached playback continues; LED enters `LED_WIFI_LOST`.
- `device_token` revoked: backend calls 4xx; firmware degrades to cache.
- Crash on `/provision`: TCP reset is normal — device reboots ~500 ms after sending `{ok:true}`.

## Build / flash

See [`firmware/VERIFICATION.md`](../../../firmware/VERIFICATION.md). Quick reference:

```sh
. $HOME/esp/esp-idf/export.sh
idf.py set-target esp32s3
idf.py build
idf.py -p /dev/ttyUSB0 flash monitor
```

Managed components pulled from Espressif registry: `esp_websocket_client`, `esp_audio_simple_player`, `esp_io_expander_tca95xx_16bit`, `led_strip`.

## Dependencies

→ [pages/firmware/components.md](components.md)
→ [pages/firmware/events.md](events.md)
→ [pages/firmware/state-machines.md](state-machines.md)
→ [pages/firmware/hardware.md](hardware.md)

---
confidence: 0.95
sources: [S1, S2, S5]
last_confirmed: 2026-04-24
status: implemented
