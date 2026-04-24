# 0003 — SMF state-machine pattern for all domain modules

**Status:** accepted
**Date:** 2026-04-25 (recorded)

## Context

Multiple long-running modules (audio, semsem, semsem_pro, led, hmi, onboarding) react to events from a shared bus. Ad-hoc task design with manual state booleans is error-prone and hard for humans to read at audit time. Per S6 the firmware should be "optimized for human understanding and readability".

## Decision

Use Espressif's State Machine Framework (`firmware/components/smf/`, vendored). Every domain module:
- Runs as an SMF instance on its own task.
- Subscribes to specific `nadeem_events` IDs.
- Encodes states as enums with explicit `enter` / `run` / `exit` handlers.

## Consequences

- Public state enums (e.g. `audio_state_t`, `led_mode_t`) become first-class API.
- Easier to draw state diagrams, easier for new contributors to map "event X in state Y → action Z".
- Slight memory overhead per state machine vs hand-rolled flags; acceptable on ESP32-S3.
- LED *patterns* are pure functions for host-testability ([pages/firmware/state-machines.md](../pages/firmware/state-machines.md)).

## Sources

- S2 (`firmware/components/smf/`, every domain module)
- S6 (readability requirement)
