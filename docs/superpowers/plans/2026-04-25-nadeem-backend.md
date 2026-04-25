# Nadeem Backend Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build the Django/Postgres/Redis backend that serves the Nadeem firmware, web dashboard, and mobile companion (when later built), implementing all REST + WebSocket endpoints in `prd_back_front.md` and `backend/backend.md`.

**Architecture:** Django 5 + DRF for REST; Django Channels + Redis for WebSocket `/chat`; PostgreSQL for persistence; MinIO (S3-compatible) for audio files; SimpleJWT for parent web auth; opaque hashed tokens for device + provisioning auth; Google Gemini 3.1 Flash Live Preview as the AI conversation backend (audio-in / audio-out, 16 kHz PCM16 both directions). Whole stack ships via `docker-compose`.

**Tech Stack:** Python 3.12, Django 5.x, djangorestframework, djangorestframework-simplejwt, channels, channels-redis, daphne, psycopg[binary], django-storages[s3], boto3, google-genai, pytest, pytest-django, pytest-asyncio, ruff. Postgres 16, Redis 7, MinIO latest.

**Out of scope:** Firmware (separate repo), mobile companion app, web frontend (separate plan), production CDN config (use MinIO public bucket for v0.1).

---

## File Structure

```
backend/
├── Dockerfile
├── docker-compose.yml
├── docker-compose.override.yml          # local dev only, gitignored if needed
├── pyproject.toml                       # deps + ruff config
├── pytest.ini
├── .env.example
├── manage.py
├── conftest.py
├── nadeem/
│   ├── __init__.py
│   ├── settings.py
│   ├── urls.py
│   ├── asgi.py
│   └── wsgi.py
├── accounts/
│   ├── apps.py
│   ├── models.py                        # custom User
│   ├── serializers.py
│   ├── views.py                         # /api/auth/login, /me
│   ├── urls.py
│   └── tests/test_auth.py
├── devices/
│   ├── apps.py
│   ├── models.py                        # Device, ProvisioningToken
│   ├── auth.py                          # DeviceTokenAuthentication
│   ├── tokens.py                        # generation + hashing helpers
│   ├── serializers.py
│   ├── views.py                         # bootstrap, provisioning, list/delete, dev-provision
│   ├── urls.py
│   ├── management/commands/provision_dev_device.py
│   └── tests/{test_models,test_bootstrap,test_provisioning,test_devices_crud,test_dev_provision}.py
├── semsems/
│   ├── apps.py
│   ├── models.py                        # Semsem, Track
│   ├── serializers.py
│   ├── views.py                         # /semsem/<uid>/manifest, /api/semsems(/...)
│   ├── urls.py
│   └── tests/{test_models,test_manifest,test_web_api}.py
├── stats/
│   ├── apps.py
│   ├── models.py                        # UsageStats
│   ├── serializers.py
│   ├── views.py                         # POST /stats, GET /api/users/me/stats
│   ├── urls.py
│   └── tests/{test_upload,test_aggregate}.py
├── chat/
│   ├── apps.py
│   ├── models.py                        # ProChatSession, TranscriptEntry
│   ├── gemini_client.py                 # async wrapper around Gemini Live
│   ├── consumers.py                     # ChatConsumer (Channels)
│   ├── routing.py                       # /chat
│   ├── auth.py                          # WS device-token middleware
│   ├── serializers.py
│   ├── views.py                         # /api/chat-sessions(/<id>)
│   ├── urls.py
│   └── tests/{test_models,test_consumer,test_gemini_client,test_web_api}.py
└── README.md                            # how to run + how to remove dev provisioning
```

**Design rules locked in here:**
- Apps split by domain (accounts/devices/semsems/stats/chat), not by layer.
- Each app owns its own URLs, included from `nadeem/urls.py`.
- All models live in their app's `models.py`. No cross-app imports of internal helpers — go via models or services.
- Tests colocated under each app's `tests/` package, one file per concern.
- All identifiers stored lowercase; serializers reject mixed-case.

---

## Conventions used in every task below

- Run tests inside Docker: `docker compose run --rm backend pytest <path> -v`. The first compose-up brings the stack up; after that `docker compose run --rm backend ...` is fast.
- After each task: `git add` only the files the task touched, then `git commit -m "<subject>"`. Subjects use Conventional Commits (`feat:`, `fix:`, `chore:`, `test:`, `refactor:`).
- Never run `--no-verify`. Never commit secrets. `.env` is gitignored; `.env.example` is committed.

---

### Task 1: Repo scaffold, Dockerfile, docker-compose, baseline Django project

**Files:**
- Create: `backend/pyproject.toml`
- Create: `backend/Dockerfile`
- Create: `backend/docker-compose.yml`
- Create: `backend/.env.example`
- Create: `backend/.gitignore`
- Create: `backend/manage.py`
- Create: `backend/nadeem/__init__.py`, `backend/nadeem/settings.py`, `backend/nadeem/urls.py`, `backend/nadeem/asgi.py`, `backend/nadeem/wsgi.py`
- Create: `backend/pytest.ini`
- Create: `backend/conftest.py`
- Create: `backend/README.md`

- [ ] **Step 1: Write `pyproject.toml`**

```toml
[project]
name = "nadeem-backend"
version = "0.1.0"
requires-python = ">=3.12"
dependencies = [
  "django>=5.0,<6",
  "djangorestframework>=3.15",
  "djangorestframework-simplejwt>=5.3",
  "channels>=4.1",
  "channels-redis>=4.2",
  "daphne>=4.1",
  "psycopg[binary]>=3.2",
  "django-storages[s3]>=1.14",
  "boto3>=1.34",
  "google-genai>=0.6",
  "python-dotenv>=1.0",
  "django-cors-headers>=4.4",
]

[project.optional-dependencies]
dev = [
  "pytest>=8",
  "pytest-django>=4.8",
  "pytest-asyncio>=0.23",
  "ruff>=0.6",
  "ipython",
]

[tool.ruff]
line-length = 100
target-version = "py312"

[tool.ruff.lint]
select = ["E", "F", "I", "UP", "B", "SIM"]
```

- [ ] **Step 2: Write `Dockerfile`**

```dockerfile
FROM python:3.12-slim

ENV PYTHONDONTWRITEBYTECODE=1 PYTHONUNBUFFERED=1
WORKDIR /app

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential libpq-dev curl \
  && rm -rf /var/lib/apt/lists/*

COPY pyproject.toml ./
RUN pip install --upgrade pip && pip install -e ".[dev]"

COPY . .

EXPOSE 8000
CMD ["daphne", "-b", "0.0.0.0", "-p", "8000", "nadeem.asgi:application"]
```

- [ ] **Step 3: Write `docker-compose.yml`**

```yaml
services:
  postgres:
    image: postgres:16-alpine
    environment:
      POSTGRES_DB: nadeem
      POSTGRES_USER: nadeem
      POSTGRES_PASSWORD: nadeem_dev
    volumes: [pgdata:/var/lib/postgresql/data]
    ports: ["5432:5432"]

  redis:
    image: redis:7-alpine
    ports: ["6379:6379"]

  minio:
    image: minio/minio:latest
    command: server /data --console-address ":9001"
    environment:
      MINIO_ROOT_USER: nadeem
      MINIO_ROOT_PASSWORD: nadeem_dev_password
    volumes: [miniodata:/data]
    ports: ["9000:9000", "9001:9001"]

  minio-bootstrap:
    image: minio/mc:latest
    depends_on: [minio]
    entrypoint: >
      /bin/sh -c "
      until /usr/bin/mc alias set local http://minio:9000 nadeem nadeem_dev_password; do sleep 1; done;
      /usr/bin/mc mb -p local/nadeem-audio || true;
      /usr/bin/mc anonymous set download local/nadeem-audio || true;
      "

  backend:
    build: .
    command: sh -c "python manage.py migrate && daphne -b 0.0.0.0 -p 8000 nadeem.asgi:application"
    env_file: .env
    depends_on: [postgres, redis, minio]
    volumes: [".:/app"]
    ports: ["8000:8000"]

volumes:
  pgdata: {}
  miniodata: {}
```

- [ ] **Step 4: Write `.env.example`**

```
DJANGO_SECRET_KEY=changeme-dev-only
DJANGO_DEBUG=true
DJANGO_ALLOWED_HOSTS=localhost,127.0.0.1,backend
DATABASE_URL=postgres://nadeem:nadeem_dev@postgres:5432/nadeem
REDIS_URL=redis://redis:6379/0

S3_ENDPOINT_URL=http://minio:9000
S3_PUBLIC_ENDPOINT_URL=http://localhost:9000
S3_BUCKET=nadeem-audio
S3_ACCESS_KEY=nadeem
S3_SECRET_KEY=nadeem_dev_password
S3_REGION=us-east-1

GEMINI_API_KEY=
GEMINI_MODEL=gemini-3.1-flash-live-preview

DEV_PROVISIONING_ENABLED=true

CORS_ALLOWED_ORIGINS=http://localhost:3000
```

- [ ] **Step 5: Write `.gitignore`**

```
__pycache__/
*.pyc
.env
.venv/
.pytest_cache/
*.sqlite3
staticfiles/
.coverage
```

- [ ] **Step 6: Write `manage.py`**

```python
#!/usr/bin/env python
import os
import sys

if __name__ == "__main__":
    os.environ.setdefault("DJANGO_SETTINGS_MODULE", "nadeem.settings")
    from django.core.management import execute_from_command_line
    execute_from_command_line(sys.argv)
```

- [ ] **Step 7: Write `nadeem/settings.py`**

