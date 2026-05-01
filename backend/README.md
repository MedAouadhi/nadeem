# Nadeem Backend

Django 5 + DRF + Channels + Postgres + Redis + MinIO. WebSocket /chat is bridged to Google Gemini 3.1 Flash Live Preview.

## Run locally

```bash
cp .env.example .env
docker compose up -d
docker compose run --rm backend python manage.py migrate
```

Backend at http://localhost:8000. MinIO console at http://localhost:9001.

## Dev provisioning (TEMPORARY)
There is no mobile companion app yet. To bind a device to a user during dev:

**Option A — management command (admin):**
```bash
docker compose run --rm backend python manage.py provision_dev_device \
    --email parent@example.com --device-id aabbccddeeff
```
Prints the `device_token` once. Save it; firmware uses it as a Bearer token.

**Option B — HTTP endpoint, gated by `DEV_PROVISIONING_ENABLED`:**
```bash
curl -X POST http://localhost:8000/api/dev/devices \
     -H "Authorization: Bearer <user_jwt>" -H "Content-Type: application/json" \
     -d '{"device_id":"aabbccddeeff"}'
```

### When the real mobile app ships, remove this:
1. Set `DEV_PROVISIONING_ENABLED=false` in prod env.
2. Delete `devices/management/commands/provision_dev_device.py`.
3. Delete `DevProvisionView` from `devices/views.py` and its URL entry.
4. Drop `DEV_PROVISIONING_ENABLED` from settings + `.env.example`.

## Run tests

```bash
docker compose run --rm backend pytest -v
```

## Production checklist

- Set `DJANGO_DEBUG=false`.
- Set a strong `DJANGO_SECRET_KEY`.
- Set a real `GEMINI_API_KEY`.
- Set `DEV_PROVISIONING_ENABLED=false`.
- Terminate TLS upstream (reverse proxy / load balancer).
- Point `S3_PUBLIC_ENDPOINT_URL` to a CDN.
