# Device ↔ Backend Contract

> **Type:** Endpoint set
> **Status:** firmware client implemented; backend spec-only

Authoritative reference for the surface the firmware actually calls. From `firmware/components/backend_client/` + S3.

Base URL is configured per device at onboarding (NVS key `backend_url`). Examples assume `https://api.nadeem.example`.

## Auth

- All device endpoints except `POST /bootstrap` require `Authorization: Bearer <device_token>`.
- WebSocket: same Bearer in upgrade-request headers; backend may also accept `?token=<device_token>` query as fallback (v0.1 firmware caveat — see [../firmware/limitations.md](../firmware/limitations.md)).

## Endpoint table

| Method | Path | Auth | Body | Response |
|--------|------|------|------|----------|
| POST | `/bootstrap` | none | `{provision_token}` | `{device_token, device_id}` |
| GET | `/semsem/{uid_hex}/manifest` | Bearer | — | manifest JSON (see below) |
| GET | `{absolute track URL}` | Bearer (or none on CDN) | — | audio bytes |
| POST | `/stats` | Bearer | per-UID counter snapshot | 2xx on success |
| WS | `/chat?role=&device=&semsem=` | Bearer | binary PCM16 / text JSON | bidirectional binary + optional control text |

## `POST /bootstrap`

```http
POST /bootstrap
Content-Type: application/json
{ "provision_token": "..." }
```

Response 200:
```json
{ "device_token": "n_dk_live_…", "device_id": "aabbccddeeff" }
```

Backend responsibilities:
- Validate `provision_token`. Single-use, ~10 min TTL.
- Bind `device_id` to the user that minted the prov token.
- Invalidate `provision_token` after exchange.
- Return long-lived, revocable `device_token`.
- Rate-limit (lost device shouldn't DoS).

## `GET /semsem/{uid_hex}/manifest`

Response 200, content-type `application/json`:

```json
{
  "uid": "04a3f91b2c",
  "title": "Animal Sounds — volume 1",
  "pro": false,
  "role": "",
  "tracks": [
    { "name": "01.mp3", "url": "https://cdn.../01.mp3" },
    { "name": "02.mp3", "url": "https://cdn.../02.mp3" }
  ]
}
```

| Field | Required | Notes |
|-------|----------|-------|
| `pro` | yes | `true` → firmware does NOT play tracks; opens chat WebSocket using `role`. |
| `role` | when `pro=true` | ≤ 31 bytes. e.g. `"doctor"`, `"engineer"`, `"storyteller"`. |
| `tracks[].name` | yes if not pro | Used as filename under `/sdcard/semsem/<uid_hex>/`. FAT-safe. |
| `tracks[].url` | yes if not pro | Absolute. CDN may be public; firmware sends Bearer regardless. |

Status semantics:
- 404 → unknown tag, firmware shows error LED.
- 403 → device not allowed (parental restriction). Same UX as 404.
- 5xx / network error → firmware falls back to SD cache; if cache empty, error.

## `POST /stats`

```json
{
  "uid": "04a3f91b2c",
  "play_count": 7,
  "total_play_ms": 284300,
  "last_played_unix": 1734551234,
  "pro_session_count": 2,
  "pro_total_ms": 61000
}
```

- One UID per request (v0.1).
- Counters are **cumulative snapshots**, not deltas. Backend stores last value per `(device_id, uid)` and computes deltas server-side.
- `last_played_unix` may be 0 / pre-2000 → "unknown".
- 2xx clears the dirty flag on the device.

## `WS /chat`

URL: `wss://<host>/chat?role=<role>&device=<device_id>&semsem=<uid_hex>`.

- Scheme = base URL with `https://`→`wss://` (or `http`→`ws`).
- Audio: 16 kHz mono PCM16, 20 ms / 320 samples / 640 bytes, binary frames upstream. Backend may send any chunk size downstream.
- Control: text JSON frames. Firmware scans for substrings `"listening"` / `"speaking"` (case-sensitive).
- Lifecycle: firmware streams immediately on open (no hello). Closes on tag removal, ws-disconnect, ws-error.

## Audio CDN

- Plain `GET` to manifest's `tracks[].url`.
- Firmware streams body in 4 KiB chunks to SD.
- No `Content-Range`/resume.
- Recommend pre-signed URLs that ignore the Bearer header.
- Files ≤ 20 MiB; 44.1/48 kHz stereo MP3 @ 128–192 kbps preferred.

## Dependencies

→ [data-model.md](data-model.md)
→ [chat-pipeline.md](chat-pipeline.md)
→ [security.md](security.md)
→ [../system/workflows.md](../system/workflows.md)

---
confidence: 0.92
sources: [S2, S3, S5]
last_confirmed: 2026-04-24
status: partial