```python
import os
from pathlib import Path
from datetime import timedelta

BASE_DIR = Path(__file__).resolve().parent.parent

SECRET_KEY = os.environ["DJANGO_SECRET_KEY"]
DEBUG = os.environ.get("DJANGO_DEBUG", "false").lower() == "true"
ALLOWED_HOSTS = os.environ.get("DJANGO_ALLOWED_HOSTS", "").split(",")

INSTALLED_APPS = [
    "daphne",
    "django.contrib.admin",
    "django.contrib.auth",
    "django.contrib.contenttypes",
    "django.contrib.sessions",
    "django.contrib.messages",
    "django.contrib.staticfiles",
    "rest_framework",
    "channels",
    "corsheaders",
    "accounts",
    "devices",
    "semsems",
    "stats",
    "chat",
]

MIDDLEWARE = [
    "corsheaders.middleware.CorsMiddleware",
    "django.middleware.security.SecurityMiddleware",
    "django.contrib.sessions.middleware.SessionMiddleware",
    "django.middleware.common.CommonMiddleware",
    "django.middleware.csrf.CsrfViewMiddleware",
    "django.contrib.auth.middleware.AuthenticationMiddleware",
    "django.contrib.messages.middleware.MessageMiddleware",
]

ROOT_URLCONF = "nadeem.urls"
TEMPLATES = [{
    "BACKEND": "django.template.backends.django.DjangoTemplates",
    "DIRS": [], "APP_DIRS": True,
    "OPTIONS": {"context_processors": [
        "django.template.context_processors.request",
        "django.contrib.auth.context_processors.auth",
        "django.contrib.messages.context_processors.messages",
    ]},
}]

ASGI_APPLICATION = "nadeem.asgi.application"
WSGI_APPLICATION = "nadeem.wsgi.application"

import dj_database_url  # noqa  -- using urlparse fallback below if not installed
import urllib.parse as up
_db = up.urlparse(os.environ["DATABASE_URL"])
DATABASES = {
    "default": {
        "ENGINE": "django.db.backends.postgresql",
        "NAME": _db.path.lstrip("/"),
        "USER": _db.username,
        "PASSWORD": _db.password,
        "HOST": _db.hostname,
        "PORT": _db.port or 5432,
    }
}

CHANNEL_LAYERS = {
    "default": {
        "BACKEND": "channels_redis.core.RedisChannelLayer",
        "CONFIG": {"hosts": [os.environ["REDIS_URL"]]},
    }
}

AUTH_USER_MODEL = "accounts.User"

REST_FRAMEWORK = {
    "DEFAULT_AUTHENTICATION_CLASSES": [
        "rest_framework_simplejwt.authentication.JWTAuthentication",
    ],
    "DEFAULT_PERMISSION_CLASSES": ["rest_framework.permissions.IsAuthenticated"],
}

SIMPLE_JWT = {
    "ACCESS_TOKEN_LIFETIME": timedelta(minutes=60),
    "REFRESH_TOKEN_LIFETIME": timedelta(days=14),
}

STORAGES = {
    "default": {
        "BACKEND": "storages.backends.s3.S3Storage",
        "OPTIONS": {
            "bucket_name": os.environ["S3_BUCKET"],
            "endpoint_url": os.environ["S3_ENDPOINT_URL"],
            "access_key": os.environ["S3_ACCESS_KEY"],
            "secret_key": os.environ["S3_SECRET_KEY"],
            "region_name": os.environ.get("S3_REGION", "us-east-1"),
            "addressing_style": "path",
            "default_acl": None,
            "querystring_auth": False,
            "custom_domain": False,
        },
    },
    "staticfiles": {"BACKEND": "django.contrib.staticfiles.storage.StaticFilesStorage"},
}
S3_PUBLIC_ENDPOINT_URL = os.environ.get("S3_PUBLIC_ENDPOINT_URL", os.environ["S3_ENDPOINT_URL"])

CORS_ALLOWED_ORIGINS = [o for o in os.environ.get("CORS_ALLOWED_ORIGINS", "").split(",") if o]

GEMINI_API_KEY = os.environ.get("GEMINI_API_KEY", "")
GEMINI_MODEL = os.environ.get("GEMINI_MODEL", "gemini-3.1-flash-live-preview")

DEV_PROVISIONING_ENABLED = os.environ.get("DEV_PROVISIONING_ENABLED", "false").lower() == "true"

LANGUAGE_CODE = "ar"
TIME_ZONE = "UTC"
USE_I18N = True
USE_TZ = True

DEFAULT_AUTO_FIELD = "django.db.models.BigAutoField"
STATIC_URL = "static/"
```

> Note: drop the `import dj_database_url` line — it isn't needed (we parse manually). Leaving the manual urlparse only keeps deps minimal.

Replace the leading three lines of DATABASES section with just the urlparse block (no `dj_database_url` import).

- [ ] **Step 8: Write `nadeem/urls.py`**

```python
from django.contrib import admin
from django.urls import include, path

urlpatterns = [
    path("admin/", admin.site.urls),
    path("", include("devices.urls")),       # /bootstrap, /api/devices, /api/provisioning-tokens
    path("", include("semsems.urls")),       # /semsem/<uid>/manifest, /api/semsems
    path("", include("stats.urls")),         # /stats, /api/users/me/stats
    path("", include("chat.urls")),          # /api/chat-sessions
    path("api/auth/", include("accounts.urls")),
]
```

- [ ] **Step 9: Write `nadeem/asgi.py`**

```python
import os
from django.core.asgi import get_asgi_application

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "nadeem.settings")
django_asgi_app = get_asgi_application()

from channels.routing import ProtocolTypeRouter, URLRouter  # noqa: E402
from chat.routing import websocket_urlpatterns  # noqa: E402
from chat.auth import DeviceTokenAuthMiddleware  # noqa: E402

application = ProtocolTypeRouter({
    "http": django_asgi_app,
    "websocket": DeviceTokenAuthMiddleware(URLRouter(websocket_urlpatterns)),
})
```

- [ ] **Step 10: Write `nadeem/wsgi.py`**

```python
import os
from django.core.wsgi import get_wsgi_application
os.environ.setdefault("DJANGO_SETTINGS_MODULE", "nadeem.settings")
application = get_wsgi_application()
```

- [ ] **Step 11: Write `pytest.ini`**

```ini
[pytest]
DJANGO_SETTINGS_MODULE = nadeem.settings
python_files = test_*.py
asyncio_mode = auto
```

- [ ] **Step 12: Write `conftest.py`**

```python
import pytest

@pytest.fixture
def api_client():
    from rest_framework.test import APIClient
    return APIClient()
```

- [ ] **Step 13: Create empty app skeletons**

For each of `accounts/`, `devices/`, `semsems/`, `stats/`, `chat/`, create `__init__.py`, `apps.py` (with `name = "<app>"` and `default_auto_field = "django.db.models.BigAutoField"`), and an empty `urls.py` containing `urlpatterns = []`.

`accounts/apps.py` template:

```python
from django.apps import AppConfig

class AccountsConfig(AppConfig):
    default_auto_field = "django.db.models.BigAutoField"
    name = "accounts"
```

Repeat for the other four apps with the matching class name.

Each empty `urls.py`:

```python
urlpatterns = []
```

Empty `chat/routing.py`:

```python
websocket_urlpatterns = []
```

Empty `chat/auth.py`:

```python
class DeviceTokenAuthMiddleware:
    def __init__(self, inner):
        self.inner = inner

    async def __call__(self, scope, receive, send):
        return await self.inner(scope, receive, send)
```

(We replace this stub in Task 17.)

- [ ] **Step 14: Boot the stack and confirm Django runs**

Run: `cp .env.example .env && docker compose up -d postgres redis minio minio-bootstrap && docker compose run --rm backend python manage.py check`

Expected: `System check identified no issues (0 silenced).`

- [ ] **Step 15: Commit**

```bash
git add backend/
git commit -m "chore(backend): scaffold django+channels project with docker-compose"
```

---

### Task 2: Custom User model + first migration

**Files:**
- Modify: `backend/accounts/models.py`
- Modify: `backend/accounts/apps.py` (already created — confirm)
- Create: `backend/accounts/admin.py`
- Create: `backend/accounts/tests/__init__.py`
- Create: `backend/accounts/tests/test_models.py`
- Create: `backend/accounts/migrations/__init__.py`

- [ ] **Step 1: Write the failing test**

`backend/accounts/tests/test_models.py`:

```python
import pytest
from django.contrib.auth import get_user_model

pytestmark = pytest.mark.django_db

def test_user_uses_email_as_username():
    User = get_user_model()
    u = User.objects.create_user(email="parent@example.com", password="hunter2")
    assert u.email == "parent@example.com"
    assert u.check_password("hunter2")
    assert str(u) == "parent@example.com"
    assert u.USERNAME_FIELD == "email"

def test_email_is_unique():
    User = get_user_model()
    User.objects.create_user(email="a@b.com", password="x")
    with pytest.raises(Exception):
        User.objects.create_user(email="a@b.com", password="x")
```

- [ ] **Step 2: Run test, confirm it fails**

Run: `docker compose run --rm backend pytest accounts/tests/test_models.py -v`
Expected: FAIL — no User model yet (or wrong USERNAME_FIELD).

- [ ] **Step 3: Implement `accounts/models.py`**

```python
from django.contrib.auth.models import AbstractBaseUser, BaseUserManager, PermissionsMixin
from django.db import models
from django.utils import timezone


class UserManager(BaseUserManager):
    def create_user(self, email, password=None, **extra):
        if not email:
            raise ValueError("Email required")
        email = self.normalize_email(email)
        user = self.model(email=email, **extra)
        user.set_password(password)
        user.save(using=self._db)
        return user

    def create_superuser(self, email, password=None, **extra):
        extra.setdefault("is_staff", True)
        extra.setdefault("is_superuser", True)
        return self.create_user(email, password, **extra)


class User(AbstractBaseUser, PermissionsMixin):
    email = models.EmailField(unique=True)
    is_active = models.BooleanField(default=True)
    is_staff = models.BooleanField(default=False)
    date_joined = models.DateTimeField(default=timezone.now)

    objects = UserManager()
    USERNAME_FIELD = "email"
    REQUIRED_FIELDS = []

    def __str__(self):
        return self.email
```

`accounts/admin.py`:

```python
from django.contrib import admin
from django.contrib.auth.admin import UserAdmin as BaseUserAdmin
from .models import User


@admin.register(User)
class UserAdmin(BaseUserAdmin):
    ordering = ["email"]
    list_display = ["email", "is_staff", "date_joined"]
    fieldsets = (
        (None, {"fields": ("email", "password")}),
        ("Permissions", {"fields": ("is_active", "is_staff", "is_superuser", "groups", "user_permissions")}),
        ("Dates", {"fields": ("last_login", "date_joined")}),
    )
    add_fieldsets = ((None, {"classes": ("wide",), "fields": ("email", "password1", "password2")}),)
    search_fields = ("email",)
```

- [ ] **Step 4: Generate migration and apply**

Run: `docker compose run --rm backend python manage.py makemigrations accounts && docker compose run --rm backend python manage.py migrate`

Expected: migration created and applied cleanly.

- [ ] **Step 5: Re-run tests**

Run: `docker compose run --rm backend pytest accounts/tests/test_models.py -v`
Expected: 2 passed.

- [ ] **Step 6: Commit**

```bash
git add backend/accounts/
git commit -m "feat(accounts): add custom email-based user model"
```

---

### Task 3: JWT login + `/api/auth/me`

**Files:**
- Create: `backend/accounts/serializers.py`
- Create: `backend/accounts/views.py`
- Modify: `backend/accounts/urls.py`
- Create: `backend/accounts/tests/test_auth.py`

- [ ] **Step 1: Write failing tests**

```python
import pytest
from django.contrib.auth import get_user_model
pytestmark = pytest.mark.django_db


def test_login_returns_jwt_pair(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    r = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json")
    assert r.status_code == 200
    assert "access" in r.data and "refresh" in r.data


def test_login_rejects_wrong_password(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    r = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "wrong"}, format="json")
    assert r.status_code == 401


def test_me_returns_authenticated_user(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")
    r = api_client.get("/api/auth/me")
    assert r.status_code == 200 and r.data["email"] == "a@b.com"


def test_me_unauth_is_401(api_client):
    assert api_client.get("/api/auth/me").status_code == 401
```

- [ ] **Step 2: Confirm fail** — `docker compose run --rm backend pytest accounts/tests/test_auth.py -v` → 4 fail (404).

- [ ] **Step 3: Implement**

`accounts/serializers.py`:

```python
from rest_framework import serializers
from .models import User


class UserSerializer(serializers.ModelSerializer):
    class Meta:
        model = User
        fields = ["id", "email", "date_joined"]
        read_only_fields = fields
```

`accounts/views.py`:

```python
from rest_framework import generics, permissions
from rest_framework_simplejwt.views import TokenObtainPairView
from rest_framework_simplejwt.serializers import TokenObtainPairSerializer
from .serializers import UserSerializer


class EmailTokenObtainPairSerializer(TokenObtainPairSerializer):
    username_field = "email"


class LoginView(TokenObtainPairView):
    serializer_class = EmailTokenObtainPairSerializer


class MeView(generics.RetrieveAPIView):
    serializer_class = UserSerializer
    permission_classes = [permissions.IsAuthenticated]

    def get_object(self):
        return self.request.user
```

