# Stats and Presence Design

## Goal

Fix inaccurate device presence and listening stats in the backend/frontend without changing the current firmware payload shape.

## Scope

This design covers:
- backend presence semantics for devices
- backend ingestion and read models for semsem listening stats
- frontend presentation of listening time and device status
- admin/dashboard corrections where current calculations are misleading

This design does not cover:
- adding a firmware heartbeat
- changing firmware `POST /stats` payload fields
- changing chat transport or semsem manifest behavior

## Current Observations

### Firmware facts

The firmware in `../nadeem_fw` currently:
- uploads cumulative per-UID snapshots to `POST /stats`
- sends one UID per request
- includes `play_count`, `total_play_ms`, `last_played_unix`, `pro_session_count`, `pro_total_ms`
- does not send a dedicated online heartbeat
- only refreshes backend visibility when it makes authenticated requests such as manifest fetches, stats uploads, or chat/authenticated traffic

### Backend facts

The current backend:
- stores one `UsageStats` row per `(device, uid_hex)` as the latest snapshot
- overwrites the snapshot on each upload
- derives online/offline from `last_seen_at`
- currently uses a `120s` online threshold in multiple places
- currently calculates some "today" values from `UsageStats.updated_at`, which is upload time and not actual play time

### Frontend facts

The current frontend:
- renders listening duration in hours in dashboard, devices, and semsem details
- expects device status booleans from backend responses
- does not distinguish between today's listening and lifetime listening

## Product Decision

### Presence semantics

`online` means recent backend activity, not true live socket presence.

Definition:
- a device is online when `last_seen_at >= now - 5 minutes`

Implications:
- an idle device can appear offline even if powered on and connected to Wi-Fi
- this is acceptable because firmware has no heartbeat today
- UI copy should avoid implying exact real-time connection state beyond recent activity

## Requirements

### Presence

1. All backend surfaces must use the same `5 minute` presence TTL.
2. Presence logic must be centralized so the model property, web APIs, and admin/dashboard cannot drift.
3. Device APIs should keep returning both a boolean status and `last_seen_at`.

### Listening stats

1. Lifetime totals must remain derived from the latest cumulative firmware snapshot.
2. "Today" totals must be computed accurately from per-upload deltas, not from snapshot write time.
3. Counter regressions must not produce negative deltas or double counting.
4. Frontend should display listening durations in minutes, not hours.
5. Frontend should show both today's listening and lifetime listening where applicable.

## Approaches Considered

### Approach A: Minimal patch

Change the presence TTL to 5 minutes and change frontend labels from hours to minutes.

Pros:
- very small change
- low migration risk

Cons:
- cannot support accurate "today" listening
- leaves admin/dashboard misleading

### Approach B: Snapshot + daily delta rollups

Keep the current snapshot table for latest totals and add a daily rollup table computed from differences between consecutive snapshots.

Pros:
- works with current firmware contract
- supports accurate today/all-time numbers
- query model stays simple for frontend/admin

Cons:
- requires migration and ingestion logic changes

### Approach C: Append-only event ledger

Convert uploads into stored delta events and derive all totals from ledger queries or materialized rollups.

Pros:
- maximum auditability and flexibility

Cons:
- more schema and code than needed right now

## Selected Approach

Approach B.

Reason:
- it preserves compatibility with firmware as-is
- it fixes both reported product issues
- it keeps read-side queries straightforward for backend/admin/frontend

## Backend Design

### 1. Centralized presence rule

Introduce one backend constant/function for device presence:
- `DEVICE_ONLINE_TTL = timedelta(minutes=5)`
- helper/query utility that answers whether a device is online from `last_seen_at`

All of these must use the same shared rule:
- `Device.online`
- `/api/users/me/stats`
- `/api/devices`
- admin device status badges
- admin dashboard KPI counts

### 2. Keep snapshot table

`UsageStats` remains the source of truth for the latest cumulative counters for a `(device, uid_hex)` pair:
- `play_count`
- `total_play_ms`
- `last_played_unix`
- `pro_session_count`
- `pro_total_ms`

This table continues to support:
- lifetime totals
- latest last-played timestamp
- current semsem/device aggregates

### 3. Add daily rollup table

Add a new model for daily deltas, one row per `(device, uid_hex, day)`.

Suggested fields:
- `device` FK
- `uid_hex`
- `day` date
- `play_count_delta`
- `play_ms_delta`
- `pro_session_count_delta`
- `pro_total_ms_delta`
- `created_at`
- `updated_at`

Uniqueness:
- unique constraint on `(device, uid_hex, day)`

Purpose:
- provide accurate day-bounded stats while leaving firmware contract unchanged

### 4. Delta computation on upload

On each `POST /stats`:
1. authenticate device and refresh `last_seen_at`
2. load the existing `UsageStats` row for `(device, uid)` if present
3. compute deltas for each cumulative counter:
   - `delta = new_value - old_value`
4. clamp negative deltas to zero
5. write/add those deltas into the matching daily rollup row
6. overwrite `UsageStats` with the new snapshot

Regression handling:
- if any incoming cumulative counter is less than the stored snapshot, treat that field's delta as `0`
- continue updating the stored snapshot to the new baseline
- do not attempt to infer missing historical playback across resets

This handles:
- device reset
- re-provisioning with cleared local stats
- NVS loss or rollback

### 5. Day attribution rule

Use the firmware timestamp when possible.

Rule:
- if `last_played_unix` is valid, attribute the delta to that UTC day
- otherwise attribute the delta to the server receive day

