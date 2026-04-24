# Entity Registry

> **Type:** Graph node catalog
> **Status:** current

Typed nodes for graph traversal per [AGENTS.md §2](../AGENTS.md). Edges in [relationships.md](relationships.md).

## Components (firmware)

- `audio` — type: Component
- `backend_client` — type: Component
- `board_io` — type: Component
- `bsp` — type: Component
- `console_cmds` — type: Component
- `hmi` — type: Component
- `led` — type: Component
- `led_strip_hw` — type: Component
- `nadeem_config` — type: Component
- `nadeem_events` — type: Component (bus)
- `nfc_pn532` — type: Component
- `onboarding` — type: Component
- `semsem` — type: Component
- `semsem_pro` — type: Component
- `smf` — type: Component (vendored)
- `stats` — type: Component

## Services

- `Cloud Backend` — type: Service · `backend/`
- `Audio CDN` — type: Service · referenced by manifest URLs
- `Web Frontend` — type: Service · `frontend/`
- `Mobile Companion App` — type: Service · `firmware/docs/mobile.md`

## Endpoints

### Device-facing (backend)
- `POST /bootstrap`
- `GET /semsem/{uid_hex}/manifest`
- `POST /stats`
- `WS /chat?role=&device=&semsem=`

### App-facing (backend)
- `POST /api/auth/login`
- `POST /api/provisioning-tokens`
- `GET /api/users/me/stats`
- `GET /api/semsems`
- `GET /api/semsems/<uid_hex>`
- `GET /api/chat-sessions/`
- `GET /api/chat-sessions/<id>`
- `GET /api/devices/`
- `DELETE /api/devices/<id>`

### Device-local (firmware)
- `GET /info`
- `GET /scan`
- `POST /provision`
- `POST /reset`

## Data models

- `User`
- `Device`
- `Semsem`
- `Manifest` (view)
- `UsageStats`
- `ProChatSession`
- `ProvisionToken`

## Identifiers

- `device_id`, `uid_hex`, `provision_token`, `device_token`, `role`

## Hardware

- `ESP32-S3` (Waveshare ESP32-S3-AUDIO-Board)
- `PN532 v3` (NFC reader)
- `ES7210` (mic codec)
- `ES8311` (DAC codec)
- `TCA9555` (IO expander)
- `LED strip` (6+1 RGB)
- `microSD`

## Events (firmware bus)

See [pages/firmware/events.md](../pages/firmware/events.md) — full list of `NEV_*` IDs.

## Constraints

- TLS-mandatory-prod
- RTL-arabic-only
- pcm16-16k-mono-20ms
- audio-files-le-20MiB
- one-uid-per-stats-post
- single-use-provision-token
- cumulative-stats-snapshots

## Decisions

See [`decisions/`](../decisions/) — each ADR is also an entity.

---
confidence: 0.9
sources: [S2, S3, S4, S7]
last_confirmed: 2026-04-25
status: implemented