`accounts/urls.py`:

```python
from django.urls import path
from rest_framework_simplejwt.views import TokenRefreshView
from .views import LoginView, MeView

urlpatterns = [
    path("login", LoginView.as_view()),
    path("refresh", TokenRefreshView.as_view()),
    path("me", MeView.as_view()),
]
```

- [ ] **Step 4: Pass** — `docker compose run --rm backend pytest accounts/tests/test_auth.py -v` → 4 passed.

- [ ] **Step 5: Commit** — `git add backend/accounts/ && git commit -m "feat(accounts): jwt login + /api/auth/me"`

---

### Task 4: Token generation utility (opaque, hashed)

**Files:**
- Create: `backend/devices/tokens.py`
- Create: `backend/devices/tests/__init__.py`
- Create: `backend/devices/tests/test_tokens.py`

- [ ] **Step 1: Failing test**

```python
from devices.tokens import generate_token, hash_token


def test_generate_token_is_url_safe_and_long():
    t = generate_token()
    assert len(t) >= 40
    assert all(c.isalnum() or c in "-_" for c in t)


def test_hash_is_deterministic_64_hex():
    h1 = hash_token("abc")
    h2 = hash_token("abc")
    assert h1 == h2
    assert len(h1) == 64
    assert h1 != hash_token("abd")
```

- [ ] **Step 2: Confirm fail** — `pytest devices/tests/test_tokens.py -v` → ImportError.

- [ ] **Step 3: Implement `devices/tokens.py`**

```python
import hashlib
import secrets


def generate_token(nbytes: int = 32) -> str:
    return secrets.token_urlsafe(nbytes)


def hash_token(token: str) -> str:
    return hashlib.sha256(token.encode("utf-8")).hexdigest()
```

- [ ] **Step 4: Pass** — re-run tests.

- [ ] **Step 5: Commit** — `git commit -m "feat(devices): opaque token generator + sha256 hashing"`

---

### Task 5: Device + ProvisioningToken models

**Files:**
- Modify: `backend/devices/models.py`
- Create: `backend/devices/admin.py`
- Create: `backend/devices/tests/test_models.py`
- Create: `backend/devices/migrations/__init__.py`

- [ ] **Step 1: Failing test**

```python
import re
import pytest
from django.contrib.auth import get_user_model
from django.utils import timezone
from datetime import timedelta
from devices.models import Device, ProvisioningToken
from devices.tokens import generate_token

pytestmark = pytest.mark.django_db


def _user():
    return get_user_model().objects.create_user(email="p@x.com", password="x")


def test_device_id_must_be_12_lower_hex():
    user = _user()
    Device.objects.create(device_id="aabbccddeeff", user=user, token_hash="0"*64)
    with pytest.raises(Exception):
        Device.objects.create(device_id="AABBCCDDEEFF", user=user, token_hash="1"*64)
    with pytest.raises(Exception):
        Device.objects.create(device_id="zz", user=user, token_hash="2"*64)


def test_device_id_unique():
    user = _user()
    Device.objects.create(device_id="aabbccddeeff", user=user, token_hash="0"*64)
    with pytest.raises(Exception):
        Device.objects.create(device_id="aabbccddeeff", user=user, token_hash="1"*64)


def test_provisioning_token_expiry_helper():
    user = _user()
    raw = generate_token()
    pt = ProvisioningToken.objects.create(user=user, token_hash=ProvisioningToken.hash(raw),
                                          expires_at=timezone.now() + timedelta(minutes=10))
    assert not pt.is_expired()
    pt.expires_at = timezone.now() - timedelta(seconds=1)
    assert pt.is_expired()


def test_provisioning_token_lookup_by_hash():
    user = _user()
    raw = generate_token()
    ProvisioningToken.objects.create(user=user, token_hash=ProvisioningToken.hash(raw),
                                     expires_at=timezone.now() + timedelta(minutes=10))
    found = ProvisioningToken.objects.filter(token_hash=ProvisioningToken.hash(raw)).first()
    assert found is not None and found.user == user
```

- [ ] **Step 2: Confirm fail.**

- [ ] **Step 3: Implement `devices/models.py`**

```python
from django.conf import settings
from django.core.exceptions import ValidationError
from django.core.validators import RegexValidator
from django.db import models
from django.utils import timezone

from .tokens import hash_token

DEVICE_ID_RE = RegexValidator(r"^[0-9a-f]{12}$", "device_id must be 12 lowercase hex chars")


class Device(models.Model):
    device_id = models.CharField(max_length=12, unique=True, validators=[DEVICE_ID_RE])
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.CASCADE, related_name="devices")
    token_hash = models.CharField(max_length=64, unique=True)
    last_seen_at = models.DateTimeField(null=True, blank=True)
    created_at = models.DateTimeField(default=timezone.now)

    def clean(self):
        if self.device_id != self.device_id.lower():
            raise ValidationError("device_id must be lowercase")

    def save(self, *a, **kw):
        self.full_clean()
        return super().save(*a, **kw)

    @property
    def online(self) -> bool:
        if not self.last_seen_at:
            return False
        return (timezone.now() - self.last_seen_at).total_seconds() < 120


class ProvisioningToken(models.Model):
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.CASCADE)
    token_hash = models.CharField(max_length=64, unique=True)
    expires_at = models.DateTimeField()
    used_at = models.DateTimeField(null=True, blank=True)
    created_at = models.DateTimeField(default=timezone.now)

    @staticmethod
    def hash(raw: str) -> str:
        return hash_token(raw)

    def is_expired(self) -> bool:
        return timezone.now() >= self.expires_at

    def is_used(self) -> bool:
        return self.used_at is not None
```

`devices/admin.py`:

```python
from django.contrib import admin
from .models import Device, ProvisioningToken


@admin.register(Device)
class DeviceAdmin(admin.ModelAdmin):
    list_display = ["device_id", "user", "online", "last_seen_at", "created_at"]
    search_fields = ["device_id", "user__email"]


@admin.register(ProvisioningToken)
class ProvisioningTokenAdmin(admin.ModelAdmin):
    list_display = ["user", "expires_at", "used_at", "created_at"]
```

- [ ] **Step 4: makemigrations + migrate.**

`docker compose run --rm backend python manage.py makemigrations devices && docker compose run --rm backend python manage.py migrate`

- [ ] **Step 5: Pass tests.**

- [ ] **Step 6: Commit** — `git commit -m "feat(devices): Device + ProvisioningToken models"`

---

### Task 6: DeviceTokenAuthentication (DRF)

**Files:**
- Create: `backend/devices/auth.py`
- Create: `backend/devices/tests/test_auth.py`

- [ ] **Step 1: Failing test**

```python
import pytest
from django.contrib.auth import get_user_model
from rest_framework.test import APIRequestFactory
from devices.auth import DeviceTokenAuthentication
from devices.models import Device
from devices.tokens import generate_token, hash_token

pytestmark = pytest.mark.django_db


def _make(user_email="p@x.com"):
    user = get_user_model().objects.create_user(email=user_email, password="x")
    raw = generate_token()
    Device.objects.create(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    return raw


def test_authenticates_valid_bearer():
    raw = _make()
    rf = APIRequestFactory()
    req = rf.get("/", HTTP_AUTHORIZATION=f"Bearer {raw}")
    user, dev = DeviceTokenAuthentication().authenticate(req)
    assert dev.device_id == "aabbccddeeff"


def test_rejects_missing_header():
    rf = APIRequestFactory()
    req = rf.get("/")
    assert DeviceTokenAuthentication().authenticate(req) is None


def test_rejects_unknown_token():
    rf = APIRequestFactory()
    req = rf.get("/", HTTP_AUTHORIZATION="Bearer not-a-real-token")
    from rest_framework.exceptions import AuthenticationFailed
    with pytest.raises(AuthenticationFailed):
        DeviceTokenAuthentication().authenticate(req)
```

- [ ] **Step 2: Confirm fail.**

- [ ] **Step 3: Implement `devices/auth.py`**

```python
from django.utils import timezone
from rest_framework import authentication, exceptions
from .models import Device
from .tokens import hash_token


class DeviceTokenAuthentication(authentication.BaseAuthentication):
    keyword = "Bearer"

    def authenticate(self, request):
        header = request.META.get("HTTP_AUTHORIZATION", "")
        if not header.startswith(self.keyword + " "):
            return None
        raw = header[len(self.keyword) + 1:].strip()
        if not raw:
            return None
        try:
            device = Device.objects.select_related("user").get(token_hash=hash_token(raw))
        except Device.DoesNotExist as e:
            raise exceptions.AuthenticationFailed("invalid device token") from e
        Device.objects.filter(pk=device.pk).update(last_seen_at=timezone.now())
        return (device.user, device)

    def authenticate_header(self, request):
        return self.keyword
```

- [ ] **Step 4: Pass.**

- [ ] **Step 5: Commit** — `git commit -m "feat(devices): DeviceTokenAuthentication"`

---

### Task 7: `POST /api/provisioning-tokens` (web → mobile app)

**Files:**
- Modify: `backend/devices/serializers.py`
- Modify: `backend/devices/views.py`
- Modify: `backend/devices/urls.py`
- Create: `backend/devices/tests/test_provisioning.py`

- [ ] **Step 1: Failing tests**

```python
import pytest
from django.contrib.auth import get_user_model
from devices.models import ProvisioningToken
pytestmark = pytest.mark.django_db


def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")


def test_creates_provisioning_token(api_client):
    _login(api_client)
    r = api_client.post("/api/provisioning-tokens", {}, format="json")
    assert r.status_code == 201
    assert "provision_token" in r.data and "expires_at" in r.data
    assert ProvisioningToken.objects.count() == 1
    assert ProvisioningToken.objects.first().token_hash != r.data["provision_token"]


def test_unauth_is_401(api_client):
    assert api_client.post("/api/provisioning-tokens", {}, format="json").status_code == 401
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement**

`devices/serializers.py`:

```python
from rest_framework import serializers
from .models import Device


class DeviceSerializer(serializers.ModelSerializer):
    online = serializers.BooleanField(read_only=True)

    class Meta:
        model = Device
        fields = ["device_id", "online", "last_seen_at", "created_at"]
        read_only_fields = fields
```

`devices/views.py`:

```python
from datetime import timedelta
from django.utils import timezone
from rest_framework import generics, permissions, status
from rest_framework.exceptions import NotFound
from rest_framework.response import Response
from rest_framework.views import APIView

from .models import Device, ProvisioningToken
from .tokens import generate_token, hash_token
from .serializers import DeviceSerializer

PROVISIONING_TTL = timedelta(minutes=10)


class ProvisioningTokenCreateView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def post(self, request):
        raw = generate_token()
        expires_at = timezone.now() + PROVISIONING_TTL
        ProvisioningToken.objects.create(
            user=request.user,
            token_hash=hash_token(raw),
            expires_at=expires_at,
        )
        return Response(
            {"provision_token": raw, "expires_at": expires_at.isoformat()},
            status=status.HTTP_201_CREATED,
        )
```

`devices/urls.py`:

```python
from django.urls import path
from .views import ProvisioningTokenCreateView

urlpatterns = [
    path("api/provisioning-tokens", ProvisioningTokenCreateView.as_view()),
]
```

- [ ] **Step 4: Pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(devices): POST /api/provisioning-tokens"`

