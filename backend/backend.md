# Nadeem Backend — Integration Spec

This document is the full contract between the Nadeem firmware and the cloud
backend. It is derived directly from the firmware source (`components/backend_client/`,
`components/semsem/`, `components/semsem_pro/`, `components/stats/`, `main/main.c`),
so a backend that satisfies this spec will work with v0.1 devices out of the box.

Nothing in the firmware depends on any language or framework — any HTTP+WebSocket
server will do.

---

## 1. Transport overview

- REST over HTTPS (HTTP is accepted too, but production should terminate TLS).
- Base URL is configured per-device at onboarding time and stored in the
  device's NVS. The device sends it to itself via the companion app's
  `POST /provision` call (indirectly — see §4.1 bootstrap).
- Authentication: a **Bearer** token, stored per-device, sent on every request
  as `Authorization: Bearer <device_token>`.
- WebSocket endpoint for the AI chat mode (Semsem-Pro).
- The firmware is tolerant of transient failures and cached-offline operation;
  the backend can be down and the device still plays cached semsems.

### 1.1 Identifiers

| Name               | Format                            | Source                                          |
|--------------------|-----------------------------------|-------------------------------------------------|
| `device_id`        | 12 hex chars (STA MAC, lowercase) | Firmware sends it in onboarding `GET /info`; also embedded in WebSocket queries via the device token payload. |
| `uid_hex`          | 2·N hex chars (N ≤ 10, lowercase) | The UID of an NFC tag ("semsem"), read over ISO14443A by the PN532. |
| `provision_token`  | opaque string                     | Issued by the app's user-authenticated endpoint, redeemed once by the device at `POST /bootstrap`. |
| `device_token`     | opaque string                     | Issued in response to `POST /bootstrap`; long-lived. |

### 1.2 Content-types

- All JSON bodies: `application/json; charset=utf-8`.
- Audio file downloads: whatever media type the `esp_audio_simple_player`
  library can decode — MP3, WAV, AAC, FLAC are all supported. MP3 is the
  expected default for v0.1.

### 1.3 Error conventions

- Firmware inspects the HTTP status code and optionally a JSON body — any 2xx is
  success; anything else is a failure and the firmware may retry, fall back to
  cache, or surface an error event to the LED.
- If you want the firmware to show an error state, just return a non-2xx.
  The firmware does not parse structured error bodies.

---

## 2. Endpoint map

| Method | Path                                    | Purpose                                 | Auth          |
|--------|-----------------------------------------|-----------------------------------------|---------------|
| POST   | `/bootstrap`                            | Redeem `provision_token` for `device_token` | none (token is in body) |
| GET    | `/semsem/{uid_hex}/manifest`            | Metadata + track list for a semsem      | Bearer device |
| GET    | `{absolute track URL}`                  | Download one audio file                 | Bearer device (usually) |
| POST   | `/stats`                                | Upload per-UID usage counters           | Bearer device |
| WS     | `wss://<host>/chat?role=<role>&device=<id>&semsem=<uid>` | Live PCM audio streaming for Pro mode | Bearer device in `Sec-WebSocket-Protocol` or query | 

The firmware builds URLs by simple concatenation. Examples below use
`https://api.nadeem.example` as the base.

---

## 3. Bootstrap: one-time device ↔ account binding

The companion app (see `mobile.md`) obtains a short-lived `provision_token` from
its own user-authenticated endpoint (`POST /provisioning-tokens`), then pushes
it to the device along with Wi-Fi credentials. On its next boot the device
calls `POST /bootstrap` exactly once to exchange it for a persistent
`device_token`.

### 3.1 `POST /bootstrap`

```http
POST /bootstrap HTTP/1.1
Host: api.nadeem.example
Content-Type: application/json

{
  "provision_token": "eyJhbGciOi..."
}
```

Response `200 OK`:

```json
{
  "device_token": "n_dk_live_8f7c…",
  "device_id":    "aabbccddeeff"
}
```

- The backend SHOULD record `device_id` at this point and bind it to the user
  that created the `provision_token`.
- `device_token` is persisted in NVS (`nadeem/dev_token`) and used as Bearer on
  every subsequent call.
- `provision_token` MUST be invalidated after a successful exchange. The
  firmware will not retry unless the user runs a factory reset.
- If `provision_token` is invalid/expired, return a 4xx; the firmware logs a
  warning but otherwise proceeds offline until the user re-provisions.

### 3.2 `POST /provisioning-tokens` (companion-app side)

Not called by the firmware, documented here for completeness.

