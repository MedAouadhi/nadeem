# Wiki Lint Report

> **Type:** Health
> **Status:** current

Self-reported issues, contradictions, and open questions. Run lint per [AGENTS.md §7.3](AGENTS.md). Resolve into ADRs or page edits.

## Open contradictions

| # | Description | Sources | Proposed resolution |
|---|-------------|---------|----------------------|
| C1 | PRD §1.1 lists "Frontend (Web & Mobile-Web): Next.js" — implies the mobile experience is also Next.js. But `firmware/docs/mobile.md` describes a native onboarding app (NEHotspotConfiguration on iOS, WifiNetworkSpecifier on Android). | S4, S7 | Backend/PRD wins for web; firmware spec wins for mobile (native is required for SoftAP join). Update PRD wording. |
| C2 | `prd_back_front.md` is silent on how the device learns its `backend_url`. v0.1 firmware expects it pre-baked or set via console; `/provision` form does not include it. | S2, S4, S7 | Add `backend_url` field to `POST /provision` form; track as ADR + firmware change. |

## Stale / unverified claims

- _none_ (wiki is fresh as of 2026-04-25)

## Orphan check

All pages listed in [index.md](index.md) — no orphans.

## Open questions (no ADR yet)

| # | Question | Affected pages |
|---|----------|----------------|
| Q1 | What is the chat-transcript retention policy? Parental delete / export? | backend/data-model, backend/security |
| Q2 | What's the catalog mutation API? (assign tracks to a UID, mark a UID as Pro.) PRD silent. | backend/contract, frontend/screens |
| Q3 | Manifest cache invalidation strategy. Currently requires factory reset to refresh local cache. | firmware/limitations, backend/contract |
| Q4 | Target child-age range — drives ASR/LLM/TTS choice and content moderation defaults. | backend/chat-pipeline |
| Q5 | OTA / firmware update path. Not covered in v0.1. | firmware/overview |
| Q6 | Web frontend stats schema — rollups, per-UID, time-window? | backend/contract, frontend/api-mapping |
| Q7 | TLS cert bundle integration in firmware (`backend_client.c`). Required before prod. | firmware/limitations |
| Q8 | WebSocket header-based Bearer auth wiring (currently un-wired in v0.1). | firmware/limitations, backend/security |

## Confidence outliers

Pages below 0.7 — needing more sources / re-confirmation:

- `pages/backend/chat-pipeline.md` (0.6) — pure inference from PRD; no backend code.
- `pages/backend/data-model.md` (0.65) — PRD-only.
- `pages/frontend/overview.md` (0.6) — spec-only.
- `pages/frontend/screens.md` (0.65) — spec-only.

These are expected (spec-only components). Re-score after backend / frontend implementations land.

## Last lint pass

- 2026-04-25 — initial bootstrap. All flags above are fresh (not stale).