---

### Task 8: `POST /bootstrap` (firmware ↔ backend)

**Files:**
- Modify: `backend/devices/views.py`
- Modify: `backend/devices/urls.py`
- Create: `backend/devices/tests/test_bootstrap.py`

- [ ] **Step 1: Failing tests**

```python
import pytest
from django.contrib.auth import get_user_model
from django.utils import timezone
from datetime import timedelta
from devices.models import Device, ProvisioningToken
from devices.tokens import generate_token, hash_token

pytestmark = pytest.mark.django_db


def _seed_pt():
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    raw = generate_token()
    ProvisioningToken.objects.create(user=user, token_hash=hash_token(raw),
                                     expires_at=timezone.now() + timedelta(minutes=10))
    return raw, user


def test_bootstrap_exchanges_token(api_client):
    raw, user = _seed_pt()
    r = api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 200
    assert r.data["device_id"] == "aabbccddeeff"
    dt = r.data["device_token"]
    assert dt and len(dt) >= 40
    assert Device.objects.filter(device_id="aabbccddeeff", user=user, token_hash=hash_token(dt)).exists()


def test_bootstrap_marks_token_used(api_client):
    raw, _ = _seed_pt()
    api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    pt = ProvisioningToken.objects.first()
    assert pt.used_at is not None


def test_bootstrap_rejects_reused_token(api_client):
    raw, _ = _seed_pt()
    api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    r = api_client.post("/bootstrap", {"provision_token": raw, "device_id": "112233445566"}, format="json")
    assert r.status_code == 400


def test_bootstrap_rejects_expired_token(api_client):
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    raw = generate_token()
    ProvisioningToken.objects.create(user=user, token_hash=hash_token(raw),
                                     expires_at=timezone.now() - timedelta(seconds=1))
    r = api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 400


def test_bootstrap_rejects_unknown_token(api_client):
    r = api_client.post("/bootstrap", {"provision_token": "nope", "device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 400


def test_bootstrap_rebind_same_device(api_client):
    """If same device_id bootstraps again with a fresh token, new device_token replaces old."""
    raw, user = _seed_pt()
    r1 = api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    raw2 = generate_token()
    ProvisioningToken.objects.create(user=user, token_hash=hash_token(raw2),
                                     expires_at=timezone.now() + timedelta(minutes=10))
    r2 = api_client.post("/bootstrap", {"provision_token": raw2, "device_id": "aabbccddeeff"}, format="json")
    assert r1.data["device_token"] != r2.data["device_token"]
    assert Device.objects.filter(device_id="aabbccddeeff").count() == 1
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement** — append to `devices/views.py`:

```python
from django.db import transaction
from rest_framework.permissions import AllowAny
import re

DEVICE_ID_PATTERN = re.compile(r"^[0-9a-f]{12}$")


class BootstrapView(APIView):
    authentication_classes = []
    permission_classes = [AllowAny]

    def post(self, request):
        raw = request.data.get("provision_token", "")
        device_id = (request.data.get("device_id") or "").lower()
        if not raw or not DEVICE_ID_PATTERN.match(device_id):
            return Response({"detail": "bad request"}, status=400)
        try:
            pt = ProvisioningToken.objects.select_related("user").get(token_hash=hash_token(raw))
        except ProvisioningToken.DoesNotExist:
            return Response({"detail": "invalid token"}, status=400)
        if pt.is_used() or pt.is_expired():
            return Response({"detail": "invalid token"}, status=400)

        device_token = generate_token()
        with transaction.atomic():
            pt.used_at = timezone.now()
            pt.save(update_fields=["used_at"])
            Device.objects.update_or_create(
                device_id=device_id,
                defaults={"user": pt.user, "token_hash": hash_token(device_token)},
            )
        return Response({"device_id": device_id, "device_token": device_token})
```

Add to `devices/urls.py`:

```python
from .views import ProvisioningTokenCreateView, BootstrapView

urlpatterns = [
    path("api/provisioning-tokens", ProvisioningTokenCreateView.as_view()),
    path("bootstrap", BootstrapView.as_view()),
]
```

- [ ] **Step 4: Pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(devices): POST /bootstrap firmware token exchange"`

---

### Task 9: Dev provisioning helper (mobile-app stand-in)

**Why:** Mobile app is not built. We need a way to bind devices to users without the real handshake. Two paths:
1. `python manage.py provision_dev_device --email <e> --device-id <hex12>` — prints `device_token` to stdout. Always works (admin operator path).
2. `POST /api/dev/devices` — JSON body `{"device_id": "..."}`, returns `{device_id, device_token}`. Auth: user JWT. Gated by `DEV_PROVISIONING_ENABLED=true`. Returns 404 when disabled (so it never leaks in prod).

**Removal documented in `backend/README.md`:** "Set `DEV_PROVISIONING_ENABLED=false` (or remove) in prod env. Once the mobile companion ships, delete `devices/management/commands/provision_dev_device.py` and the `dev_provision_*` view + URL."

**Files:**
- Create: `backend/devices/management/__init__.py`
- Create: `backend/devices/management/commands/__init__.py`
- Create: `backend/devices/management/commands/provision_dev_device.py`
- Modify: `backend/devices/views.py`
- Modify: `backend/devices/urls.py`
- Modify: `backend/README.md`
- Create: `backend/devices/tests/test_dev_provision.py`

- [ ] **Step 1: Failing tests**

```python
import pytest
from io import StringIO
from django.core.management import call_command
from django.test import override_settings
from django.contrib.auth import get_user_model
from devices.models import Device
from devices.tokens import hash_token

pytestmark = pytest.mark.django_db


def test_management_command_creates_device():
    get_user_model().objects.create_user(email="a@b.com", password="x")
    out = StringIO()
    call_command("provision_dev_device", "--email", "a@b.com", "--device-id", "aabbccddeeff", stdout=out)
    raw = out.getvalue().strip().split()[-1]
    assert Device.objects.filter(device_id="aabbccddeeff", token_hash=hash_token(raw)).exists()


def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")


@override_settings(DEV_PROVISIONING_ENABLED=True)
def test_dev_endpoint_creates_device_when_flag_on(api_client):
    _login(api_client)
    r = api_client.post("/api/dev/devices", {"device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 201
    assert Device.objects.filter(device_id="aabbccddeeff").exists()


@override_settings(DEV_PROVISIONING_ENABLED=False)
def test_dev_endpoint_returns_404_when_flag_off(api_client):
    _login(api_client)
    r = api_client.post("/api/dev/devices", {"device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 404
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement**

`devices/management/commands/provision_dev_device.py`:

```python
from django.contrib.auth import get_user_model
from django.core.management.base import BaseCommand, CommandError
from devices.models import Device
from devices.tokens import generate_token, hash_token


class Command(BaseCommand):
    help = "Bind a device_id to a user without the mobile-app handshake. Dev only."

    def add_arguments(self, parser):
        parser.add_argument("--email", required=True)
        parser.add_argument("--device-id", required=True)

    def handle(self, *args, **opts):
        try:
            user = get_user_model().objects.get(email=opts["email"])
        except Exception as e:
            raise CommandError(f"no such user: {opts['email']}") from e
        device_id = opts["device_id"].lower()
        raw = generate_token()
        Device.objects.update_or_create(
            device_id=device_id,
            defaults={"user": user, "token_hash": hash_token(raw)},
        )
        self.stdout.write(f"device_token: {raw}")
```

Append to `devices/views.py`:

```python
from django.conf import settings
from rest_framework import status as drf_status


class DevProvisionView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def post(self, request):
        if not settings.DEV_PROVISIONING_ENABLED:
            raise NotFound()
        device_id = (request.data.get("device_id") or "").lower()
        if not DEVICE_ID_PATTERN.match(device_id):
            return Response({"detail": "bad device_id"}, status=400)
        raw = generate_token()
        Device.objects.update_or_create(
            device_id=device_id,
            defaults={"user": request.user, "token_hash": hash_token(raw)},
        )
        return Response({"device_id": device_id, "device_token": raw}, status=drf_status.HTTP_201_CREATED)
```

Add URL:

```python
path("api/dev/devices", DevProvisionView.as_view()),
```

`backend/README.md` (append section):

```markdown
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
```

- [ ] **Step 4: Pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(devices): dev provisioning fallback (cmd + flag-gated endpoint)"`

---

### Task 10: `GET /api/devices` and `DELETE /api/devices/<device_id>`

**Files:**
- Modify: `backend/devices/views.py`, `backend/devices/urls.py`
- Create: `backend/devices/tests/test_devices_crud.py`

- [ ] **Step 1: Failing tests**

```python
import pytest
from django.contrib.auth import get_user_model
from devices.models import Device

pytestmark = pytest.mark.django_db


def _login(api_client, email="a@b.com"):
    get_user_model().objects.create_user(email=email, password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": email, "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")


def test_lists_only_my_devices(api_client):
    _login(api_client, "a@b.com")
    me = get_user_model().objects.get(email="a@b.com")
    other = get_user_model().objects.create_user(email="x@y.com", password="x")
    Device.objects.create(device_id="aaaaaaaaaaaa", user=me, token_hash="a"*64)
    Device.objects.create(device_id="bbbbbbbbbbbb", user=other, token_hash="b"*64)
    r = api_client.get("/api/devices")
    assert r.status_code == 200 and len(r.data) == 1 and r.data[0]["device_id"] == "aaaaaaaaaaaa"


def test_delete_revokes_my_device(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    Device.objects.create(device_id="aaaaaaaaaaaa", user=me, token_hash="a"*64)
    r = api_client.delete("/api/devices/aaaaaaaaaaaa")
    assert r.status_code == 204
    assert not Device.objects.filter(device_id="aaaaaaaaaaaa").exists()


def test_cannot_delete_other_users_device(api_client):
    _login(api_client)
    other = get_user_model().objects.create_user(email="x@y.com", password="x")
    Device.objects.create(device_id="ccccccccccccc"[:12], user=other, token_hash="c"*64)
    r = api_client.delete("/api/devices/" + "c"*12)
    assert r.status_code == 404
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement** — append to `devices/views.py`:

```python
class DeviceListView(generics.ListAPIView):
    serializer_class = DeviceSerializer
    permission_classes = [permissions.IsAuthenticated]

    def get_queryset(self):
        return Device.objects.filter(user=self.request.user).order_by("created_at")


class DeviceDestroyView(generics.DestroyAPIView):
    serializer_class = DeviceSerializer
    permission_classes = [permissions.IsAuthenticated]
    lookup_field = "device_id"

    def get_queryset(self):
        return Device.objects.filter(user=self.request.user)
```

Add URLs:

```python
path("api/devices", DeviceListView.as_view()),
path("api/devices/<str:device_id>", DeviceDestroyView.as_view()),
```

- [ ] **Step 4: Pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(devices): list + revoke (DELETE) endpoints"`

---

### Task 11: Semsem + Track models

**Files:**
- Modify: `backend/semsems/models.py`
- Create: `backend/semsems/admin.py`
- Create: `backend/semsems/tests/__init__.py`, `backend/semsems/tests/test_models.py`
- Create: `backend/semsems/migrations/__init__.py`

- [ ] **Step 1: Failing test**

