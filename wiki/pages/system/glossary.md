# Glossary

> **Type:** Reference
> **Status:** implemented (firmware-grounded)

| Term | Meaning |
|------|---------|
| **Nadeem** (نديم) | The product. Arabic for "companion". |
| **Semsem** (سمسم), pl. **Samasem** (سماسم) | Physical NFC tag/figurine placed on the box. Each maps to either an audio playlist or an AI role. |
| **Semsem-Pro** | A semsem with `pro=true` in its manifest. Triggers live AI chat instead of audio playback. |
| `device_id` | 12 hex chars, lowercase. Derived from STA MAC. Stable across reboots. |
| `uid_hex` | 2·N hex chars (N ≤ 10), lowercase. NFC tag UID hex-encoded, no separators. |
| `provision_token` | Opaque, single-use, ~10 min TTL. Issued by backend to mobile app; redeemed once by device at `POST /bootstrap`. |
| `device_token` | Opaque, long-lived, revocable. Bearer auth on every device→backend call after bootstrap. |
| `role` | Short string (≤ 31 bytes) on a Pro semsem manifest. E.g. `"doctor"`, `"engineer"`, `"storyteller"`. Forwarded as `?role=` on the chat WebSocket. |
| **Manifest** | `GET /semsem/{uid_hex}/manifest` JSON. Either `pro=false` + `tracks[]` or `pro=true` + `role`. |
| **NVS namespace `nadeem`** | Persistent key/value store on device for Wi-Fi creds, backend URL, `device_token`, `prov_token` (transient). |
| **NVS namespace `nadeem_stat`** | Per-UID usage counters. |
| **SoftAP `Nadeem-XXXX`** | Open Wi-Fi network the device broadcasts during onboarding. `XXXX` = last 4 hex of SoftAP MAC. |
| **SMF** | Espressif State Machine Framework. All firmware modules use this pattern (see `firmware/components/smf/`). |
| **BSP** | Board Support Package. Pin map, codec init, SD mount, I2S helpers. Specific to Waveshare ESP32-S3-AUDIO-Board. |
| **HMI** | Human-Machine Interface. Buttons + NFC reader → events on the bus. |
| **PCM16 / 16k mono** | Audio format for the chat WebSocket: signed 16-bit little-endian, 16 kHz, mono, 20 ms frames = 320 samples = 640 bytes. |

---
confidence: 0.95
sources: [S2, S3, S4]
last_confirmed: 2026-04-24
status: implemented