```http
POST /provisioning-tokens HTTP/1.1
Host: api.nadeem.example
Authorization: Bearer <user_session_jwt>
```

Response `200 OK`:

```json
{
  "provision_token": "eyJhbGciOi...",
  "expires_at": "2026-04-19T21:00:00Z"
}
```

TTL ~10 min, single-use.

---

## 4. Semsem catalog

Each NFC tag placed on the box is a **semsem**. The firmware identifies it by
the raw NFC UID, hex-encoded (no separators, lowercase). When a new UID is
detected, the firmware:

1. Looks for `/sdcard/semsem/<uid_hex>/manifest.json` locally.
2. If missing, calls `GET /semsem/<uid_hex>/manifest` and downloads listed
   files into that directory.
3. Caches everything for offline use.

### 4.1 `GET /semsem/{uid_hex}/manifest`

```http
GET /semsem/04a3f91b2c/manifest HTTP/1.1
Host: api.nadeem.example
Authorization: Bearer n_dk_live_8f7c…
```

Response `200 OK`, `Content-Type: application/json`:

```json
{
  "uid": "04a3f91b2c",
  "title": "Animal Sounds — volume 1",
  "pro": false,
  "role": "",
  "tracks": [
    { "name": "01.mp3", "url": "https://cdn.nadeem.example/semsem/04a3f91b2c/01.mp3" },
    { "name": "02.mp3", "url": "https://cdn.nadeem.example/semsem/04a3f91b2c/02.mp3" }
  ]
}
```

Fields parsed by the firmware:

| Field        | Required | Notes                                                                 |
|--------------|----------|-----------------------------------------------------------------------|
| `pro`        | yes      | If `true`, the firmware does NOT play tracks — it enters AI chat mode using `role`. |
| `role`       | when `pro=true` | Short string (≤ 31 bytes). Examples: `"doctor"`, `"engineer"`, `"storyteller"`. The firmware forwards this as the `?role=` query on the WebSocket. |
| `tracks[].name` | yes (if not pro) | Used as the local filename under `/sdcard/semsem/<uid_hex>/`. Must be a safe FAT filename (no `/`, < 32 chars is safe). |
| `tracks[].url`  | yes (if not pro) | Absolute URL. Firmware sends the same Bearer device token; if your CDN is public, ignore the header. |

Extra fields are ignored and may be used by the companion app.

Status codes:

- `404 Not Found`: tag is unknown — firmware surfaces an error event and LEDs go red.
- `403 Forbidden`: the authenticated device is not allowed to access this tag
  (e.g. parental restriction). Treat identically to 404.
- Any 5xx or network failure: firmware falls back to SD cache. If the cache is
  empty it errors out.

### 4.2 Audio file downloads

Tracks are downloaded with plain `GET` to the `url` field. The firmware has
two modes:

**SD mode** (SD card present at boot):

- sends the device's Bearer token;
- streams the body in 4 KiB chunks straight to the SD card;
- expects `200 OK`;
- partial downloads are discarded on the next boot if corrupted (the firmware
  will re-fetch if decode fails).

**Stream mode** (no SD card):

- sends the device's Bearer token;
- sends `Range: bytes=N-` when resuming mid-track (`N` = compressed-byte
  offset tracked by the firmware);
- expects `206 Partial Content` with `Content-Range` and `Accept-Ranges:
  bytes` headers;
- if the server returns `200` instead of `206`, the firmware falls back to
  playing the track from the start.

Requirements for the backend / CDN:

- **Must support `Range` requests** and return `206 Partial Content` with a
  valid `Content-Range` header. This is required for stream-mode devices to
  resume playback after NFC removal.
- **Must return `Accept-Ranges: bytes`** on all audio responses so the
  firmware can detect Range support.
- Serve from a CDN; the `url` can be a pre-signed URL that skips the auth
  header. The firmware sets the header but tolerates 2xx regardless.
- Keep files < 20 MiB each for flash/SD comfort.
- Prefer 44.1 kHz or 48 kHz stereo MP3 at 128–192 kbps.

### 4.3 Manifest authoring tips

- A "regular" semsem: `pro=false`, list the files in playlist order. Firmware
  persists per-track progress so the child can resume where they left off.
- A "pro" semsem: `pro=true`, provide a meaningful `role`. No tracks are
  necessary — the device will never read them.
- Changing a manifest doesn't invalidate local caches. For v0.1, the
  companion app should instruct the user to long-press PLAY/PAUSE (factory
  reset) or implement a manifest-version header and a background refresh in a
  later revision.

---

## 5. Semsem-Pro: live AI conversation

