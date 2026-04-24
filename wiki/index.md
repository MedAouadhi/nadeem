# Nadeem Wiki Index

Catalog of wiki pages. Human-readable; LLM-readable up to ~100 pages then add hybrid search per [llm-wiki.md §search](../llm-wiki.md).

Schema: [AGENTS.md](AGENTS.md). Sources: [sources.md](sources.md). Health: [lint.md](lint.md).

## System

- [pages/system/architecture.md](pages/system/architecture.md) — four-component layout, where the seams are
- [pages/system/glossary.md](pages/system/glossary.md) — semsem, device_id, uid_hex, provision_token, device_token, role
- [pages/system/workflows.md](pages/system/workflows.md) — bootstrap, regular playback, pro chat, stats upload
- [pages/system/constraints.md](pages/system/constraints.md) — hardware, audio format, RTL, TLS, file sizes
- [pages/system/component-status.md](pages/system/component-status.md) — implemented vs spec-only

## Firmware (implemented)

- [pages/firmware/overview.md](pages/firmware/overview.md) — boot path, mode selection, ESP-IDF + SMF
- [pages/firmware/components.md](pages/firmware/components.md) — table of components and responsibilities
- [pages/firmware/events.md](pages/firmware/events.md) — `nadeem_events` bus IDs and payloads
- [pages/firmware/state-machines.md](pages/firmware/state-machines.md) — audio, semsem, semsem-pro, led modes
- [pages/firmware/onboarding.md](pages/firmware/onboarding.md) — SoftAP + provision flow
- [pages/firmware/hardware.md](pages/firmware/hardware.md) — board, codecs, NFC wiring, GPIO map
- [pages/firmware/console.md](pages/firmware/console.md) — REPL commands for dev/test
- [pages/firmware/limitations.md](pages/firmware/limitations.md) — known v0.1 caveats

## Backend (spec-only)

- [pages/backend/overview.md](pages/backend/overview.md) — Django/Postgres/Redis, what it owns
- [pages/backend/contract.md](pages/backend/contract.md) — every endpoint the firmware calls
- [pages/backend/data-model.md](pages/backend/data-model.md) — User, Device, Semsem, Manifest, Stats, ProChatSession
- [pages/backend/chat-pipeline.md](pages/backend/chat-pipeline.md) — VAD/ASR/LLM/TTS responsibilities
- [pages/backend/security.md](pages/backend/security.md) — TLS, token revocation, rate limits

## Frontend (spec-only)

- [pages/frontend/overview.md](pages/frontend/overview.md) — Next.js, Tailwind, RTL
- [pages/frontend/screens.md](pages/frontend/screens.md) — login, dashboard, library, chat history, devices
- [pages/frontend/design-system.md](pages/frontend/design-system.md) — colors, typography, badging
- [pages/frontend/api-mapping.md](pages/frontend/api-mapping.md) — view ↔ endpoint table

## Mobile companion (spec-only)

- [pages/mobile/overview.md](pages/mobile/overview.md) — single-purpose onboarding app
- [pages/mobile/local-api.md](pages/mobile/local-api.md) — `192.168.4.1` HTTP surface
- [pages/mobile/flow.md](pages/mobile/flow.md) — happy path and failure modes

## Graph

- [graph/entities.md](graph/entities.md) — typed entity registry
- [graph/relationships.md](graph/relationships.md) — typed edges for traversal

## Decisions

- [decisions/index.md](decisions/index.md) — ADR roll-up
- [decisions/0001-esp32s3-audio-board.md](decisions/0001-esp32s3-audio-board.md)
- [decisions/0002-pn532-uart.md](decisions/0002-pn532-uart.md)
- [decisions/0003-smf-state-machines.md](decisions/0003-smf-state-machines.md)
- [decisions/0004-pcm16-16k-mono.md](decisions/0004-pcm16-16k-mono.md)
- [decisions/0005-django-postgres-backend.md](decisions/0005-django-postgres-backend.md)
- [decisions/0006-nextjs-rtl-arabic.md](decisions/0006-nextjs-rtl-arabic.md)
- [decisions/0007-stats-cumulative-snapshots.md](decisions/0007-stats-cumulative-snapshots.md)

## Crystallization (episodic digests)

- _empty — file digests under `crystallization/` as investigations close_
