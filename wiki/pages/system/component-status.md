# Component Status

> **Type:** Status snapshot
> **Status:** current as of 2026-04-24

| Component | Status | Source of truth |
|-----------|--------|-----------------|
| Firmware (ESP32-S3) | **implemented** v0.1 | `firmware/main/main.c`, `firmware/components/` |
| Cloud backend (Django/Postgres) | **partial** — admin platform + models + OTA endpoint implemented; remaining APIs spec-only | `prd_back_front.md`, `backend/backend.md`, `firmware/docs/backend.md` |
| Web frontend (Next.js) | spec-only + static HTML mockups | `prd_back_front.md`, `frontend/design.md` |
| Mobile companion app | spec-only | `firmware/docs/mobile.md`, `prd_back_front.md` |
| Audio CDN | spec-only | `firmware/docs/backend.md` §4.2 |

## Firmware sub-component readiness

| Module | State | Notes |
|--------|-------|-------|
| `bsp` | done | Pin map, codecs, SD mount |
| `board_io` | done | TCA9555 + 3-button scanner |
| `nadeem_events` | done | Bus, payloads, UID→hex helper |
| `nadeem_config` | done | NVS-backed Wi-Fi/URL/token |
| `nfc_pn532` | done | UART1, polling mode (IRQ wired but unused) |
| `hmi` | done | NFC poll task + button callback |
| `audio` | done with caveats | `start_byte` seek best-effort |
| `semsem` | done | Manifest cache, fetch, playback dispatch |
| `semsem_pro` | done | WebSocket client, mic stream, control frames |
| `led` + `led_strip_hw` | done | 10 modes, animations, host-testable patterns |
| `onboarding` | done | SoftAP + `/info`, `/scan`, `/provision`, `/reset` |
| `backend_client` | done with caveats | No TLS cert bundle wired (see limitations) |
| `stats` | done | NVS persistence, dirty flag, 5-min flush |
| `console_cmds` | done | REPL on UART0 |
| `smf` | done (vendored) | Espressif state machine framework |

---
confidence: 0.95
sources: [S1, S2, S5]
last_confirmed: 2026-04-24
status: implemented
