# Firmware v0.1 Known Limitations

> **Type:** Constraint
> **Status:** current

Per `firmware/VERIFICATION.md` §6 and `firmware/docs/backend.md`.

## Audio

- `audio` seek to `start_byte` relies on `esp_audio_simple_player` seeking — best-effort. Some MP3s restart from byte 0. Track *index* progress is preserved; in-track byte progress is not guaranteed.

## Wake-word / on-device ASR

- `esp_sr` was removed. No Arabic wake-word detection on-device. All speech understanding happens on the backend during Pro mode only.

## TLS

- `backend_client` does **not** attach a TLS cert bundle in v0.1. Production must enable `CONFIG_MBEDTLS_CERTIFICATE_BUNDLE=y` and add `.crt_bundle_attach = esp_crt_bundle_attach` in `backend_client.c`.

## WebSocket auth

- ESP-IDF websocket client supports `Authorization` header via `headers` config, but v0.1 does not wire it explicitly. Backend should accept the device token as a query param fallback (`&token=<device_token>`) until the header path is added.

## Manifest cache

- Changing a manifest server-side does not invalidate the local SD cache. v0.1 workaround: factory reset (long-press PLAY/PAUSE). Future: manifest version header + background refresh.

## Stats

- `POST /stats` uploads **one UID per request**, not a batch. Expect bursts when multiple UIDs are dirty at the 5-min tick.
- `last_played_unix` may be 0 or pre-2000 if NTP hasn't synced — backend must treat as "unknown" rather than reject.

## Onboarding

- No captive portal. App is responsible for opening the right URL.
- No mDNS during onboarding.
- `backend_url` is **not** part of the `/provision` form. Set via console or pre-baked in v0.1. Mobile integration needs to add this.

## Resume

- **SD mode:** a partial audio download is discarded on next boot if decode fails; firmware re-fetches. No in-track byte-level seek — `start_byte` in `nev_audio_play_t` initialises the position counter only.
- **Stream mode:** resume after NFC removal uses `Range: bytes=N-` HTTP header where N is the compressed-byte offset tracked by `stream_read_cb`. CDNs that return `200` (instead of `206`) for range requests fall back to playing from the start of the track. Track-index resume (i.e., continuing from track 3 rather than track 1) works in both modes.

## Stream mode (no SD card)

- Requires active Wi-Fi + backend reachability. No offline fallback.
- Manifest is cached in NVS namespace `nd_manifest`. Manifest change on the backend is not reflected until NVS is cleared (factory reset) or the entry expires (no auto-expiry in v0.1).
- TLS sessions during HTTPS streaming increase heap pressure. `audio_sm` task stack is 8 KiB to accommodate. Monitor `heap_caps_get_free_size` on long streams.

---
confidence: 0.95
sources: [S5, S3, S2]
last_confirmed: 2026-04-26
status: implemented
