# System Architecture

> **Type:** System
> **Status:** firmware implemented; backend / web / mobile are spec-only

## What

Nadeem is a four-component system. The device is a dumb audio pipe; intelligence sits in the cloud. NFC tag placement triggers either local audio playback (regular semsem) or a live AI conversation (Semsem-Pro).

## Topology

```
        ┌────────────────────┐
        │   Mobile Companion │  one-shot onboarding only
        │   (spec-only)      │
        └─────────┬──────────┘
                  │ POST /provisioning-tokens
                  ▼
┌──────────────────────────────────────┐
│             Cloud Backend            │  Django + Postgres + Redis
│  /bootstrap  /semsem/{uid}/manifest  │  (spec-only)
│  /stats      wss://.../chat          │
│  /api/* (web)                        │
└────────┬───────────────┬─────────────┘
         │               │
         │               │ HTTPS / WSS, Bearer device_token
         ▼               ▼
   ┌─────────┐      ┌─────────────────────────────┐
   │  Web    │      │  Firmware (ESP32-S3)        │
   │ Frontend│      │  audio · semsem · led · hmi │
   │  Next.js│      │  onboarding · backend_client│
   │  RTL AR │      │  PN532 NFC · SD card cache  │
   │ (spec)  │      │  (implemented v0.1)         │
   └─────────┘      └─────────────────────────────┘
```

## Seams

- **Device ↔ Backend:** REST + WebSocket, Bearer `device_token` (see `pages/backend/contract.md`).
- **App ↔ Device:** local HTTP at `192.168.4.1` while in onboarding mode only (see `pages/mobile/local-api.md`).
- **App ↔ Backend:** user-session JWT; mints `provision_token` (see `pages/system/workflows.md` §bootstrap).
- **Web ↔ Backend:** session/JWT; read-only on stats and chat history; can revoke devices.

## Trust boundaries

1. SoftAP `192.168.4.1` is open + unauthenticated. Only valid while device is in onboarding mode (no Wi-Fi creds in NVS). Physical proximity = the only gate.
2. `provision_token` is single-use, ~10 min TTL — burned at first `POST /bootstrap`.
3. `device_token` is long-lived but revocable via `DELETE /api/devices/<id>`.
4. Child mic audio crosses the WebSocket — TLS mandatory in production.

## Where intelligence lives

| Concern | Owner |
|---------|-------|
| NFC read, audio playback, mic capture, LED | Firmware |
| Local cache of semsem audio | Firmware (SD card) |
| Track progress per UID | Firmware (NVS) |
| ASR / LLM / TTS | Backend |
| Content moderation, age-gating | Backend |
| Catalog management, parental dashboards | Backend + Web |
| Wi-Fi handoff | Mobile + Firmware SoftAP |

## Dependencies

→ [pages/firmware/overview.md](../firmware/overview.md)
→ [pages/backend/overview.md](../backend/overview.md)
→ [pages/system/workflows.md](workflows.md)
→ [graph/relationships.md](../../graph/relationships.md)

---
confidence: 0.92
sources: [S1, S2, S3, S7]
last_confirmed: 2026-04-24
status: partial
