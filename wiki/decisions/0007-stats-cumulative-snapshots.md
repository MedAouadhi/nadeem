# 0007 — Stats are cumulative snapshots, not deltas

**Status:** accepted
**Date:** 2026-04-25 (recorded)

## Context

Firmware tracks per-UID counters and uploads them. Two design choices:

A. **Deltas:** firmware sends "play_count += 3" — backend accumulates. Risk: lost upload = lost data.
B. **Cumulative snapshots:** firmware sends "play_count = 7" — backend stores the latest, computes deltas server-side.

## Decision

Cumulative snapshots. Counters are monotonically increasing on the device, persisted in NVS namespace `nadeem_stat`, and uploaded as a full snapshot per UID per 5-min tick.

## Consequences

- **Idempotent uploads.** Repeated identical posts are safe; the server overwrites with the same value.
- **Lossy-network tolerant.** A skipped tick just means the next snapshot is a bigger jump.
- **One UID per `POST /stats` request** (v0.1) — not a batch. Bursts when multiple UIDs are dirty.
- Backend must store last `(device_id, uid)` value and compute deltas for analytics.
- Resetting counters (e.g. factory reset) creates a "negative delta" that the backend should detect and treat as a new history segment, not as data loss.

## Sources

- S2 (`firmware/components/stats/`, `firmware/components/backend_client/`)
- S3 §6.1
