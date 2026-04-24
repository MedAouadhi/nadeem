# Web Frontend Overview

> **Type:** Service
> **Status:** spec-only (static HTML mockups in `frontend/design.md`)

## What

Parental dashboard. Single-page-ish Next.js app for parents to manage devices, browse the semsem library, audit AI chat sessions, and revoke device tokens.

## Stack (per PRD)

- **Framework:** Next.js (React).
- **Styling:** Tailwind CSS.
- **Localization:** Arabic only, RTL strict (`dir="rtl"`).
- **Typography:** Baloo Bhaijaan 2 (headings/logos), Cairo or Tajawal (body).
- **Auth:** session/JWT against backend `/api/auth/login`.

## Pages

See [screens.md](screens.md). Core set:
- Login
- Main dashboard (stats cards, devices)
- Semsem library (grid of bound UIDs)
- Semsem details
- Chat history list
- Chat history detail (transcript)
- Device management (with revoke)

## Data plane

All reads/writes against backend `/api/*`. See [api-mapping.md](api-mapping.md).

## Design system

See [design-system.md](design-system.md). Material-Design-3 inspired tokens already chosen in mockup (primary `#00668a`, surface `#fafaf5`, etc.).

## Out of scope

- No direct device communication. Web only talks to backend.
- No content authoring tools in v0.1 (catalog management is hand-wavy in PRD).

## Dependencies

→ [api-mapping.md](api-mapping.md)
→ [design-system.md](design-system.md)
→ [screens.md](screens.md)
→ [../backend/overview.md](../backend/overview.md)

---
confidence: 0.6
sources: [S7, S9]
last_confirmed: 2026-04-25
status: spec-only
