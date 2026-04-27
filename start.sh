#!/bin/bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
MODE="${1:-}"

if [ -z "$MODE" ] || { [ "$MODE" != "dev" ] && [ "$MODE" != "production" ]; }; then
    echo "Usage: $(basename "$0") [dev|production]"
    exit 1
fi

cd "$SCRIPT_DIR"

# Compose file selection
DEV_COMPOSE="docker-compose.yml"
PROD_COMPOSE="docker-compose.prod.yml"

echo "=== Checking prerequisites ==="

# Check env files exist
if [ "$MODE" = "dev" ]; then
    if [ ! -f "backend/.env" ]; then
        echo "ERROR: backend/.env missing. Run: cp backend/.env.example backend/.env" >&2
        exit 1
    fi
    if [ ! -f "frontend/.env" ]; then
        echo "ERROR: frontend/.env missing. Run: cp frontend/.env.example frontend/.env" >&2
        exit 1
    fi
else
    if [ ! -f "backend/.env.prod" ]; then
        echo "ERROR: backend/.env.prod missing. Run: cp backend/.env.prod.example backend/.env.prod" >&2
        exit 1
    fi
    if [ ! -f "frontend/.env.prod" ]; then
        echo "ERROR: frontend/.env.prod missing. Run: cp frontend/.env.prod.example frontend/.env.prod" >&2
        exit 1
    fi
fi

echo "=== Stopping and removing any existing services ==="
for f in "$DEV_COMPOSE" "$PROD_COMPOSE"; do
    if [ -f "$f" ]; then
        docker compose -f "$f" down --remove-orphans -v 2>/dev/null || true
    fi
done

# Check ports are free
if [ "$MODE" = "dev" ]; then
    PORTS=(3001 8001 5434 6381 9004 9005)
else
    PORTS=(3000 8000 5432 6379 9000 9001)
fi

for port in "${PORTS[@]}"; do
    if ss -tln | awk '{print $4}' | grep -qE ":${port}$"; then
        echo "ERROR: Port ${port} is already in use. Free it or change the compose file." >&2
        ss -tln | grep -E ":${port}$" >&2
        exit 1
    fi
done

echo ""
if [ "$MODE" = "dev" ]; then
    echo "=== Starting DEVELOPMENT environment ==="
    docker compose -f "$DEV_COMPOSE" up -d --build

    echo ""
    echo "Services are up:"
    echo "  Web frontend  → http://localhost:3001"
    echo "  Backend API   → http://localhost:8001"
    echo "  MinIO console → http://localhost:9005"
    echo "  PostgreSQL    → localhost:5434"
    echo "  Redis         → localhost:6381"
    echo ""
    echo "Create a superuser:"
    echo "  docker compose run --rm backend python manage.py createsuperuser"
    echo "Provision a dev device:"
    echo "  docker compose run --rm backend python manage.py provision_dev_device \\"
    echo "      --email <your-email> --device-id aabbccddeeff"

else
    echo "=== Starting PRODUCTION environment ==="
    if [ ! -f "$PROD_COMPOSE" ]; then
        echo "ERROR: $PROD_COMPOSE not found. Something is wrong." >&2
        exit 1
    fi
    docker compose -f "$PROD_COMPOSE" up -d --build

    echo ""
    echo "Services are up:"
    echo "  Web frontend  → http://localhost:3000"
    echo "  Backend API   → http://localhost:8000"
    echo "  MinIO console → http://localhost:9001"
    echo ""
    echo "Production checklist:"
    echo "  - Ensure DJANGO_DEBUG=false"
    echo "  - Ensure a strong DJANGO_SECRET_KEY set"
    echo "  - Ensure GEMINI_API_KEY is set"
    echo "  - Set DEV_PROVISIONING_ENABLED=false"
    echo "  - Terminate TLS via a reverse proxy"
fi

echo ""
echo "=== Container status ==="
docker compose -f "$([ "$MODE" = "dev" ] && echo "$DEV_COMPOSE" || echo "$PROD_COMPOSE")" ps
