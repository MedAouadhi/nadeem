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
| transcript | array of `[speaker, text, timestamp]` |

## ProvisionToken (transient)

| Field | Notes |
|-------|-------|
| token | opaque, single-use |
| user_id | FK |
| issued_at, expires_at | ~10 min TTL |
| consumed_at | when redeemed by `/bootstrap` |

## Relationships

```
User 1───* Device 1───* UsageStats
                  *───* Semsem (via ownership / catalog table)
                  1───* ProChatSession
Semsem 1───* UsageStats (via uid_hex)
       1───* ProChatSession
User 1───* ProvisionToken (transient)
```

## Privacy

- ProChatSession transcripts contain child speech. Retention policy needed (PRD silent — open question; flagged in [lint.md](../../lint.md)).
- `device_token`, `provision_token` never logged in plaintext.

---
confidence: 0.65
sources: [S7, S3, S8]
last_confirmed: 2026-04-24
status: spec-only