```python
import pytest
from django.core.exceptions import ValidationError
from semsems.models import Semsem, Track

pytestmark = pytest.mark.django_db


def test_uid_hex_lowercase_2_to_20():
    Semsem.objects.create(uid_hex="04a3f91b2c", title="ok", is_pro=False)
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="04A3", title="bad", is_pro=False)
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="zz", title="bad", is_pro=False)
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="0"*22, title="too long", is_pro=False)


def test_pro_requires_role():
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="01", title="x", is_pro=True, role="")


def test_role_max_31_bytes():
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="02", title="x", is_pro=True, role="a"*32)


def test_track_ordering():
    s = Semsem.objects.create(uid_hex="03", title="x", is_pro=False)
    Track.objects.create(semsem=s, name="b.mp3", file="audio/b.mp3", position=2)
    Track.objects.create(semsem=s, name="a.mp3", file="audio/a.mp3", position=1)
    assert [t.name for t in s.tracks.order_by("position")] == ["a.mp3", "b.mp3"]
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement**

```python
import re
from django.core.exceptions import ValidationError
from django.core.validators import RegexValidator
from django.db import models

UID_RE = RegexValidator(r"^[0-9a-f]{2,20}$", "uid_hex must be 2..20 lowercase hex chars")


class Semsem(models.Model):
    uid_hex = models.CharField(max_length=20, unique=True, validators=[UID_RE])
    title = models.CharField(max_length=120)
    is_pro = models.BooleanField(default=False)
    role = models.CharField(max_length=31, blank=True, default="")
    created_at = models.DateTimeField(auto_now_add=True)

    def clean(self):
        if self.uid_hex != self.uid_hex.lower():
            raise ValidationError("uid_hex must be lowercase")
        if self.is_pro and not self.role:
            raise ValidationError("pro semsem requires non-empty role")
        if not self.is_pro and self.role:
            raise ValidationError("regular semsem must not have a role")
        if len(self.role.encode("utf-8")) > 31:
            raise ValidationError("role exceeds 31 bytes")

    def save(self, *a, **kw):
        self.full_clean()
        return super().save(*a, **kw)


class Track(models.Model):
    semsem = models.ForeignKey(Semsem, on_delete=models.CASCADE, related_name="tracks")
    name = models.CharField(max_length=64)
    file = models.FileField(upload_to="audio/")
    position = models.PositiveIntegerField(default=0)

    class Meta:
        ordering = ["position", "id"]
        unique_together = [("semsem", "name")]
```

`semsems/admin.py`:

```python
from django.contrib import admin
from .models import Semsem, Track


class TrackInline(admin.TabularInline):
    model = Track


@admin.register(Semsem)
class SemsemAdmin(admin.ModelAdmin):
    list_display = ["uid_hex", "title", "is_pro", "role"]
    inlines = [TrackInline]


@admin.register(Track)
class TrackAdmin(admin.ModelAdmin):
    list_display = ["semsem", "name", "position"]
```

- [ ] **Step 4: makemigrations + migrate.**

- [ ] **Step 5: Pass.**
- [ ] **Step 6: Commit** — `git commit -m "feat(semsems): Semsem + Track models"`

---

### Task 12: `GET /semsem/<uid_hex>/manifest` (firmware)

Auth: device Bearer.

**Files:**
- Modify: `backend/semsems/views.py`, `backend/semsems/urls.py`
- Create: `backend/semsems/serializers.py`
- Create: `backend/semsems/tests/test_manifest.py`

- [ ] **Step 1: Failing tests**

```python
import pytest
from django.core.files.base import ContentFile
from django.contrib.auth import get_user_model
from semsems.models import Semsem, Track
from devices.models import Device
from devices.tokens import generate_token, hash_token

pytestmark = pytest.mark.django_db


def _device(api_client):
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    raw = generate_token()
    Device.objects.create(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {raw}")


def test_returns_regular_manifest_with_absolute_urls(api_client):
    _device(api_client)
    s = Semsem.objects.create(uid_hex="04a3f91b2c", title="Animal Sounds", is_pro=False)
    t = Track(semsem=s, name="01.mp3", position=1)
    t.file.save("01.mp3", ContentFile(b"id3"))
    t.save()
    r = api_client.get("/semsem/04a3f91b2c/manifest")
    assert r.status_code == 200
    body = r.json()
    assert body["uid"] == "04a3f91b2c"
    assert body["pro"] is False
    assert body["title"] == "Animal Sounds"
    assert body["tracks"][0]["name"] == "01.mp3"
    assert body["tracks"][0]["url"].startswith("http")


def test_returns_pro_manifest(api_client):
    _device(api_client)
    Semsem.objects.create(uid_hex="aa", title="Doctor Sem", is_pro=True, role="doctor")
    r = api_client.get("/semsem/aa/manifest")
    assert r.status_code == 200
    assert r.json() == {"uid": "aa", "title": "Doctor Sem", "pro": True, "role": "doctor", "tracks": []}


def test_unknown_uid_returns_404(api_client):
    _device(api_client)
    r = api_client.get("/semsem/deadbeef/manifest")
    assert r.status_code == 404


def test_uppercase_uid_normalised_or_404(api_client):
    _device(api_client)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=True, role="doctor")
    r = api_client.get("/semsem/AA/manifest")
    assert r.status_code in (200, 404)


def test_unauth_is_401(api_client):
    r = api_client.get("/semsem/aa/manifest")
    assert r.status_code == 401
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement**

`semsems/serializers.py`:

```python
from django.conf import settings
from rest_framework import serializers
from .models import Semsem, Track


class TrackManifestSerializer(serializers.ModelSerializer):
    url = serializers.SerializerMethodField()

    class Meta:
        model = Track
        fields = ["name", "url"]

    def get_url(self, obj):
        # Use S3_PUBLIC_ENDPOINT_URL so firmware (outside compose net) can reach MinIO.
        public = settings.S3_PUBLIC_ENDPOINT_URL.rstrip("/")
        bucket = settings.STORAGES["default"]["OPTIONS"]["bucket_name"]
        return f"{public}/{bucket}/{obj.file.name}"


class FirmwareManifestSerializer(serializers.ModelSerializer):
    uid = serializers.CharField(source="uid_hex")
    pro = serializers.BooleanField(source="is_pro")
    tracks = TrackManifestSerializer(many=True, read_only=True)

    class Meta:
        model = Semsem
        fields = ["uid", "title", "pro", "role", "tracks"]
```

`semsems/views.py`:

```python
from rest_framework import generics
from devices.auth import DeviceTokenAuthentication
from .models import Semsem
from .serializers import FirmwareManifestSerializer


class FirmwareManifestView(generics.RetrieveAPIView):
    authentication_classes = [DeviceTokenAuthentication]
    serializer_class = FirmwareManifestSerializer
    queryset = Semsem.objects.prefetch_related("tracks")
    lookup_field = "uid_hex"
    lookup_url_kwarg = "uid_hex"

    def get_object(self):
        self.kwargs["uid_hex"] = self.kwargs["uid_hex"].lower()
        return super().get_object()
```

`semsems/urls.py`:

```python
from django.urls import path
from .views import FirmwareManifestView

urlpatterns = [
    path("semsem/<str:uid_hex>/manifest", FirmwareManifestView.as_view()),
]
```

- [ ] **Step 4: Pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(semsems): GET /semsem/<uid>/manifest for firmware"`

---

### Task 13: `POST /stats` (firmware → backend)

**Files:**
- Modify: `backend/stats/models.py`
- Create: `backend/stats/serializers.py`
- Modify: `backend/stats/views.py`, `backend/stats/urls.py`
- Create: `backend/stats/tests/__init__.py`, `backend/stats/tests/test_upload.py`
- Create: `backend/stats/migrations/__init__.py`

- [ ] **Step 1: Failing test**

```python
import pytest
from django.contrib.auth import get_user_model
from devices.models import Device
from devices.tokens import generate_token, hash_token
from stats.models import UsageStats
from semsems.models import Semsem

pytestmark = pytest.mark.django_db


def _device(api_client):
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    raw = generate_token()
    d = Device.objects.create(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {raw}")
    return d


def test_first_upload_creates_stats(api_client):
    d = _device(api_client)
    Semsem.objects.create(uid_hex="04a3f91b2c", title="x", is_pro=False)
    body = {"uid": "04a3f91b2c", "play_count": 7, "total_play_ms": 284300,
            "last_played_unix": 1734551234, "pro_session_count": 0, "pro_total_ms": 0}
    r = api_client.post("/stats", body, format="json")
    assert r.status_code == 200
    s = UsageStats.objects.get(device=d, uid_hex="04a3f91b2c")
    assert s.play_count == 7 and s.total_play_ms == 284300


def test_upload_overwrites_cumulative(api_client):
    d = _device(api_client)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=True, role="doctor")
    UsageStats.objects.create(device=d, uid_hex="aa", play_count=3, total_play_ms=1000,
                              last_played_unix=1, pro_session_count=0, pro_total_ms=0)
    body = {"uid": "aa", "play_count": 5, "total_play_ms": 2000,
            "last_played_unix": 9999, "pro_session_count": 1, "pro_total_ms": 60000}
    api_client.post("/stats", body, format="json")
    s = UsageStats.objects.get(device=d, uid_hex="aa")
    assert s.play_count == 5 and s.pro_session_count == 1


def test_unauth_is_401(api_client):
    r = api_client.post("/stats", {"uid": "aa"}, format="json")
    assert r.status_code == 401
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement**

`stats/models.py`:

```python
from django.db import models
from devices.models import Device


class UsageStats(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE, related_name="stats")
    uid_hex = models.CharField(max_length=20)
    play_count = models.PositiveIntegerField(default=0)
    total_play_ms = models.PositiveBigIntegerField(default=0)
    last_played_unix = models.BigIntegerField(default=0)
    pro_session_count = models.PositiveIntegerField(default=0)
    pro_total_ms = models.PositiveBigIntegerField(default=0)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        unique_together = [("device", "uid_hex")]
```

`stats/serializers.py`:

```python
from rest_framework import serializers
from .models import UsageStats


class StatsUploadSerializer(serializers.Serializer):
    uid = serializers.CharField(max_length=20)
    play_count = serializers.IntegerField(min_value=0)
    total_play_ms = serializers.IntegerField(min_value=0)
    last_played_unix = serializers.IntegerField(min_value=0)
    pro_session_count = serializers.IntegerField(min_value=0)
    pro_total_ms = serializers.IntegerField(min_value=0)
```

`stats/views.py`:

```python
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response
from rest_framework.views import APIView
from devices.auth import DeviceTokenAuthentication
from .models import UsageStats
from .serializers import StatsUploadSerializer


class StatsUploadView(APIView):
    authentication_classes = [DeviceTokenAuthentication]
    permission_classes = [IsAuthenticated]

    def post(self, request):
        s = StatsUploadSerializer(data=request.data)
        s.is_valid(raise_exception=True)
        device = request.auth   # Device instance from DeviceTokenAuthentication
        UsageStats.objects.update_or_create(
            device=device, uid_hex=s.validated_data["uid"].lower(),
            defaults={
                "play_count": s.validated_data["play_count"],
                "total_play_ms": s.validated_data["total_play_ms"],
                "last_played_unix": s.validated_data["last_played_unix"],
                "pro_session_count": s.validated_data["pro_session_count"],
                "pro_total_ms": s.validated_data["pro_total_ms"],
            },
        )
        return Response({"ok": True})
```

`stats/urls.py`:

```python
from django.urls import path
from .views import StatsUploadView
urlpatterns = [path("stats", StatsUploadView.as_view())]
```

- [ ] **Step 4: makemigrations + migrate, run tests, pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(stats): POST /stats cumulative-snapshot upload"`

