# Frontend ↔ Backend API Mapping

> **Type:** Endpoint set
> **Status:** spec-only

From PRD §4.2.

| Method | Endpoint | View | Notes |
|--------|----------|------|-------|
| POST | `/api/auth/login` | Login | session/JWT issued |
| POST | `/api/provisioning-tokens` | Add Device (Dashboard) | mints `provision_token` for mobile app |
| GET | `/api/users/me/stats` | Dashboard stats cards | aggregated rollups |
| GET | `/api/semsems` | Library grid | list bound UIDs |
| GET | `/api/semsems/<uid_hex>` | Semsem details | manifest data |
| GET | `/api/chat-sessions/` | Chat history list | reverse-chronological |
| GET | `/api/chat-sessions/<id>` | Chat detail | transcript |
| GET | `/api/devices/` | Device management | list bound devices, online status |
| DELETE | `/api/devices/<id>` | Device management | revoke `device_token` |

## Auth

User session/JWT. Same surface used by mobile app for `POST /api/provisioning-tokens`.

## Differences vs device API

- These never touch the device directly. All firmware-facing endpoints (`/bootstrap`, `/semsem/{uid}/manifest`, `/stats`, `/chat`) are user-agnostic; auth is `device_token`.
- App API uses standard JSON REST; no WebSocket on this surface.

## Open shape questions

- Stats response schema (rollups vs per-UID breakdown) — PRD silent.
- Catalog mutation endpoints (add/remove semsem, mark Pro) — referenced in `firmware/docs/mobile.md` §6 but not in PRD.

→ tracked in [../../lint.md](../../lint.md).

---
confidence: 0.7
sources: [S7]
last_confirmed: 2026-04-25
status: spec-only