When the manifest says `"pro": true`, the firmware opens a **WebSocket** to the
backend and bidirectionally streams 16 kHz mono PCM16 audio. All speech
recognition, LLM reasoning, and TTS are your problem — the device is a dumb
audio pipe with a microphone, a speaker, and a role label.

### 5.1 URL

```
wss://<host>/chat?role=<role>&device=<device_id>&semsem=<uid_hex>
```

- The scheme is derived by substituting `https://`→`wss://` (or `http://`→`ws://`) from the configured base URL.
- `role` is URL-encoded as-is from the manifest's `role` field.
- `device` and `semsem` identify which unit/tag is streaming. (These query
  params are already in the firmware; they were not in the plan doc but the
  WebSocket URL embeds them for your telemetry.)

Auth: pass the device's Bearer token in the `Authorization` header on the
initial HTTP upgrade request. The ESP-IDF websocket client supports this via
`esp_websocket_client_config_t.headers`. (v0.1 doesn't wire it explicitly;
add it on your side by requiring the token in a query parameter too, e.g.
`&token=<device_token>`, if header-based auth is awkward.)

### 5.2 Audio format (both directions)

- **Codec:** signed 16-bit little-endian PCM, mono, **16 kHz**.
- **Frame size:** firmware sends exactly 20 ms frames = **320 samples = 640
  bytes** per binary WS message. The backend may send any chunk size; the
  firmware feeds whatever it receives directly into the DAC.
- **Transport:** WebSocket binary frames (`opcode = 0x2`).
- No header, no timestamps — raw samples only.

### 5.3 Control frames (text)

The backend MAY send JSON control frames (`opcode = 0x1`) to drive LED state.
The firmware scans each text frame for the substrings `"listening"` and
`"speaking"` (case-sensitive) and reacts:

| Contains      | LED mode switches to |
|---------------|----------------------|
| `"listening"` | `PRO_LISTENING` (purple breath) |
| `"speaking"`  | `PRO_SPEAKING`  (purple rotating) |

Minimal example frame:

```json
{"type":"listening"}
```

```json
{"type":"speaking"}
```

Anything else in the JSON is ignored. You can also stuff telemetry in the
frame — the firmware won't care.

### 5.4 Session lifecycle

1. Firmware opens the socket when `pro=true` on `GET /manifest`.
2. Firmware immediately starts streaming mic audio as binary frames. There is
   no "hello" message from the firmware; the very first thing you'll see is
   audio.
3. Backend performs ASR on the uploaded audio (typically using VAD to split
   utterances).
4. Backend streams TTS audio back as binary frames; optionally sends
   `"speaking"`/`"listening"` text frames for LED feedback.
5. Session ends when the child lifts the semsem (firmware closes the socket).
   The firmware also closes on `WEBSOCKET_EVENT_DISCONNECTED` /
   `WEBSOCKET_EVENT_ERROR` from the IDF client and reports an error LED.

### 5.5 Safety / content moderation

The backend is responsible for all safety: age-appropriate replies, profanity
filtering, medical-advice disclaimers for the `doctor` role, etc. The firmware
has no moderation layer and will play whatever PCM you send.

---

## 6. Usage statistics

The firmware tracks a small set of per-UID counters locally and uploads them
in batches every 5 minutes while online, and on clean shutdown.

### 6.1 `POST /stats`

The firmware currently uploads **one UID per request** (not a batch). You will
receive multiple `POST /stats` calls in quick succession when several UIDs are
dirty.

```http
POST /stats HTTP/1.1
Host: api.nadeem.example
Authorization: Bearer n_dk_live_8f7c…
Content-Type: application/json

{
  "uid": "04a3f91b2c",
  "play_count": 7,
  "total_play_ms": 284300,
  "last_played_unix": 1734551234,
  "pro_session_count": 2,
  "pro_total_ms": 61000
}
```

Field semantics:

| Field               | Meaning |
|---------------------|---------|
| `uid`               | The semsem UID hex (matches the manifest path). |
| `play_count`        | Number of distinct placement-playback events since last upload plus prior value. (Monotonically increasing counter on the device.) |
| `total_play_ms`     | Accumulated ms actually played (best-effort — currently derived from bytes written). |
| `last_played_unix`  | UTC wall-clock seconds of the last placement. If the device has no NTP-synced time yet this may be 0 or pre-2000; treat as "unknown". |
| `pro_session_count` | Number of Semsem-Pro chat sessions opened. |
| `pro_total_ms`      | Total duration of Pro chat sessions. |