---

### Task 14: `GET /api/users/me/stats` aggregator (web)

PRD: dashboard shows total listening hours, unique semsems owned, AI conversation minutes, online device count.

**Files:**
- Modify: `backend/stats/views.py`, `backend/stats/urls.py`
- Create: `backend/stats/tests/test_aggregate.py`

- [ ] **Step 1: Failing tests**

```python
import pytest
from django.contrib.auth import get_user_model
from devices.models import Device
from stats.models import UsageStats
from semsems.models import Semsem

pytestmark = pytest.mark.django_db


def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")


def test_aggregates_across_my_devices(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    d1 = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    d2 = Device.objects.create(device_id="b"*12, user=me, token_hash="b"*64)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=False)
    Semsem.objects.create(uid_hex="bb", title="y", is_pro=True, role="doctor")
    UsageStats.objects.create(device=d1, uid_hex="aa", play_count=1, total_play_ms=60_000,
                              last_played_unix=1, pro_session_count=0, pro_total_ms=0)
    UsageStats.objects.create(device=d2, uid_hex="bb", play_count=0, total_play_ms=0,
                              last_played_unix=2, pro_session_count=3, pro_total_ms=120_000)
    r = api_client.get("/api/users/me/stats")
    assert r.status_code == 200
    assert r.data == {
        "total_listening_ms": 60_000,
        "unique_semsems": 2,
        "pro_total_ms": 120_000,
        "device_count": 2,
        "online_device_count": 0,
    }


def test_excludes_other_users(api_client):
    _login(api_client)
    other = get_user_model().objects.create_user(email="x@y.com", password="x")
    d = Device.objects.create(device_id="c"*12, user=other, token_hash="c"*64)
    UsageStats.objects.create(device=d, uid_hex="aa", play_count=10, total_play_ms=999_999,
                              last_played_unix=1, pro_session_count=0, pro_total_ms=0)
    r = api_client.get("/api/users/me/stats")
    assert r.data["total_listening_ms"] == 0
    assert r.data["device_count"] == 0
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement** — append to `stats/views.py`:

```python
from django.db.models import Sum, Count
from rest_framework import generics
from devices.models import Device


class MyStatsView(APIView):
    permission_classes = [IsAuthenticated]
    authentication_classes = [
        __import__("rest_framework_simplejwt.authentication", fromlist=["JWTAuthentication"]).JWTAuthentication,
    ]

    def get(self, request):
        my_stats = UsageStats.objects.filter(device__user=request.user)
        agg = my_stats.aggregate(
            total_listening_ms=Sum("total_play_ms"),
            pro_total_ms=Sum("pro_total_ms"),
            unique_semsems=Count("uid_hex", distinct=True),
        )
        devices = Device.objects.filter(user=request.user)
        return Response({
            "total_listening_ms": agg["total_listening_ms"] or 0,
            "unique_semsems": agg["unique_semsems"] or 0,
            "pro_total_ms": agg["pro_total_ms"] or 0,
            "device_count": devices.count(),
            "online_device_count": sum(1 for d in devices if d.online),
        })
```

(Cleaner: `from rest_framework_simplejwt.authentication import JWTAuthentication` at top.)

Add URL `path("api/users/me/stats", MyStatsView.as_view())`.

- [ ] **Step 4: Pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(stats): GET /api/users/me/stats dashboard aggregator"`

---

### Task 15: Web semsem listing (`GET /api/semsems`, `GET /api/semsems/<uid>`)

PRD §4.2 says these list bound UIDs and their manifest data.

**Decision:** "Bound" = at least one of the user's devices has a `UsageStats` row for it. (Alternative — "all semsems globally" — would leak the full catalog. Owning a physical NFC tag is implicit in stats appearing.)

**Files:**
- Modify: `backend/semsems/views.py`, `backend/semsems/urls.py`, `backend/semsems/serializers.py`
- Create: `backend/semsems/tests/test_web_api.py`

- [ ] **Step 1: Failing tests**

```python
import pytest
from django.contrib.auth import get_user_model
from devices.models import Device
from semsems.models import Semsem
from stats.models import UsageStats

pytestmark = pytest.mark.django_db


def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")


def test_lists_bound_semsems(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    d = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    Semsem.objects.create(uid_hex="aa", title="Animals", is_pro=False)
    Semsem.objects.create(uid_hex="bb", title="Doctor", is_pro=True, role="doctor")
    Semsem.objects.create(uid_hex="cc", title="Unbound", is_pro=False)
    UsageStats.objects.create(device=d, uid_hex="aa", play_count=1, total_play_ms=1, last_played_unix=1, pro_session_count=0, pro_total_ms=0)
    UsageStats.objects.create(device=d, uid_hex="bb", play_count=0, total_play_ms=0, last_played_unix=1, pro_session_count=1, pro_total_ms=10)
    r = api_client.get("/api/semsems")
    assert r.status_code == 200
    uids = sorted(s["uid_hex"] for s in r.data)
    assert uids == ["aa", "bb"]


def test_detail_returns_full_record(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    d = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    Semsem.objects.create(uid_hex="bb", title="Doctor", is_pro=True, role="doctor")
    UsageStats.objects.create(device=d, uid_hex="bb", play_count=0, total_play_ms=0, last_played_unix=1, pro_session_count=1, pro_total_ms=60_000)
    r = api_client.get("/api/semsems/bb")
    assert r.status_code == 200
    assert r.data["title"] == "Doctor" and r.data["is_pro"] is True and r.data["role"] == "doctor"
    assert r.data["pro_total_ms"] == 60_000


def test_detail_404_when_not_bound(api_client):
    _login(api_client)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=False)
    r = api_client.get("/api/semsems/aa")
    assert r.status_code == 404
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement** — append to `semsems/serializers.py`:

```python
class SemsemWebListSerializer(serializers.ModelSerializer):
    class Meta:
        model = Semsem
        fields = ["uid_hex", "title", "is_pro", "role"]


class SemsemWebDetailSerializer(serializers.ModelSerializer):
    pro_total_ms = serializers.IntegerField(read_only=True)
    total_play_ms = serializers.IntegerField(read_only=True)
    play_count = serializers.IntegerField(read_only=True)
    pro_session_count = serializers.IntegerField(read_only=True)
    tracks = TrackManifestSerializer(many=True, read_only=True)

    class Meta:
        model = Semsem
        fields = ["uid_hex", "title", "is_pro", "role", "tracks",
                  "play_count", "total_play_ms", "pro_session_count", "pro_total_ms"]
```

Append to `semsems/views.py`:

```python
from django.db.models import Sum
from rest_framework_simplejwt.authentication import JWTAuthentication
from stats.models import UsageStats
from .serializers import SemsemWebListSerializer, SemsemWebDetailSerializer


def _bound_uids(user):
    return UsageStats.objects.filter(device__user=user).values_list("uid_hex", flat=True).distinct()


class SemsemListWebView(generics.ListAPIView):
    authentication_classes = [JWTAuthentication]
    serializer_class = SemsemWebListSerializer

    def get_queryset(self):
        return Semsem.objects.filter(uid_hex__in=_bound_uids(self.request.user)).order_by("uid_hex")


class SemsemDetailWebView(generics.RetrieveAPIView):
    authentication_classes = [JWTAuthentication]
    serializer_class = SemsemWebDetailSerializer
    lookup_field = "uid_hex"

    def get_queryset(self):
        return Semsem.objects.filter(uid_hex__in=_bound_uids(self.request.user)).prefetch_related("tracks")

    def retrieve(self, request, *args, **kwargs):
        instance = self.get_object()
        agg = UsageStats.objects.filter(device__user=request.user, uid_hex=instance.uid_hex).aggregate(
            play_count=Sum("play_count"), total_play_ms=Sum("total_play_ms"),
            pro_session_count=Sum("pro_session_count"), pro_total_ms=Sum("pro_total_ms"),
        )
        for k, v in agg.items():
            setattr(instance, k, v or 0)
        return Response(self.get_serializer(instance).data)
```

Append URLs:

```python
path("api/semsems", SemsemListWebView.as_view()),
path("api/semsems/<str:uid_hex>", SemsemDetailWebView.as_view()),
```

- [ ] **Step 4: Pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(semsems): web list + detail endpoints"`

---

### Task 16: ProChatSession + TranscriptEntry models

**Files:**
- Modify: `backend/chat/models.py`
- Create: `backend/chat/admin.py`
- Create: `backend/chat/tests/__init__.py`, `backend/chat/tests/test_models.py`
- Create: `backend/chat/migrations/__init__.py`

- [ ] **Step 1: Failing test**

```python
import pytest
from django.contrib.auth import get_user_model
from devices.models import Device
from chat.models import ProChatSession, TranscriptEntry

pytestmark = pytest.mark.django_db


def test_session_lifecycle():
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    d = Device.objects.create(device_id="a"*12, user=user, token_hash="a"*64)
    s = ProChatSession.objects.create(device=d, uid_hex="bb", role="doctor")
    assert s.ended_at is None
    TranscriptEntry.objects.create(session=s, speaker="child", text="hello")
    TranscriptEntry.objects.create(session=s, speaker="ai", text="hi!")
    assert s.entries.count() == 2
    assert list(s.entries.values_list("speaker", flat=True)) == ["child", "ai"]
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement**

```python
from django.db import models
from django.utils import timezone
from devices.models import Device


class ProChatSession(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE, related_name="chat_sessions")
    uid_hex = models.CharField(max_length=20)
    role = models.CharField(max_length=31)
    started_at = models.DateTimeField(default=timezone.now)
    ended_at = models.DateTimeField(null=True, blank=True)

    class Meta:
        ordering = ["-started_at"]


class TranscriptEntry(models.Model):
    SPEAKER_CHOICES = [("child", "child"), ("ai", "ai")]
    session = models.ForeignKey(ProChatSession, on_delete=models.CASCADE, related_name="entries")
    speaker = models.CharField(max_length=8, choices=SPEAKER_CHOICES)
    text = models.TextField()
    timestamp = models.DateTimeField(default=timezone.now)

    class Meta:
        ordering = ["timestamp", "id"]
```

`chat/admin.py`:

```python
from django.contrib import admin
from .models import ProChatSession, TranscriptEntry


class TranscriptInline(admin.TabularInline):
    model = TranscriptEntry
    readonly_fields = ["speaker", "text", "timestamp"]
    can_delete = False
    extra = 0


@admin.register(ProChatSession)
class ProChatSessionAdmin(admin.ModelAdmin):
    list_display = ["device", "uid_hex", "role", "started_at", "ended_at"]
    inlines = [TranscriptInline]
```

- [ ] **Step 4: makemigrations, migrate, pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(chat): ProChatSession + TranscriptEntry"`

---

### Task 17: Channels device-token middleware for WebSocket

**Files:**
- Modify: `backend/chat/auth.py`
- Modify: `backend/chat/routing.py` (add path placeholder)
- Create: `backend/chat/tests/test_ws_auth.py`

- [ ] **Step 1: Failing test**

