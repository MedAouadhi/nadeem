# Backend Data Model

> **Type:** DataModel set
> **Status:** spec-only

Target schema. Map to Django ORM (per PRD).

## User (Parent)

| Field | Type | Notes |
|-------|------|-------|
| email | string, unique | login |
| password_hash | string | |
| account_settings | JSON | preferences |

## Device

| Field | Type | Notes |
|-------|------|-------|
| device_id | char(12) PK | lowercase hex of STA MAC |
| device_token | opaque, indexed, unique | revocable, long-lived |
| user_id | FK User | bound at `POST /bootstrap` |
| online_status | bool / last_seen | derived from request recency |

## Semsem (NFC tag)

| Field | Type | Notes |
|-------|------|-------|
| uid_hex | varchar PK (composite with device or global by user — design choice) | NFC UID hex |
| title | string | shown in dashboard |
| is_pro | bool | drives `pro` field in manifest |
| role | char(31) | required when `is_pro=true`; ≤ 31 bytes |
| groups | M2M SemsemGroup | catalog grouping |

## SemsemGroup

| Field | Type | Notes |
|-------|------|-------|
| name | char(64), unique | e.g. "Animals", "Professions" |
| description | text | optional |
| icon | char(8) | emoji or short symbol |

## Manifest (derived view)

Returned by `GET /semsem/{uid_hex}/manifest`. Two shapes:

- **Regular:** `pro=false`, `tracks[]` of `{name, url}`.
- **Pro:** `pro=true`, `role` (≤ 31 bytes).

## UsageStats

Aggregated per `(device_id, uid_hex)`. Counter snapshot uploaded by firmware.

| Field | Notes |
|-------|-------|
| play_count | cumulative (compute deltas server-side) |
| total_play_ms | cumulative |
| last_played_unix | UTC seconds; 0 / pre-2000 = "unknown" |
| pro_session_count | cumulative |
| pro_total_ms | cumulative |

## ProChatSession

| Field | Notes |
|-------|-------|
| session_id | PK |
| device_id | FK |
| uid_hex | FK |
| start_time, end_time | session boundaries |
| flagged | bool, default=false | moderation flag |
| flag_reason | char(256), default="" | human-readable reason |
| transcript | array of `[speaker, text, timestamp]` |

## ProvisionToken (transient)

| Field | Notes |
|-------|-------|
| token | opaque, single-use |
| user_id | FK |
| issued_at, expires_at | ~10 min TTL |
| consumed_at | when redeemed by `/bootstrap` |

## FirmwareRelease

| Field | Type | Notes |
|-------|------|-------|
| version | char(32), unique | semantic version string |
| file | FileField | binary artifact stored on S3/MinIO |
| changelog | text | release notes |
| is_stable | bool, default=false | whether this is a production release |

## ReleaseGroup

| Field | Type | Notes |
|-------|------|-------|
| name | char(64), unique | e.g. "Stable", "Beta Testers" |
| description | text | optional |
| assigned_release | FK FirmwareRelease, nullable | release routed to this group |

Device optionally belongs to one ReleaseGroup via `release_group` FK. Devices without a group receive no OTA updates.

## Relationships

```
User 1───* Device 1───* UsageStats
                  *───* Semsem (via ownership / catalog table)
                  1───* ProChatSession
                  *───0..1 ReleaseGroup
Semsem 1───* UsageStats (via uid_hex)
       1───* ProChatSession
       *───* SemsemGroup
User 1───* ProvisionToken (transient)
FirmwareRelease 1───* ReleaseGroup
```

## Privacy

- ProChatSession transcripts contain child speech. Retention policy needed (PRD silent — open question; flagged in [lint.md](../../lint.md)).
- `device_token`, `provision_token` never logged in plaintext.

---
confidence: 0.85
sources: [S7, S3, S8, backend/semsems/models.py, backend/chat/models.py, backend/firmware/models.py, backend/devices/models.py]
last_confirmed: 2026-04-27
status: partial
