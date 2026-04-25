# Nadeem Backend

Django 5 + DRF + Channels + Postgres + Redis + MinIO. WebSocket /chat is bridged to Google Gemini 3.1 Flash Live Preview.

## Run locally

```bash
cp .env.example .env
docker compose up -d
docker compose run --rm backend python manage.py migrate
```

Backend at http://localhost:8000. MinIO console at http://localhost:9001.

## Run tests

```bash
docker compose run --rm backend pytest -v
```
