# Mobile Companion App Overview

> **Type:** Service
> **Status:** spec-only

## What

Single-purpose onboarding app. Joins a fresh Nadeem device to home Wi-Fi and hands it a `provision_token` that binds it to the parent's account. Once provisioning succeeds the device reboots into normal mode and the app has no further direct communication with it — all subsequent management goes through the backend (catalog, dashboards, etc).

## Two surfaces

1. **Backend** — same `/api/*` user-session endpoints as web; primarily `POST /api/provisioning-tokens`.
2. **Device local HTTP** — open, no auth, only valid while in onboarding mode (SoftAP). See [local-api.md](local-api.md).

## Platform notes

- **iOS ≥ 14:** `NEHotspotConfiguration` to join `Nadeem-XXXX`.
- **Android 10+:** `WifiNetworkSpecifier.Builder().setSsidPattern(...)` matching `Nadeem-` prefix; or scan-results filter.
- Detect "am I on the device AP?" via `GET http://192.168.4.1/info` returning a JSON body with `"fw"`.
- No mDNS during onboarding.

## Out of scope (per S4)

- Catalog management
- Parental dashboard
- Content curation / Pro flag editing
- Remote reset

These are app↔backend (or web), not device.

## Dependencies

→ [local-api.md](local-api.md)
→ [flow.md](flow.md)
→ [../system/workflows.md](../system/workflows.md) §W1
→ [../firmware/onboarding.md](../firmware/onboarding.md)

---
confidence: 0.85
sources: [S4, S7]
last_confirmed: 2026-04-25
status: spec-only
