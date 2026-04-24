# Backend Overview

> **Type:** Service
> **Status:** spec-only

## What

Cloud HTTP+WebSocket service. Owns user accounts, device binding, audio CDN (or pre-signed URLs), AI chat pipeline (VAD/ASR/LLM/TTS), aggregate stats, and the parental dashboard's data plane.

## Stack (per PRD)

- **Language/framework:** Python + Django.
- **Database:** PostgreSQL.
- **Cache / queues:** Redis.
- **Infra:** fully Dockerized, `docker-compose` for dev and prod. TLS mandatory in prod.

## Two integration surfaces

1. **Device API** (firmware ↔ backend) — see [contract.md](contract.md). Bearer `device_token`. REST + one WebSocket.
2. **App API** (web frontend / mobile ↔ backend) — see [../frontend/api-mapping.md](../frontend/api-mapping.md). User session (JWT or cookie). REST.

## Minimum viable surface to be "Nadeem-compatible"

Per S3 §8:

1. `POST /provisioning-tokens` (user-auth)
2. `POST /bootstrap` (device token exchange)
3. `GET /semsem/{uid_hex}/manifest` (Bearer)
4. CDN (public or Bearer-guarded) for audio files
5. `POST /stats` (Bearer)
6. `wss://.../chat?role=...` (Bearer)

Everything else (parental dashboards, catalog editors, OTA, fleet management) is app↔backend and doesn't touch the firmware.

## Non-firmware surface (web/mobile)

| Endpoint | Purpose |
|----------|---------|
| `POST /api/auth/login` | user auth |
| `POST /api/provisioning-tokens` | mint provision token |
| `GET /api/users/me/stats` | aggregate stats for dashboard |
| `GET /api/semsems` | list bound UIDs |
| `GET /api/semsems/<uid_hex>` | semsem details |
| `GET /api/chat-sessions/` | chat history list |
| `GET /api/chat-sessions/<id>` | chat transcript |
| `GET /api/devices/` | bound devices |
| `DELETE /api/devices/<id>` | revoke device token |

## Dependencies

→ [contract.md](contract.md)
→ [data-model.md](data-model.md)
→ [chat-pipeline.md](chat-pipeline.md)
→ [security.md](security.md)

---
confidence: 0.7
sources: [S3, S7, S8]
last_confirmed: 2026-04-24
status: spec-only
