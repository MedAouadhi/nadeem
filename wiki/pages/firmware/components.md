# Firmware Components

> **Type:** Component registry
> **Status:** implemented

One row per component under `firmware/components/`. Public surface from each `include/*.h`.

| Component | Public API (selected) | Responsibility |
|-----------|-----------------------|----------------|
| `bsp` | `bsp_init`, `bsp_sdcard_mount`, `bsp_i2s_write`, `bsp_i2s_read_mic`, `bsp_set_volume`, `bsp_list_files` | Board pinout, codecs (ES7210 mic, ES8311 DAC), SD mount, I2S helpers. Specific to Waveshare ESP32-S3-AUDIO-Board. |
| `board_io` | `board_io_init`, `board_io_expander_set/read`, `board_io_buttons_register` | TCA9555 IO expander + 3 buttons (pins 9/10/11) with short/long press detection. |
| `nadeem_events` | `nadeem_events_init`, `nadeem_events_post`, `nadeem_events_on`, `nadeem_uid_to_hex` | Shared `esp_event` loop and ID/payload definitions. See [events.md](events.md). |
| `nadeem_config` | `nadeem_config_has_wifi`, `*_get/set_wifi`, `*_get/set_backend_url`, `*_get/set_device_token`, `*_clear`, `nadeem_wifi_sta_start_from_nvs` | NVS namespace `nadeem`. Persists Wi-Fi creds, backend URL, device token. |
| `nfc_pn532` | (see component test) | UART1 driver for PN532, ISO14443A polling, frame/ACK parsing. |
| `hmi` | `hmi_start` | Owns NFC poll task + button callback. Emits `NEV_HMI_*` events. |
| `audio` | `audio_start`, `audio_current_state`, `audio_current_position`, `audio_set/get_volume` | SMF task. Reacts to `NEV_AUDIO_PLAY/STOP/PAUSE/RESUME/NEXT/PREV`. Emits `NEV_AUDIO_STATE`, `NEV_AUDIO_TRACK_DONE`. Wraps `esp_audio_simple_player`. |
| `semsem` | `semsem_start`, `semsem_trigger_uid` | SMF task. On placement: cache check → manifest fetch → download → play. Tracks per-track progress for resume. |
| `semsem_pro` | `semsem_pro_start` | On `NEV_SYS_MODE_PRO_ENTER`: opens `wss://.../chat`, streams mic up + TTS down. Parses control frames for LED. |
| `led` + `led_strip_hw` | `led_start`, `led_set_mode`, `led_get_mode`, pure `led_pattern_*` functions | 10 modes, animation tick. Patterns are pure — host-testable. |
| `onboarding` | `onboarding_start`, `onboarding_parse_form` | SoftAP `Nadeem-XXXX` + HTTP server on `192.168.4.1`. Endpoints `/info`, `/scan`, `/provision`, `/reset`. |
| `backend_client` | `backend_client_init`, `*_fetch_semsem_manifest`, `*_download_file`, `*_upload_stats`, `*_bootstrap` | HTTP client wrapped around `esp_http_client`. Bearer auth. No TLS bundle wired (see [limitations.md](limitations.md)). |
| `stats` | `stats_init`, `stats_get`, `stats_record_play_start`, `stats_record_play_ms`, `stats_record_pro_session`, `stats_for_each`, `stats_mark_uploaded`, `stats_has_dirty`, `stats_flush` | NVS namespace `nadeem_stat`. RAM cache + dirty flag + 5-min flush. |
| `console_cmds` | `console_cmds_start` | esp_console REPL on UART0; commands listed in [console.md](console.md). |
| `smf` | vendored | Espressif State Machine Framework. |

## Component dependency sketch

```
main
 ├─ bsp ─┬─ I2C → board_io
 │       └─ I2S, SD, codec
 ├─ nadeem_events  (used by all domain modules)
 ├─ nadeem_config  (used by main, onboarding, backend_client)
 ├─ stats          (used by semsem, audio, backend_client)
 ├─ led ── led_strip_hw
 ├─ hmi ── nfc_pn532
 ├─ audio
 ├─ semsem ── backend_client, audio, stats, sdcard
 ├─ semsem_pro ── backend_client (WS), bsp (I2S read/write)
 └─ onboarding (only when no Wi-Fi creds)
```

## Test coverage (per S5)

- `nadeem_events`: post/dispatch, UID→hex.
- `nfc_pn532`: SAMConfiguration framing, response parsing, ACK recognition.
- `stats`: play-count persistence, dirty flag lifecycle.
- `led`: rotating-head and blink patterns (pure functions).
- `onboarding`: form parser decodes `%20`/`+`, recognises all fields.

---
confidence: 0.95
sources: [S2]
last_confirmed: 2026-04-24
status: implemented
