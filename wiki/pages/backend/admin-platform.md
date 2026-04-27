# Admin Platform

> **Type:** Service
> **Status:** implemented

Nadeem's business operations platform built on Django Admin with `django-unfold` (Tailwind-based skin). Replaces the default Django admin index with a platform analytics dashboard and adds domain-specific tooling for content, fleet, chat moderation, and firmware release management.

## Architecture

- **Admin site:** `NadeemAdminSite` (subclasses `UnfoldAdminSite`) in `backend/nadeem/admin_site.py`
- **Dashboard callback:** `nadeem.dashboard.dashboard_callback` — injects KPIs, today's stats, and top semsems into the index template
- **Custom views:** Stats Explorer at `/admin/stats-explorer/`
- **Skin:** `django-unfold` v0.90 + `django-admin-sortable2` v2.3 for drag-to-reorder tracks

## Registered Models

| Model | Admin Class | Key Features |
|-------|-------------|--------------|
| `accounts.User` | `UserAdmin` | device count, total listen time computed columns |
| `devices.Device` | `DeviceAdmin` | online/offline badge, `UsageStats` inline, release group filter |
| `devices.ProvisioningToken` | `ProvisioningTokenAdmin` | status badge (Active/Expired/Used), bulk delete-expired action |
| `semsems.Semsem` | `SemsemAdmin` | Pro badge, group list, track count, drag-to-reorder `TrackInline` with audio preview |
| `semsems.SemsemGroup` | `SemsemGroupAdmin` | icon column, semsem count |
| `semsems.Track` | `TrackAdmin` | audio preview player inline |
| `chat.ProChatSession` | `ProChatSessionAdmin` | flagged badge, duration display, transcript inline, flag/unflag bulk actions |
| `stats.UsageStats` | `UsageStatsAdmin` | listen hours computed column |
| `firmware.FirmwareRelease` | `FirmwareReleaseAdmin` | stable/dev badge, release group inline |
| `firmware.ReleaseGroup` | `ReleaseGroupAdmin` | device count, assigned release filter |

## Sidebar Navigation (UNFOLD config)

Defined in `backend/nadeem/settings.py` under `UNFOLD["SIDEBAR"]["navigation"]`:

- **Content:** Semsems, Semsem Groups, Tracks
- **Users & Devices:** Users, Devices, Provisioning Tokens
- **AI & Chat:** Chat Sessions
- **Analytics:** Usage Stats, Stats Explorer
- **Firmware:** Releases, Release Groups

## Dashboard (/admin/)

KPI cards (4-up):
- Total Users
- Total Devices
- Online Now (with percentage)
- Total Semsems

Today panel:
- Listening Time (hours)
- Pro Chat Sessions

Top Semsems table (top 10 by plays) with link to Stats Explorer.

## Stats Explorer (/admin/stats-explorer/)

Two tables:
1. **Semsem Popularity** — plays, listen hours, pro sessions, devices per semsem
2. **User Engagement** — devices, total listen hours, pro sessions per user

## Known Issues / Adaptations

- Plan specified `{% extends "unfold/index.html" %}` but unfold v0.90 does not ship `unfold/index.html`. Adapted to extend `admin/base.html` and manually include `unfold/helpers/app_list_default.html` + `unfold/helpers/history.html`.
- `@admin_site.register(Model)` decorator syntax fails during Django autodiscover when `admin_site` is imported from `nadeem.admin_site` (interaction with unfold's import-time settings access). All registrations use explicit `admin_site.register(Model, ModelAdmin)` instead.
- `django-admin-sortable2` requires the parent admin to inherit from `SortableAdminBase` when using `SortableInlineAdminMixin`. Applied to `SemsemAdmin`.
- `format_html()` without args is deprecated in Django 5.2. Replaced static HTML badges with `mark_safe()`.

---
confidence: 0.95
sources: [backend/nadeem/admin_site.py, backend/nadeem/dashboard.py, backend/nadeem/templates/admin/index.html, backend/nadeem/templates/admin/stats_explorer.html, backend/accounts/admin.py, backend/devices/admin.py, backend/semsems/admin.py, backend/chat/admin.py, backend/stats/admin.py, backend/firmware/admin.py]
last_confirmed: 2026-04-27
status: implemented
