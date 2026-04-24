# Web Frontend Screens

> **Type:** UI screens
> **Status:** spec-only; static HTML reference in `frontend/design.md`

| Screen | Purpose | Backend endpoints |
|--------|---------|--------------------|
| **Login** (تسجيل الدخول) | email + password auth | `POST /api/auth/login` |
| **Main Dashboard** (الرئيسية) | stats cards (listening hours, unique semsems, AI minutes), online status of devices | `GET /api/users/me/stats`, `GET /api/devices/` |
| **Add Device** (entry point in Dashboard) | mint provision token for the mobile app | `POST /api/provisioning-tokens` |
| **Semsem Library** | grid of bound UIDs with badge "عادي" (Regular) vs "ذكي" (Pro) | `GET /api/semsems` |
| **Semsem Details** | manifest details, edit (future) | `GET /api/semsems/<uid_hex>` |
| **Pro Chat History** | chronological list of AI conversations | `GET /api/chat-sessions/` |
| **Chat Detail** | full transcript, child vs AI bubbles | `GET /api/chat-sessions/<id>` |
| **Device Management** | list, revoke (lost / stolen) | `GET /api/devices/`, `DELETE /api/devices/<id>` |

## Implementation notes (from PRD §5)

- **Library badging:** check `is_pro` boolean → render "عادي" or "ذكي" badge.
- **Chat detail:** distinguish "Child" (transcribed audio) vs "AI" (Semsem responses) using chat-bubble layout.
- **Device security:** UI must expose `DELETE /api/devices/<id>` for lost/stolen recovery.
- **All copy in Arabic.** RTL layout. Confirm dialog text, empty states, error messages.

## Open questions

- Catalog editor (assigning audio playlists to UIDs, marking UIDs as Pro) is referenced in `firmware/docs/mobile.md` §6 but not specified in the PRD. Tracked in [lint.md](../../lint.md).

---
confidence: 0.65
sources: [S7, S9]
last_confirmed: 2026-04-25
status: spec-only
