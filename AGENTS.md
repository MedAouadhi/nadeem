# AGENTS.md

## Project at a glance

Nadeem = NFC-enabled audio box for Arabic-speaking children. Four parts:

1. **Firmware** (ESP32-S3 + PN532) — implemented v0.1 in `../nadeem_fw/`
2. **Cloud backend** (Django/Postgres/Redis) — located in backend/
3. **Web frontend** (Next.js, Arabic RTL)  located in frontend/
4. **Mobile companion app** (native, single-purpose onboarding), nadeem_mobile/
 
Physical NFC tags = "semsem" (سمسم). Placement plays audio (regular) or opens AI chat (Pro).

---

## 0. Non-negotiables

These rules override everything else when in conflict:

1. **No flattery, no filler.** Skip "Great question", "You're absolutely right", "I'd be happy to". Start with the answer or the action.
2. **Disagree when you disagree.** If the user's premise is wrong, say so before doing the work.
3. **Never fabricate.** Not file paths, not commit hashes, not API names, not test results. If you don't know, read the file, run the command, or say "I don't know, let me check."
4. **Stop when confused.** Two plausible interpretations? Ask. Do not pick silently.
5. **Touch only what you must.** Every changed line traces to the request. No drive-by refactors.
6. **Consult the agents domain docs first.** Skipping domain docs lookup and re-deriving wastes context and time.

---
## 1. Project context

### Stack

| Component | Stack | Status |
|-----------|-------|--------|
| Firmware | C, ESP-IDF ≥ 5.0, FreeRTOS, Espressif SMF | implemented v0.1 |
| Backend | Python, Django, PostgreSQL, Redis | spec-only |
| Web frontend | TypeScript, Next.js (React), Tailwind CSS | spec-only |
| Mobile app | iOS / Android native (single-purpose onboarding) | spec-only |
| Hardware | Waveshare ESP32-S3-AUDIO-Board + Elechouse PN532 v3 | wired per `wiki/pages/firmware/hardware.md` |

### Conventions

- **Identifiers:** `device_id` 12 hex lowercase; `uid_hex` 2·N hex lowercase (N ≤ 10); `role` ≤ 31 bytes.
- **Audio chat:** PCM16, mono, 16 kHz, 20 ms / 640-byte binary WS frames upstream. Don't change without an ADR.
- **Stats:** cumulative snapshots, not deltas. One UID per `POST /stats`.
- **Frontend (when built):** `dir="rtl" lang="ar"`, all copy in Arabic, fonts = Baloo Bhaijaan 2 + Cairo/Tajawal.

### Forbidden

- Do not commit secrets (`device_token`, `provision_token`, real user JWTs, real chat transcripts).
- Do not introduce on-device ASR/wake-word — `esp_sr` was removed deliberately. All voice understanding is backend-side.
- Do not skip TLS in production code paths.

---

### Source authority (when claims conflict)
1. Firmware code (`firmware/components/`, `firmware/main/main.c`) — ground truth for v0.1
2. Firmware-derived specs (`firmware/docs/*.md`, `firmware/VERIFICATION.md`)
3. PRDs (`prd_back_front.md`, `backend/backend.md`, `frontend/design.md`)

Code beats PRD for current behavior. PRD beats code for "what should be built". Tag both.

---

## 3. Project Learnings

Accumulated corrections in the domain docs. Append concrete one-liners when the user corrects an approach. Tighten existing lines instead of duplicating.

---

## Agent skills

### Issue tracker

Issues live as local markdown files under `.scratch/<feature-slug>/`. See `docs/agents/issue-tracker.md`.

### Triage labels

Default five-role vocabulary (`needs-triage`, `needs-info`, `ready-for-agent`, `ready-for-human`, `wontfix`). See `docs/agents/triage-labels.md`.

### Domain docs

Single-context repo: one `CONTEXT.md` + `docs/adr/` at repo root. See `docs/agents/domain.md`.