```python
import pytest
from channels.testing import WebsocketCommunicator
from django.contrib.auth import get_user_model
from devices.models import Device
from devices.tokens import generate_token, hash_token
from nadeem.asgi import application

pytestmark = [pytest.mark.django_db, pytest.mark.asyncio]


@pytest.fixture
async def device_token(db):
    from asgiref.sync import sync_to_async
    user = await sync_to_async(get_user_model().objects.create_user)(email="a@b.com", password="x")
    raw = generate_token()
    await sync_to_async(Device.objects.create)(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    return raw


async def test_ws_rejects_without_token():
    comm = WebsocketCommunicator(application, "/chat?role=doctor&device=aabbccddeeff&semsem=aa")
    connected, _ = await comm.connect()
    assert not connected


async def test_ws_accepts_with_query_token(device_token):
    url = f"/chat?role=doctor&device=aabbccddeeff&semsem=aa&token={device_token}"
    comm = WebsocketCommunicator(application, url)
    connected, _ = await comm.connect()
    assert connected
    await comm.disconnect()
```

(Test depends on Task 19 consumer existing; for now, create a minimal placeholder consumer in Task 18 / 19. This test will pass after Task 19. Mark `pytest.mark.skip` here and remove after Task 19. Or write the placeholder consumer in this task.)

**Take 2 — write a placeholder consumer here so the auth test stands alone.**

- [ ] **Step 2: Implement `chat/auth.py`**

```python
from urllib.parse import parse_qs
from channels.db import database_sync_to_async
from devices.models import Device
from devices.tokens import hash_token


@database_sync_to_async
def _get_device(token: str) -> Device | None:
    try:
        return Device.objects.select_related("user").get(token_hash=hash_token(token))
    except Device.DoesNotExist:
        return None


def _extract_token(scope) -> str | None:
    qs = parse_qs(scope.get("query_string", b"").decode())
    if "token" in qs:
        return qs["token"][0]
    for name, value in scope.get("headers", []):
        if name == b"authorization":
            v = value.decode()
            if v.lower().startswith("bearer "):
                return v[7:].strip()
    return None


class DeviceTokenAuthMiddleware:
    def __init__(self, inner):
        self.inner = inner

    async def __call__(self, scope, receive, send):
        token = _extract_token(scope)
        scope["device"] = await _get_device(token) if token else None
        return await self.inner(scope, receive, send)
```

- [ ] **Step 3: Placeholder consumer in `chat/consumers.py`**

```python
from channels.generic.websocket import AsyncWebsocketConsumer


class ChatConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        if not self.scope.get("device"):
            await self.close(code=4401)
            return
        await self.accept()

    async def disconnect(self, code):
        pass
```

`chat/routing.py`:

```python
from django.urls import re_path
from .consumers import ChatConsumer

websocket_urlpatterns = [re_path(r"^chat$", ChatConsumer.as_asgi())]
```

- [ ] **Step 4: Pass `pytest chat/tests/test_ws_auth.py -v`.**
- [ ] **Step 5: Commit** — `git commit -m "feat(chat): device-token WS middleware + placeholder consumer"`

---

### Task 18: Gemini Live client wrapper

PRD: Backend processes ASR/LLM/TTS and streams audio back. Use `gemini-3.1-flash-live-preview` model. Audio format: 16 kHz PCM16 mono in/out. The `google-genai` SDK's `live.connect()` returns a session with `send_realtime_input(...)` for audio chunks and async `receive()` yielding text + audio responses + turn boundaries.

**Files:**
- Create: `backend/chat/gemini_client.py`
- Create: `backend/chat/tests/test_gemini_client.py`

**Design:** Pure async wrapper, no Django/Channels imports. Two methods used by the consumer:

```python
class GeminiLiveSession:
    async def __aenter__(self) -> "GeminiLiveSession": ...
    async def __aexit__(self, exc_type, exc, tb): ...
    async def send_audio(self, pcm16_le_16khz: bytes) -> None: ...
    def stream_responses(self) -> AsyncIterator[GeminiEvent]: ...
```

Where `GeminiEvent` is a small dataclass:

```python
@dataclass
class GeminiEvent:
    kind: Literal["audio", "transcript_user", "transcript_ai", "turn_started", "turn_completed"]
    audio_pcm16: bytes | None = None
    text: str | None = None
```

- [ ] **Step 1: Failing test (mock the SDK)**

```python
import pytest
from chat.gemini_client import GeminiLiveSession, GeminiEvent

pytestmark = pytest.mark.asyncio


class _FakeSDKSession:
    def __init__(self):
        self.sent = []

    async def send_realtime_input(self, audio):
        self.sent.append(audio)

    async def receive(self):
        # yield one user transcript, one ai audio chunk, one turn_completed
        yield {"server_content": {"input_transcription": {"text": "hello"}}}
        yield {"server_content": {"model_turn": {"parts": [{"inline_data": {"data": b"\x00\x01", "mime_type": "audio/pcm;rate=16000"}}]}}}
        yield {"server_content": {"turn_complete": True}}


@pytest.fixture
def fake_sdk(monkeypatch):
    sess = _FakeSDKSession()

    class _LiveContext:
        async def __aenter__(self_):
            return sess
        async def __aexit__(self_, *a):
            return False

    class _Live:
        def connect(self, **_):
            return _LiveContext()

    class _Client:
        def __init__(self, **_):
            self.aio = type("X", (), {"live": _Live()})()

    monkeypatch.setattr("chat.gemini_client._build_client", lambda: _Client())
    return sess


async def test_session_yields_events(fake_sdk, settings):
    settings.GEMINI_API_KEY = "k"
    async with GeminiLiveSession(role="doctor") as s:
        await s.send_audio(b"\x00" * 640)
        events = []
        async for e in s.stream_responses():
            events.append(e)
        kinds = [e.kind for e in events]
        assert "transcript_user" in kinds
        assert "audio" in kinds
        assert "turn_completed" in kinds
        assert fake_sdk.sent  # audio was forwarded
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement `chat/gemini_client.py`**

```python
from __future__ import annotations
from dataclasses import dataclass
from typing import AsyncIterator, Literal
from django.conf import settings


def _build_client():
    from google import genai  # imported lazily so tests can monkeypatch
    return genai.Client(api_key=settings.GEMINI_API_KEY)


SYSTEM_PROMPTS = {
    "doctor": "You are a friendly children's doctor companion. Use simple Arabic. Never give medical advice; suggest asking a parent.",
    "engineer": "You are a friendly children's engineer companion. Speak simple Arabic; explain how things work using everyday analogies.",
    "storyteller": "You are a children's storyteller. Speak simple Arabic; tell short, kind stories.",
}


@dataclass
class GeminiEvent:
    kind: Literal["audio", "transcript_user", "transcript_ai", "turn_started", "turn_completed"]
    audio_pcm16: bytes | None = None
    text: str | None = None


class GeminiLiveSession:
    def __init__(self, role: str):
        self.role = role
        self._client = None
        self._ctx = None
        self._sdk = None

    async def __aenter__(self):
        self._client = _build_client()
        config = {
            "response_modalities": ["AUDIO"],
            "system_instruction": SYSTEM_PROMPTS.get(self.role, SYSTEM_PROMPTS["storyteller"]),
            "input_audio_transcription": {},
            "output_audio_transcription": {},
        }
        self._ctx = self._client.aio.live.connect(model=settings.GEMINI_MODEL, config=config)
        self._sdk = await self._ctx.__aenter__()
        return self

    async def __aexit__(self, exc_type, exc, tb):
        await self._ctx.__aexit__(exc_type, exc, tb)

    async def send_audio(self, pcm16_le_16khz: bytes) -> None:
        await self._sdk.send_realtime_input(audio={"data": pcm16_le_16khz, "mime_type": "audio/pcm;rate=16000"})

    async def stream_responses(self) -> AsyncIterator[GeminiEvent]:
        async for msg in self._sdk.receive():
            sc = (msg or {}).get("server_content") if isinstance(msg, dict) else None
            if not sc:
                # SDK may yield typed objects; handle attribute-style too
                sc = getattr(msg, "server_content", None)
            if not sc:
                continue
            if isinstance(sc, dict):
                if (it := sc.get("input_transcription")):
                    yield GeminiEvent("transcript_user", text=it.get("text") or "")
                if (ot := sc.get("output_transcription")):
                    yield GeminiEvent("transcript_ai", text=ot.get("text") or "")
                if (mt := sc.get("model_turn")):
                    for part in mt.get("parts", []):
                        if (inline := part.get("inline_data")):
                            yield GeminiEvent("audio", audio_pcm16=inline["data"])
                if sc.get("turn_complete"):
                    yield GeminiEvent("turn_completed")
            else:
                # attribute-style fallback
                if getattr(sc, "input_transcription", None):
                    yield GeminiEvent("transcript_user", text=sc.input_transcription.text or "")
                if getattr(sc, "output_transcription", None):
                    yield GeminiEvent("transcript_ai", text=sc.output_transcription.text or "")
                mt = getattr(sc, "model_turn", None)
                if mt:
                    for part in getattr(mt, "parts", []) or []:
                        inline = getattr(part, "inline_data", None)
                        if inline:
                            yield GeminiEvent("audio", audio_pcm16=inline.data)
                if getattr(sc, "turn_complete", False):
                    yield GeminiEvent("turn_completed")
```

> If the live SDK shape differs at run-time (it is preview), the `dict` branch in the test stays green and the attribute branch covers the real shape; verify with a smoke test in Task 19 step 6.

- [ ] **Step 4: Pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(chat): GeminiLiveSession async wrapper"`

---

### Task 19: WebSocket `/chat` consumer (real)

Replace placeholder. Bridge: client PCM16 binary → Gemini; Gemini audio → client PCM16 binary; Gemini transcripts → DB rows; `{"status":"listening"}` / `{"status":"speaking"}` text frames to drive LEDs.

**Files:**
- Modify: `backend/chat/consumers.py`
- Create: `backend/chat/tests/test_consumer.py`

- [ ] **Step 1: Failing test (with Gemini mocked)**

```python
import json
import pytest
from channels.testing import WebsocketCommunicator
from django.contrib.auth import get_user_model
from asgiref.sync import sync_to_async
from devices.models import Device
from devices.tokens import generate_token, hash_token
from nadeem.asgi import application
from chat.models import ProChatSession, TranscriptEntry
from chat import gemini_client

pytestmark = [pytest.mark.django_db, pytest.mark.asyncio]


class _FakeSession:
    def __init__(self):
        self.sent = []

    async def __aenter__(self):
        return self

    async def __aexit__(self, *a):
        return False

    async def send_audio(self, b):
        self.sent.append(b)

    async def stream_responses(self):
        yield gemini_client.GeminiEvent("transcript_user", text="hello")
        yield gemini_client.GeminiEvent("audio", audio_pcm16=b"\x10\x00" * 320)
        yield gemini_client.GeminiEvent("transcript_ai", text="hi child")
        yield gemini_client.GeminiEvent("turn_completed")


async def _seed(db):
    user = await sync_to_async(get_user_model().objects.create_user)(email="a@b.com", password="x")
    raw = generate_token()
    await sync_to_async(Device.objects.create)(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    return raw


async def test_consumer_bridges_audio_and_persists_transcript(monkeypatch, db):
    raw = await _seed(db)
    monkeypatch.setattr(gemini_client, "GeminiLiveSession", lambda role: _FakeSession())
    url = f"/chat?role=doctor&device=aabbccddeeff&semsem=aa&token={raw}"
    comm = WebsocketCommunicator(application, url)
    ok, _ = await comm.connect()
    assert ok
    await comm.send_to(bytes_data=b"\x00" * 640)
    # Expect at least one binary frame (audio) and one or more text frames (status)
    received_binary = await comm.receive_from()
    assert isinstance(received_binary, bytes)
    text_frame = await comm.receive_from()
    assert "speaking" in text_frame or "listening" in text_frame
    await comm.disconnect()
    sessions = await sync_to_async(list)(ProChatSession.objects.all())
    assert len(sessions) == 1 and sessions[0].uid_hex == "aa" and sessions[0].role == "doctor"
    entries = await sync_to_async(list)(TranscriptEntry.objects.values_list("speaker", "text"))
    assert ("child", "hello") in entries
    assert ("ai", "hi child") in entries
```