Validity rule:
- `last_played_unix <= 0` is invalid
- obviously bogus historical values should also be treated as unknown if existing code already enforces that boundary

Tradeoff:
- if a single upload includes playback accumulated across multiple days while the device was offline, the delta lands on the most recent known play day, not perfectly spread across calendar days

Why this is acceptable now:
- firmware only exposes cumulative counters plus one last-play timestamp, so exact intra-window distribution is impossible without a protocol change
- this design is still materially more accurate than using `updated_at`

### 6. Read-side API changes

#### `GET /api/users/me/stats`

Return:
- `today_listening_ms`
- `total_listening_ms`
- `today_pro_ms`
- `total_pro_ms`
- `unique_semsems`
- `device_count`
- `online_device_count`

Notes:
- `today_*` comes from daily rollups for the current day
- `total_*` comes from `UsageStats`

#### `GET /api/devices`

For each device return:
- `device_id`
- `online_status`
- `bootstrapped`
- `last_seen_at`
- `created_at`
- `today_listening_ms`
- `total_listening_ms`
- `total_semsems`

Notes:
- current frontend already expects richer device cards than the backend currently serves
- this endpoint should be aligned to the real UI needs instead of relying on mock-only shapes

#### `GET /api/semsems/<uid>`

Return:
- `uid_hex`
- `title`
- `is_pro`
- `role`
- `tracks`
- `today_play_count`
- `total_play_count`
- `today_listening_ms`
- `total_listening_ms`
- `pro_session_count`
- `pro_total_ms`
- `last_played_at`

Notes:
- `last_played_at` should be derived from the max valid `last_played_unix` across the user's devices for that semsem

### 7. Admin/dashboard corrections

Fix current admin/dashboard read logic:
- online device KPI uses 5-minute TTL
- today's listening uses daily rollups, not `UsageStats.updated_at`
- admin/list pages that display listening duration should use minutes-oriented labels where the product now requires it

The underlying stored unit remains milliseconds.

## Frontend Design

### 1. Time presentation

Replace hour-focused displays with minute-focused displays for listening time.

Use raw milliseconds from the API and format into Arabic minute values.

Affected surfaces:
- dashboard cards
- devices page summary cards
- semsem detail cards

### 2. Dashboard content

Dashboard should show:
- today's listening minutes
- total listening minutes
- unique semsems
- online device count or primary device status

If layout space is limited, today's and lifetime totals can be shown as primary value + secondary subtext in the same card.

### 3. Device status copy

Keep the binary badge but align copy to recent activity semantics.

Preferred copy direction:
- online: `نشط خلال آخر 5 دقائق` or equivalent short Arabic copy
- offline: `غير نشط حالياً`

If UI space is tight, short labels can remain, but device details should also expose `last_seen_at` to reduce ambiguity.

### 4. Semsem details

Semsem detail should show both:
- today's listening minutes
- total listening minutes

If only three stats cards are kept, recommended set is:
- total play count
- today's listening minutes
- total listening minutes or last played date

## Data Flow

1. Firmware uploads cumulative snapshot.
2. Backend authenticates device; auth refresh updates `last_seen_at`.
3. Stats ingestion compares snapshot with previous stored snapshot.
4. Backend writes day delta to daily rollup row.
5. Backend overwrites latest snapshot row.
6. Frontend/admin query:
   - daily rollups for today-bound metrics
   - latest snapshots for lifetime metrics
   - devices with shared 5-minute online rule for presence

## Edge Cases

1. Unknown semsem UID:
- keep rejecting upload as today

2. First-ever upload for a `(device, uid)`:
- delta equals full snapshot values

3. Counter goes backwards:
- delta becomes zero for that field
- snapshot baseline is replaced with the new smaller value

4. Invalid timestamp from firmware:
- attribute delta to server receive day

5. Device idle for more than 5 minutes but still powered on:
- shown offline by design under the selected recent-activity semantics

6. Offline accumulation over multiple days then one upload:
- backend cannot perfectly distribute time across those days from current firmware data
- attribute to the most recent known play day

## Testing Strategy

### Backend

1. Presence tests
- online at `4m59s`
- offline at `5m00s` or later
- all affected endpoints share the same TTL behavior

2. Stats ingestion tests
- first upload creates snapshot and daily delta row
- later larger upload adds only the delta to the same day
- upload on a different day creates a second day row
- negative regression clamps to zero and resets baseline safely
- invalid `last_played_unix` falls back to server day

3. Aggregate API tests
- `/api/users/me/stats` returns today and total metrics correctly
- `/api/devices` returns enriched per-device totals and presence fields
- `/api/semsems/<uid>` returns today/total semsem metrics and last played value

### Frontend

1. Formatter tests for minute rendering
2. Page/component tests for updated labels and values
3. E2E or integration coverage for dashboard/devices/library detail rendering with new API payloads

## Migration / Rollout Notes

1. Add the daily rollup table with a forward-only migration.
2. Existing `UsageStats` rows remain valid as lifetime snapshot baselines.
3. Historical accurate "today" data only begins after the new rollup ingestion logic ships.
4. If needed, lifetime totals remain available immediately because they continue to come from `UsageStats`.

## Success Criteria

1. Device status is consistently derived from `last_seen_at` with a `5 minute` TTL across backend/admin/frontend.
2. Frontend no longer presents listening time in hours.
3. Backend exposes both today and lifetime listening totals.
4. Today's stats are based on snapshot deltas, not upload timestamps.
5. Counter resets do not create negative totals or duplicate time.
