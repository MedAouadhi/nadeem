# 0005 — Django + PostgreSQL + Redis for backend

**Status:** proposed (PRD-stated, not yet implemented)
**Date:** 2026-04-25 (recorded)

## Context

Need a backend that handles: REST + WebSocket; OAuth/session auth for parents; long-running chat sessions with transcripts; aggregate stats; Dockerized deploys. Team familiarity matters; Python ASGI ecosystem is mature for the websocket + ASR/LLM/TTS combo.

## Decision

- **Framework:** Django (with Channels for WebSocket).
- **DB:** PostgreSQL.
- **Cache / queues:** Redis.
- **Infra:** Dockerized. `docker-compose` for dev and prod.
- **TLS:** mandatory in prod.

## Consequences

- Django ORM defines the schema in [pages/backend/data-model.md](../pages/backend/data-model.md).
- Channels (or Daphne / Uvicorn) required for `WS /chat`.
- Redis used for: session store, channel layer, async task queue (Celery/RQ for ASR/LLM if not inline).
- Choice constrains hosting options (postgres-friendly, container-native).
- Migration to async-first FastAPI later would be a rewrite. Worth flagging if WebSocket throughput becomes a bottleneck.

## Sources

- S7 §1.1
- S8
