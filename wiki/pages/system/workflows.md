# Workflows

> **Type:** Procedural
> **Status:** firmware side implemented; backend / app sides spec-only

End-to-end sequences that cross component boundaries. Procedural memory tier per [AGENTS.md §6](../../AGENTS.md).

---

## W1. First-boot onboarding & bootstrap

```
App  → Backend:  POST /provisioning-tokens   (user JWT)
App  ← Backend:  200 {provision_token, expires_at}      (~10 min TTL)

User joins SoftAP "Nadeem-XXXX" (open).
App  → Device:   GET  /info                  → {device_id, fw, "onboarding"}
App  → Device:   GET  /scan                  → [{ssid, rssi, auth}, …]
App  → Device:   POST /provision             form: ssid, password, provision_token
App  ← Device:   200 {ok:true}; device persists creds, reboots ~500 ms later

Device boots STA, joins LAN.
Dev  → Backend:  POST /bootstrap             {provision_token}
Dev  ← Backend:  200 {device_token, device_id}
Backend invalidates provision_token, binds device_id to user.
Device persists device_token to NVS `nadeem/dev_token`. Erases `prov_token`.
```

Source of truth: `firmware/main/main.c::on_wifi_up`, `firmware/components/onboarding/`, S3, S4.

---

## W2a. Regular semsem playback — SD mode

Activated when `bsp_sdcard_available()` returns true at boot.

```
Child places tag.
hmi  → bus:      NEV_HMI_SEMSEM_PLACED {uid}
semsem checks /sdcard/semsem/<uid>/manifest.json
  cache hit  → play tracks via audio component (file:// path)
  cache miss → led: SEMSEM_LOADING
               backend_client → GET /semsem/<uid>/manifest    (Bearer)
               for each track: GET <url> → /sdcard/semsem/<uid>/<name>
               then play
audio component emits NEV_AUDIO_STATE / NEV_AUDIO_TRACK_DONE.
stats records play_count, total_play_ms, last_played_unix.
Child lifts tag → NEV_HMI_SEMSEM_REMOVED → NEV_AUDIO_STOP.
```

Resume: track-index + byte offset saved to NVS `nd_progress`; best-effort byte seek.

---

## W2b. Regular semsem playback — stream mode (no SD)

Activated when `bsp_sdcard_available()` returns false at boot.

```
Child places tag.
hmi  → bus:      NEV_HMI_SEMSEM_PLACED {uid}
semsem checks NVS nd_manifest/<uid>
  cache hit  → parse manifest (name + url per track)
  cache miss → led: SEMSEM_LOADING
               backend_client → GET /semsem/<uid>/manifest    (Bearer)
               persist JSON to NVS nd_manifest/<uid>
               parse manifest
semsem → audio_set_stream_source(backend_client_read_chunk, http_client)
semsem → NEV_AUDIO_PLAY {path="raw://stream/N.mp3"}
         (audio ASP calls read callback; backend_client streams HTTPS with
          optional Range: bytes=N- header for resumption)
audio component emits NEV_AUDIO_STATE / NEV_AUDIO_TRACK_DONE.
stats records play_count, total_play_ms, last_played_unix.
Child lifts tag → NEV_HMI_SEMSEM_REMOVED → stream_close(), NEV_AUDIO_STOP.
```

Resume: track-index saved in NVS `nd_progress`; byte offset is compressed-HTTP offset (tracked by `stream_read_cb`). CDN must support `Range` for byte resume; falls back to track start otherwise.

---

## W3. Semsem-Pro AI conversation

```
Child places PRO tag.
semsem fetches manifest → {pro:true, role:"doctor"}
semsem posts NEV_SYS_MODE_PRO_ENTER {uid, role}
semsem_pro opens wss://<host>/chat?role=doctor&device=<id>&semsem=<uid>
  Auth: Bearer <device_token> in Authorization header (or query, by config).
Loop:
  Mic → 16 kHz mono PCM16, 20 ms / 640-byte binary WS frames upstream.
  Backend VAD/ASR/LLM/TTS → PCM16 binary frames downstream → DAC.
  Backend MAY send text frames {"type":"listening"} / {"type":"speaking"}
    to drive LED_PRO_LISTENING / LED_PRO_SPEAKING.
Child lifts tag → firmware closes socket → NEV_SYS_MODE_PRO_EXIT.
```

Backend owns all moderation. Firmware plays whatever PCM it receives.

---

## W4. Stats upload

- Per-UID counters held in `nadeem_stat` NVS namespace, RAM-cached, dirty-flagged.
- Tick: every 5 minutes while online + on clean shutdown.
- Per dirty UID: `POST /stats {uid, play_count, total_play_ms, last_played_unix, pro_session_count, pro_total_ms}` (one UID per request).
- 2xx → clear dirty. Non-2xx → keep dirty, retry next tick.
- Counters are **cumulative snapshots**, not deltas; backend stores last value per `(device_id, uid)` and computes deltas.

---

## W5. Token revocation (incident response)

```
Parent flags lost device in web dashboard.
Frontend → Backend: DELETE /api/devices/<id>
Backend invalidates device_token.
Device's next call → 401/403.
Firmware degrades to offline + cached playback.
Recovery: factory reset (long-press PLAY/PAUSE ≥3 s OR POST /reset on SoftAP)
          + re-run W1.
```

---

## W6. Factory reset

Triggers:
- Long-press PLAY/PAUSE for ≥ 3 s (`NEV_HMI_BTN_LONG_RESET`).
- `POST /reset` on local HTTP (only valid in onboarding mode).

Effect: clears `nadeem` NVS namespace (Wi-Fi, backend URL, device_token, prov_token), reboots into onboarding.

---

## Dependencies

→ [pages/firmware/onboarding.md](../firmware/onboarding.md)
→ [pages/firmware/state-machines.md](../firmware/state-machines.md)
→ [pages/backend/contract.md](../backend/contract.md)
→ [pages/mobile/flow.md](../mobile/flow.md)

---
confidence: 0.9
sources: [S1, S2, S3, S4, S5]
last_confirmed: 2026-04-26
status: partial