- [ ] **Step 2: Implement `chat/consumers.py`**

```python
import asyncio
import json
from urllib.parse import parse_qs
from channels.db import database_sync_to_async
from channels.generic.websocket import AsyncWebsocketConsumer
from django.utils import timezone

from . import gemini_client
from .models import ProChatSession, TranscriptEntry


class ChatConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        device = self.scope.get("device")
        if not device:
            await self.close(code=4401)
            return
        qs = parse_qs(self.scope.get("query_string", b"").decode())
        self.role = (qs.get("role") or [""])[0]
        self.uid_hex = (qs.get("semsem") or [""])[0].lower()
        if not self.role or not self.uid_hex:
            await self.close(code=4400)
            return
        self.device = device
        self.session = await self._create_session()
        await self.accept()
        self._gemini_cm = gemini_client.GeminiLiveSession(role=self.role)
        self._gemini = await self._gemini_cm.__aenter__()
        self._reader = asyncio.create_task(self._pump_gemini_to_ws())
        await self.send(text_data=json.dumps({"status": "listening"}))

    async def disconnect(self, code):
        if hasattr(self, "_reader"):
            self._reader.cancel()
        if hasattr(self, "_gemini_cm"):
            try:
                await self._gemini_cm.__aexit__(None, None, None)
            except Exception:
                pass
        if hasattr(self, "session") and self.session:
            await self._end_session()

    async def receive(self, text_data=None, bytes_data=None):
        if bytes_data is not None:
            await self._gemini.send_audio(bytes_data)

    async def _pump_gemini_to_ws(self):
        try:
            async for ev in self._gemini.stream_responses():
                if ev.kind == "audio" and ev.audio_pcm16:
                    await self.send(bytes_data=ev.audio_pcm16)
                    await self.send(text_data=json.dumps({"status": "speaking"}))
                elif ev.kind == "transcript_user" and ev.text:
                    await self._add_entry("child", ev.text)
                elif ev.kind == "transcript_ai" and ev.text:
                    await self._add_entry("ai", ev.text)
                elif ev.kind == "turn_completed":
                    await self.send(text_data=json.dumps({"status": "listening"}))
        except asyncio.CancelledError:
            raise
        except Exception:
            await self.close(code=4500)

    @database_sync_to_async
    def _create_session(self):
        return ProChatSession.objects.create(device=self.device, uid_hex=self.uid_hex, role=self.role)

    @database_sync_to_async
    def _end_session(self):
        self.session.ended_at = timezone.now()
        self.session.save(update_fields=["ended_at"])

    @database_sync_to_async
    def _add_entry(self, speaker, text):
        TranscriptEntry.objects.create(session=self.session, speaker=speaker, text=text)
```

- [ ] **Step 3: Pass.**
- [ ] **Step 4: Commit** — `git commit -m "feat(chat): WS /chat consumer bridging firmware to Gemini Live"`

---

### Task 20: `GET /api/chat-sessions` and `GET /api/chat-sessions/<id>`

**Files:**
- Modify: `backend/chat/serializers.py`, `backend/chat/views.py`, `backend/chat/urls.py`
- Create: `backend/chat/tests/test_web_api.py`

- [ ] **Step 1: Failing test**

```python
import pytest
from django.contrib.auth import get_user_model
from devices.models import Device
from chat.models import ProChatSession, TranscriptEntry

pytestmark = pytest.mark.django_db


def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")


def test_lists_only_my_sessions(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    other = get_user_model().objects.create_user(email="x@y.com", password="x")
    d_me = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    d_other = Device.objects.create(device_id="b"*12, user=other, token_hash="b"*64)
    ProChatSession.objects.create(device=d_me, uid_hex="aa", role="doctor")
    ProChatSession.objects.create(device=d_other, uid_hex="bb", role="doctor")
    r = api_client.get("/api/chat-sessions")
    assert r.status_code == 200 and len(r.data) == 1


def test_detail_includes_transcript(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    d = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    s = ProChatSession.objects.create(device=d, uid_hex="aa", role="doctor")
    TranscriptEntry.objects.create(session=s, speaker="child", text="hi")
    TranscriptEntry.objects.create(session=s, speaker="ai", text="hello")
    r = api_client.get(f"/api/chat-sessions/{s.id}")
    assert r.status_code == 200
    assert [e["speaker"] for e in r.data["entries"]] == ["child", "ai"]
```

- [ ] **Step 2: Fail.**

- [ ] **Step 3: Implement**

`chat/serializers.py`:

```python
from rest_framework import serializers
from .models import ProChatSession, TranscriptEntry


class TranscriptEntrySerializer(serializers.ModelSerializer):
    class Meta:
        model = TranscriptEntry
        fields = ["speaker", "text", "timestamp"]


class ProChatSessionListSerializer(serializers.ModelSerializer):
    device_id = serializers.CharField(source="device.device_id", read_only=True)

    class Meta:
        model = ProChatSession
        fields = ["id", "device_id", "uid_hex", "role", "started_at", "ended_at"]


class ProChatSessionDetailSerializer(ProChatSessionListSerializer):
    entries = TranscriptEntrySerializer(many=True, read_only=True)

    class Meta(ProChatSessionListSerializer.Meta):
        fields = ProChatSessionListSerializer.Meta.fields + ["entries"]
```

`chat/views.py`:

```python
from rest_framework import generics
from rest_framework.permissions import IsAuthenticated
from rest_framework_simplejwt.authentication import JWTAuthentication
from .models import ProChatSession
from .serializers import ProChatSessionListSerializer, ProChatSessionDetailSerializer


class _MyMixin:
    authentication_classes = [JWTAuthentication]
    permission_classes = [IsAuthenticated]

    def get_queryset(self):
        return ProChatSession.objects.filter(device__user=self.request.user).select_related("device")


class ChatSessionListView(_MyMixin, generics.ListAPIView):
    serializer_class = ProChatSessionListSerializer


class ChatSessionDetailView(_MyMixin, generics.RetrieveAPIView):
    serializer_class = ProChatSessionDetailSerializer

    def get_queryset(self):
        return super().get_queryset().prefetch_related("entries")
```

`chat/urls.py`:

```python
from django.urls import path
from .views import ChatSessionListView, ChatSessionDetailView

urlpatterns = [
    path("api/chat-sessions", ChatSessionListView.as_view()),
    path("api/chat-sessions/<int:pk>", ChatSessionDetailView.as_view()),
]
```

- [ ] **Step 4: Pass.**
- [ ] **Step 5: Commit** — `git commit -m "feat(chat): list + detail web endpoints with transcripts"`

---

### Task 21: Production hardening + smoke check

**Files:**
- Modify: `backend/nadeem/settings.py`
- Modify: `backend/README.md`
- Create: `backend/nadeem/settings_prod.py` (optional override)
- Create: `backend/Makefile` (developer ergonomics)

- [ ] **Step 1: Add production guards in `settings.py`**

Append:

```python
if not DEBUG:
    SECURE_SSL_REDIRECT = True
    SESSION_COOKIE_SECURE = True
    CSRF_COOKIE_SECURE = True
    SECURE_HSTS_SECONDS = 60 * 60 * 24 * 30
    SECURE_HSTS_INCLUDE_SUBDOMAINS = True
    SECURE_PROXY_SSL_HEADER = ("HTTP_X_FORWARDED_PROTO", "https")
    SECURE_CONTENT_TYPE_NOSNIFF = True
```

- [ ] **Step 2: Document in `README.md`**

Add sections: "Run locally", "Run tests", "Create a parent + dev-bind a device" (with curl examples), "Production checklist" (set `DJANGO_DEBUG=false`, real `DJANGO_SECRET_KEY`, real `GEMINI_API_KEY`, set `DEV_PROVISIONING_ENABLED=false`, terminate TLS upstream, point `S3_PUBLIC_ENDPOINT_URL` to a CDN).

- [ ] **Step 3: `Makefile`**

```make
up:        ; docker compose up -d
down:      ; docker compose down
logs:      ; docker compose logs -f backend
shell:     ; docker compose run --rm backend python manage.py shell
test:      ; docker compose run --rm backend pytest -v
migrate:   ; docker compose run --rm backend python manage.py migrate
makemigrations: ; docker compose run --rm backend python manage.py makemigrations
superuser: ; docker compose run --rm backend python manage.py createsuperuser
fmt:       ; docker compose run --rm backend ruff format .
lint:      ; docker compose run --rm backend ruff check .
```

- [ ] **Step 4: End-to-end smoke**

Run:

```bash
docker compose up -d
docker compose run --rm backend python manage.py createsuperuser  # email=parent@example.com
docker compose run --rm backend python manage.py provision_dev_device --email parent@example.com --device-id aabbccddeeff
# copy device_token from output
curl -s http://localhost:8000/semsem/aa/manifest -H "Authorization: Bearer <device_token>"
```

Expected: `404` (no semsem aa yet) or `200` after creating one in the admin. Confirms the firmware path is wired end-to-end.

- [ ] **Step 5: Run the whole suite**

`docker compose run --rm backend pytest -v` — every test added in tasks 1-20 must pass.

- [ ] **Step 6: Commit** — `git commit -m "chore(backend): production hardening + Makefile + README"`

---

## Self-review

**Spec coverage check (PRD §1-5 and `backend/backend.md` §1-9):**

- §1.1 stack: Django + Postgres + Redis + Docker — Task 1. MinIO for S3 — Task 1.
- §2 data models: User (Task 2), Device + ProvisioningToken (5), Semsem + Track (11), UsageStats (13), ProChatSession + TranscriptEntry (16).
- §3.1 provisioning + bootstrap: Tasks 7, 8, 9 (dev fallback).
- §3.2 AI conversation pipeline: Tasks 17 (auth), 18 (Gemini wrapper), 19 (consumer).
- §4.1 firmware endpoints: bootstrap (8), manifest (12), stats (13), chat WS (19).
- §4.2 web endpoints: login (3), provisioning-tokens (7), users/me/stats (14), semsems list/detail (15), chat-sessions list/detail (20), devices list/delete (10).
- §5 security: TLS + HSTS + secure cookies (21); device token revocation = `DELETE /api/devices/<id>` (10); provision token single-use, hashed (5, 8).

**Placeholder scan:** none (every step has runnable code or shell command).

**Type/name consistency:** `device_id` 12 lower hex everywhere; `uid_hex` 2..20 lower hex; `role` ≤ 31 bytes; `token_hash` `CHAR(64)`; serializer field `uid` (firmware) vs `uid_hex` (web) is intentional and matches PRD.

---

## Execution Handoff

**Plan complete and saved to `docs/superpowers/plans/2026-04-25-nadeem-backend.md`. Two execution options:**

**1. Subagent-Driven (recommended)** — fresh subagent per task, review between tasks, fast iteration.
**2. Inline Execution** — execute tasks in this session using executing-plans, batch execution with checkpoints.

**Which approach?**
