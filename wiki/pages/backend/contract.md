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
| POST | `/bootstrap` | none | `{provision_token, device_id}` | `{device_token, device_id}` |
| GET | `/semsem/{uid_hex}/manifest` | Bearer | — | manifest JSON (see below) |
| GET | `{absolute track URL}` | Bearer (or none on CDN) | — | audio bytes |
| POST | `/stats` | Bearer | per-UID counter snapshot | 2xx on success |
| WS | `/chat?role=&device=&semsem=` | Bearer | binary PCM16 / text JSON | bidirectional binary + optional control text |
| GET | `/firmware/check?current_version=` | Bearer | — | `{has_update, version?, download_url?, changelog?}` |

## `POST /bootstrap`

```http
POST /bootstrap
Content-Type: application/json

{ "provision_token": "...", "device_id": "aabbccddeeff" }
```

Response 200:
```json
{ "device_token": "n_dk_live_…", "device_id": "aabbccddeeff" }
```

Backend responsibilities:
- Validate `provision_token`. Single-use, ~10 min TTL.
- Require `device_id` (lowercase 12-hex STA MAC) in the request body.
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
- Backend also rolls those computed deltas into one `DailyUsageStats` row per `(device_id, uid, day)` so "today" metrics can be queried accurately enough without changing the firmware payload.
- `last_played_unix` may be 0 / pre-2000 → "unknown".
- If `last_played_unix` is invalid or overflows timestamp parsing, backend attributes the delta to the server receive day.
- 2xx clears the dirty flag on the device.

## `WS /chat`

URL: `wss://<host>/chat?role=<role>&device=<device_id>&semsem=<uid_hex>`.

- Scheme = base URL with `https://`→`wss://` (or `http`→`ws`).
- Audio: 16 kHz mono PCM16, 20 ms / 320 samples / 640 bytes, binary frames upstream. Backend may send any chunk size downstream.
- Control: text JSON frames. Firmware scans for substrings `"listening"` / `"speaking"` (case-sensitive).
- Lifecycle: firmware streams immediately on open (no hello). Closes on tag removal, ws-disconnect, ws-error.

## `GET /firmware/check`

Device-facing OTA endpoint. Returns update availability based on the device's assigned ReleaseGroup.

Query params:
- `current_version` — device's current firmware version (semantic, e.g. `"1.0.0"`)

Response 200:
```json
{ "has_update": false }
```

Or when an update is available:
```json
{
  "has_update": true,
  "version": "2.0.0",
  "download_url": "https://cdn.../firmware/v2.0.0.bin",
  "changelog": "Fixed audio dropout bug"
}
```

Rules:
- Device must belong to a `ReleaseGroup` with an `assigned_release`.
- Compares version tuples numerically (`2.0.0` > `1.9.9`).
- `download_url` is null if release has no file artifact.
- No group or no assigned release → `{has_update: false}`.

## Audio CDN

- `GET` to manifest's `tracks[].url`.
- **SD mode:** firmware streams body in 4 KiB chunks directly to `/sdcard/semsem/<uid>/<name>`.
- **Stream mode (no SD):** firmware opens the URL with `Range: bytes=N-` when resuming mid-track; `N` is the compressed-byte offset. CDN **must** support `Range` requests and return `206 Partial Content` with `Accept-Ranges: bytes`. If it returns `200`, firmware plays from the start of the track.
- Recommend pre-signed URLs; firmware sends Bearer header but CDN may ignore it.
- Files ≤ 20 MiB; 44.1/48 kHz stereo MP3 @ 128–192 kbps preferred.

## Dependencies

→ [data-model.md](data-model.md)
→ [chat-pipeline.md](chat-pipeline.md)
→ [security.md](security.md)
→ [../system/workflows.md](../system/workflows.md)

---
confidence: 0.94
sources: [S2, S3, S5, backend/firmware/views.py, backend/stats/views.py, backend/stats/models.py]
last_confirmed: 2026-05-01
status: partial
