# Chat Pipeline (Semsem-Pro)

> **Type:** Workflow / Service
> **Status:** spec-only

The backend half of `WS /chat`. Firmware is a dumb audio pipe; everything else is the backend's job.

## Inputs

- WebSocket binary frames upstream: 16 kHz mono PCM16, ~20 ms each.
- Query params: `role` (e.g. `"doctor"`), `device` (id), `semsem` (uid_hex).
- Bearer `device_token` on upgrade.

## Pipeline

```
PCM16 frames ─► VAD ─► utterance buffer ─► ASR ─► text
                                                   │
                                                   ▼
                                                  LLM (system prompt = role)
                                                   │
                                                   ▼
                                                  TTS ─► PCM16 chunks ─► WS down
```

- **VAD:** split utterances, decide when ASR fires.
- **ASR:** Arabic. Backend's choice (Whisper-style, Azure Speech, etc).
- **LLM:** prompted by role. Backend owns moderation (age-appropriate, profanity filter, medical-advice disclaimers for `doctor`, etc).
- **TTS:** Arabic, child-friendly voice. Stream as binary frames.

## Control frames (text)

Backend MAY send JSON text frames to drive LEDs:

```json
{ "type": "listening" }
{ "type": "speaking"  }
```

Firmware scans for substrings `"listening"` / `"speaking"`; the rest is ignored. Use this for VAD-state feedback so the child sees when the box is listening vs replying.

## Session lifecycle

1. Firmware connects → backend logs session start, allocates `session_id`.
2. Backend reads frames continuously; on detected utterance end → ASR → LLM → TTS.
3. Backend persists transcript (`[speaker, text, timestamp]`) per turn.
4. Firmware closes on tag removal → backend finalizes `ProChatSession` (start, end, full transcript).

## Safety

Backend is solely responsible for:
- Age-appropriate replies (target age unspecified in PRD — open question).
- Profanity filter on both ASR text and LLM output.
- Role-specific disclaimers (e.g. `doctor` should not give medical advice).
- Rate limits on session length.

Firmware will play whatever PCM you send. There is no on-device moderation.

## Dependencies

→ [contract.md](contract.md) — WS surface
→ [data-model.md](data-model.md) — `ProChatSession`
→ [security.md](security.md)
→ [../firmware/state-machines.md](../firmware/state-machines.md) — `semsem_pro` module

---
confidence: 0.6
sources: [S3, S7]
last_confirmed: 2026-04-24
status: spec-only
