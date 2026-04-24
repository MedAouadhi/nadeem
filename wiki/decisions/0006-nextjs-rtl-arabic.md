# 0006 — Next.js + Tailwind, Arabic-only RTL frontend

**Status:** proposed (PRD-stated, not yet implemented)
**Date:** 2026-04-25 (recorded)

## Context

Frontend is a parental dashboard for Arab-region users. Single-locale (Arabic). Design mockups already use Tailwind utility classes and Material-3-style design tokens.

## Decision

- **Framework:** Next.js (React).
- **Styling:** Tailwind CSS.
- **Localization:** Arabic only; no English fallback.
- **Direction:** strict RTL (`<html dir="rtl" lang="ar">`).
- **Typography:** Baloo Bhaijaan 2 (headings/logos), Cairo or Tajawal (body).
- **Design tokens:** Material 3 color set (see [pages/frontend/design-system.md](../pages/frontend/design-system.md)).

## Consequences

- All copy must be Arabic from day one. No `t()` strings stubbed in English.
- RTL mirroring affects layout: padding/margin need logical (`ps-*` / `pe-*`) not directional.
- Tailwind config must include the custom color tokens already chosen in `frontend/design.md`.
- Server vs client components: not specified; default to App Router unless a reason emerges.

## Open

- i18n is not strictly necessary now, but if expansion to other locales is on the roadmap, design tokens should remain locale-agnostic.

## Sources

- S7 §1.1
- S9
