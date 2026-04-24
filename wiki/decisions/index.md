# Architectural Decisions Index

Lightweight ADRs. One file per decision. New decisions append; supersession links upward.

| # | Decision | Status |
|---|----------|--------|
| 0001 | [Waveshare ESP32-S3-AUDIO-Board as devkit](0001-esp32s3-audio-board.md) | accepted |
| 0002 | [PN532 v3 over UART1 (HSU mode)](0002-pn532-uart.md) | accepted |
| 0003 | [SMF state-machine pattern for all domain modules](0003-smf-state-machines.md) | accepted |
| 0004 | [16 kHz mono PCM16, 20 ms frames for chat WebSocket](0004-pcm16-16k-mono.md) | accepted |
| 0005 | [Django + PostgreSQL + Redis for backend](0005-django-postgres-backend.md) | proposed |
| 0006 | [Next.js + Tailwind, Arabic-only RTL for frontend](0006-nextjs-rtl-arabic.md) | proposed |
| 0007 | [Stats are cumulative snapshots, not deltas](0007-stats-cumulative-snapshots.md) | accepted |

## Open decisions (no ADR yet)

- How does the device learn `backend_url`? Currently out-of-band; v0.1 expects pre-bake or console.
- Chat transcript retention policy (privacy / parental deletion).
- Catalog mutation API surface (assigning playlists to UIDs, marking Pro).
- Manifest cache invalidation strategy (currently requires factory reset).

Tracked in [../lint.md](../lint.md).
