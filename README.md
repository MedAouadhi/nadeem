# Nadeem (نديم)

NFC-enabled audio box for Arabic-speaking children. Four components:

| Component | Stack | Directory |
|-----------|-------|-----------|
| Firmware | C, ESP-IDF 5, FreeRTOS | `firmware/` |
| Backend | Python, Django, Postgres, Redis, MinIO | `backend/` |
| Web frontend | TypeScript, Next.js, Tailwind CSS | `frontend/` |
| Mobile app | iOS / Android native | spec-only |

## Quick start (Docker Compose)

### Prerequisites

- Docker + Docker Compose v2
- Git

### 1. Clone and configure

```bash
git clone <repo-url> && cd nadeem

cp backend/.env.example backend/.env
cp frontend/.env.example frontend/.env
```

Edit `backend/.env` — at minimum set `GEMINI_API_KEY` for Pro (AI chat) features.

### 2. Start everything

```bash
docker compose up -d
```

First run builds both images (2-3 min). Subsequent starts are instant.

### 3. Create a parent account

```bash
docker compose run --rm backend python manage.py createsuperuser
```

### 4. Open the app

| Service | URL |
|---------|-----|
| Web frontend | http://localhost:3001 |
| Backend API | http://localhost:8001 |
| MinIO console | http://localhost:9005 |
| PostgreSQL | localhost:5434 |
| Redis | localhost:6381 |

### 5. Provision a device (dev)

```bash
docker compose run --rm backend python manage.py provision_dev_device \
    --email <your-email> --device-id aabbccddeeff
```

## Running tests

```bash
# Backend
docker compose run --rm backend pytest -v

# Frontend (unit)
docker compose run --rm frontend npm run test -- --run

# Frontend (E2E — requires dev server running)
cd frontend && npx playwright test
```

## Stopping

```bash
docker compose down          # stop containers, keep data
docker compose down -v       # stop and delete volumes (resets DB + MinIO)
```

## Architecture

```
┌─────────────┐     ┌──────────────┐     ┌──────────┐
│  Browser     │────▶│  Next.js BFF │────▶│  Django   │
│  (Arabic RTL)│◀────│  :3001       │◀────│  :8001    │
└─────────────┘     └──────────────┘     └────┬─────┘
                         │                     │
                         │  JWT in httpOnly     │
                         │  cookie (BFF)        │
                                               │
                    ┌──────────┬──────────┐     │
                    │          │          │     │
               ┌────▼───┐ ┌───▼──┐ ┌─────▼────┐
               │Postgres│ │Redis │ │  MinIO    │
               │ :5432  │ │:6379 │ │ :9000    │
               └────────┘ └──────┘ └──────────┘
```

- **Frontend (Next.js)** acts as a BFF — holds the JWT in an httpOnly cookie and proxies all API calls to Django. The browser never sees the JWT.
- **Backend (Django)** serves REST + WebSocket APIs. Pro semsems bridge to Gemini for live AI chat.
- **Firmware** runs on ESP32-S3 + PN532. Reads NFC tags ("semsems") and streams audio via WebSocket.

## Production checklist

- Set `DJANGO_DEBUG=false` and a strong `DJANGO_SECRET_KEY`
- Set a real `GEMINI_API_KEY`
- Set `DEV_PROVISIONING_ENABLED=false`
- Terminate TLS via a reverse proxy
- Point `S3_PUBLIC_ENDPOINT_URL` to a CDN

## Project docs

- `backend/README.md` — backend specifics, dev provisioning, tests
- `frontend/design.md` — UI mockups and design system
- `prd_back_front.md` — product requirements for backend + frontend
- `wiki/` — project knowledge base
- `AGENTS.md` — operating instructions for AI coding agents