Successful upload: any 2xx. The firmware then clears the entry's dirty flag.
Non-2xx: the firmware keeps the entry dirty and retries on the next tick.

The backend should treat the counters as **cumulative snapshots**, not deltas.
Store the last value per `(device_id, uid)` and compute deltas server-side if
you want rate metrics.

### 6.2 Rate & size

- At most one `POST /stats` call per UID per 5-minute tick, plus one on shutdown.
- Body is small (~200 bytes).
- No streaming/chunked uploads.

### 6.3 Aggregating for the companion app

The backend is expected to expose its own aggregated read-side (not called by
the firmware), something like `GET /devices/<id>/stats` returning per-UID
breakdowns plus rollups. Design that to taste — it's purely app ↔ backend.

---

## 7. Onboarding round-trip summary

A concrete timeline for a fresh unit:

```
App → backend:   POST /provisioning-tokens        (user JWT)
App ← backend:   200 { provision_token, expires_at }

User joins "Nadeem-XXXX"
App → device:    GET /info                        -> {device_id, "onboarding"}
App → device:    GET /scan                        -> AP list
App → device:    POST /provision (ssid, pw, provision_token)
App ← device:    200 {ok:true}; device reboots

Device boots in STA mode, associates with home Wi-Fi.
Device → backend: POST /bootstrap {provision_token}
Device ← backend: 200 {device_token, device_id}     <-- binds device to user
Device persists device_token in NVS.

Child places a semsem.
Device → backend: GET /semsem/<uid>/manifest        (Bearer device_token)
Device ← backend: 200 {pro:false, tracks:[...]}
Device → CDN:     GET https://cdn.../01.mp3
Device ← CDN:     audio bytes
Device plays audio; persists progress locally.
... 5 min later ...
Device → backend: POST /stats {uid, play_count, ...}  (Bearer)
Device ← backend: 200
```

---

## 8. Minimum backend surface to be "Nadeem-compatible"

A minimum viable backend must implement:

1. `POST /provisioning-tokens` (app-side auth)
2. `POST /bootstrap`                  (device-side token exchange)
3. `GET  /semsem/{uid_hex}/manifest`  (Bearer)
4. Public or Bearer-guarded CDN for audio files
5. `POST /stats`                      (Bearer)
6. `wss://.../chat?role=...`          (Bearer)

Everything else (parental dashboards, catalog editors, OTA, fleet management)
is a matter of the app-backend contract and doesn't touch the firmware for
v0.1.

---

## 9. Security notes

- **TLS is mandatory in production.** The firmware uses `esp_http_client`
  which supports it; enable `CONFIG_MBEDTLS_CERTIFICATE_BUNDLE=y` and attach
  `esp_crt_bundle_attach` in `backend_client.c`.
- **Device tokens SHOULD be long-lived but revocable.** If a child loses the
  device or it's stolen, the backend needs a way to revoke its token; the
  firmware will gracefully degrade to offline + cached playback until a
  factory reset + re-bootstrap.
- **`provision_token` is single-use and short-lived.** Don't accept stale ones.
- **Rate-limit `/bootstrap`.** A lost device with a valid device token
  shouldn't be able to DoS it — but `/bootstrap` happens once per device ever.
- **CORS is not relevant** — all firmware calls are server-to-server style
  (no browser origin). But the app-side endpoints may need CORS.
- **The WebSocket carries a live mic feed from a child's bedroom.** Treat the
  audio stream with the same care as any sensitive content; TLS-only (wss://),
  require authentication on the upgrade, and log sparingly.

---

## 10. Test matrix (backend self-test)

Use the firmware's `console_cmds` to drive each flow. From the ESP32 serial
REPL (`idf.py monitor`):

- `nfc_poll` — confirms the reader works.
- `semsem_trigger <uid_hex>` — simulates a placement; exercises manifest fetch.
- `stats_dump` — shows current counters.
- `stats_upload` — forces an immediate `POST /stats`.
- `wifi_set <ssid> <pw>` and `wifi_forget` — re-onboarding shortcuts.

A backend passes the MVP bar when:

- A fresh device provisions in < 60 s and `POST /bootstrap` is called exactly
  once, producing a valid `device_token`.
- A known regular semsem plays from backend + CDN, is cached on SD, and
  continues playing with the Ethernet cable unplugged.
- A known PRO semsem opens a WebSocket, audio flows both ways, and the LEDs
  alternate between `PRO_LISTENING` and `PRO_SPEAKING` in response to text
  control frames.
- `POST /stats` arrives within 5 min of playback with sane values.
- Removing Wi-Fi doesn't crash the device — it falls back to cached semsems.
