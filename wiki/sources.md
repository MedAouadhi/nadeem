# Source Registry

Pointers to authoritative source material. Authority order in [AGENTS.md §4](AGENTS.md).

| ID | Path | Kind | Authority | Notes |
|----|------|------|-----------|-------|
| S1 | [`firmware/main/main.c`](../firmware/main/main.c) | code | 1 | Boot path, mode selection |
| S2 | [`firmware/components/`](../firmware/components/) | code | 1 | All firmware components and headers |
| S3 | [`firmware/docs/backend.md`](../firmware/docs/backend.md) | spec-from-code | 2 | Firmware↔backend contract derived from S2 |
| S4 | [`firmware/docs/mobile.md`](../firmware/docs/mobile.md) | spec-from-code | 2 | Device-local onboarding HTTP surface |
| S5 | [`firmware/VERIFICATION.md`](../firmware/VERIFICATION.md) | spec-from-code | 2 | Build, test matrix, wiring, known limitations |
| S6 | [`firmware/requirements.md`](../firmware/requirements.md) | original-brief | 3 | Initial product brief from author |
| S7 | [`prd_back_front.md`](../prd_back_front.md) | PRD | 3 | Unified backend + frontend PRD |
| S8 | [`backend/backend.md`](../backend/backend.md) | PRD-detail | 3 | Re-states S3 for backend implementers |
| S9 | [`frontend/design.md`](../frontend/design.md) | UI mockup | 3 | Static HTML+Tailwind design references |
| S10 | [`AGENTS.md`](../AGENTS.md) | process | n/a | Engineering rules; not domain |
| S11 | [`llm-wiki.md`](../llm-wiki.md) | methodology | n/a | Wiki construction guide |
| S12 | [`docs/`](../docs/) | scaffolding | 3 | Templates and modes; mostly empty knowledge-base/diagrams |
| S13 | [`docs/superpowers/plans/2026-04-27-nadeem-admin-platform.md`](../docs/superpowers/plans/2026-04-27-nadeem-admin-platform.md) | implementation-plan | 3 | Step-by-step plan for admin platform v1 |

## Implementation status by source

- **S1, S2, S3, S4, S5** — implemented in firmware v0.1 (per S5).
- **S7, S8, S9** — partially implemented (admin platform v1 done 2026-04-27; remaining APIs spec-only).
- **S6** — original brief; superseded in detail by later docs but still useful for intent.
- **S13** — fully executed on 2026-04-27.

## Ingest log

| Date | Source | Action |
|------|--------|--------|
| 2026-04-24 | All above | Initial wiki bootstrap |
| 2026-04-27 | S13 + backend code | Admin platform implementation; updated data-model, contract, component-status, relationships; added admin-platform.md |
