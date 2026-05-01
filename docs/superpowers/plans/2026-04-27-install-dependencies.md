# Nadeem Admin Platform - Task 1: Install dependencies Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Configure dependencies and scaffold the admin interface using `django-unfold` and `django-admin-sortable2`, and stub out the `firmware` app.

**Architecture:** We are integrating `django-unfold` as the primary admin skin, which requires specific ordering in `INSTALLED_APPS` and a custom configuration object. We are also adding a `firmware` app stub to allow the admin configuration to reference it immediately.

**Tech Stack:** Django, django-unfold, django-admin-sortable2, pip

---

### Task 1.1: Dependency Update

**Files:**
- Modify: `backend/pyproject.toml`

- [ ] **Step 1: Update dependencies in pyproject.toml**

In `backend/pyproject.toml`, replace the `dependencies` list (lines 9-22) with:

```toml
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
  "django-unfold>=0.40",
  "django-admin-sortable2>=2.1",
]
```

- [ ] **Step 2: Update tool.setuptools.packages.find include**

Add `firmware*` to the `include` list in `backend/pyproject.toml` (line 34):

```toml
include = ["nadeem*", "accounts*", "devices*", "semsems*", "stats*", "chat*", "firmware*"]
```

- [ ] **Step 3: Install new packages**

Run: `cd backend && pip install "django-unfold>=0.40" "django-admin-sortable2>=2.1"`
Expected: Success

- [ ] **Step 4: Commit**

```bash
git add backend/pyproject.toml
git commit -m "chore(deps): add django-unfold and django-admin-sortable2"
```

---

### Task 1.2: Settings Configuration

**Files:**
- Modify: `backend/nadeem/settings.py`

- [ ] **Step 1: Update INSTALLED_APPS**

Replace `INSTALLED_APPS` (lines 12-28) in `backend/nadeem/settings.py` to include `unfold` (before admin), `adminsortable2`, and the new `firmware` app:

```python
INSTALLED_APPS = [
    "unfold",
    "unfold.contrib.filters",
    "unfold.contrib.forms",
    "adminsortable2",
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
    "firmware",
]
```

- [ ] **Step 2: Update TEMPLATES**

Replace the `TEMPLATES` setting (lines 41-49) in `backend/nadeem/settings.py`:

```python
TEMPLATES = [{
    "BACKEND": "django.template.backends.django.DjangoTemplates",
    "DIRS": [BASE_DIR / "nadeem" / "templates"],
    "APP_DIRS": True,
    "OPTIONS": {"context_processors": [
        "django.template.context_processors.request",
        "django.contrib.auth.context_processors.auth",
        "django.contrib.messages.context_processors.messages",
    ]},
}]
```

- [ ] **Step 3: Add UNFOLD config**

Append the `UNFOLD` dictionary to the bottom of `backend/nadeem/settings.py` (before the `if not DEBUG:` block at line 124):

```python
UNFOLD = {
    "SITE_TITLE": "Nadeem Admin",
    "SITE_HEADER": "نديم — لوحة الإدارة",
    "SITE_URL": "/",
    "DASHBOARD_CALLBACK": "nadeem.dashboard.dashboard_callback",
    "SIDEBAR": {
        "navigation": [
            {
                "title": "Content",
                "separator": True,
                "items": [
                    {"title": "Semsems", "icon": "toys", "link": "/admin/semsems/semsem/"},
                    {"title": "Semsem Groups", "icon": "folder", "link": "/admin/semsems/semsemgroup/"},
                    {"title": "Tracks", "icon": "music_note", "link": "/admin/semsems/track/"},
                ],
            },
            {
                "title": "Users & Devices",
                "separator": True,
                "items": [
                    {"title": "Users", "icon": "person", "link": "/admin/accounts/user/"},
                    {"title": "Devices", "icon": "devices", "link": "/admin/devices/device/"},
                    {"title": "Provisioning Tokens", "icon": "key", "link": "/admin/devices/provisioningtoken/"},
                ],
            },
            {
                "title": "AI & Chat",
                "separator": True,
                "items": [
                    {"title": "Chat Sessions", "icon": "chat", "link": "/admin/chat/prochatsession/"},
                ],
            },
            {
                "title": "Analytics",
                "separator": True,
                "items": [
                    {"title": "Usage Stats", "icon": "bar_chart", "link": "/admin/stats/usagestats/"},
                    {"title": "Stats Explorer", "icon": "analytics", "link": "/admin/stats-explorer/"},
                ],
            },
            {
                "title": "Firmware",
                "separator": True,
                "items": [
                    {"title": "Releases", "icon": "system_update", "link": "/admin/firmware/firmwarerelease/"},
                    {"title": "Releases Group", "icon": "group_work", "link": "/admin/firmware/releasegroup/"},
                ],
            },
        ]
    },
}
```

- [ ] **Step 4: Commit**

```bash
git add backend/nadeem/settings.py
git commit -m "config(admin): configure django-unfold and templates"
```

---

### Task 1.3: Scaffolding and Verification

**Files:**
- Create: `backend/nadeem/dashboard.py`
- Create: `backend/firmware/__init__.py`
- Create: `backend/firmware/apps.py`
- Create: `backend/firmware/models.py`
- Create: `backend/firmware/migrations/__init__.py`

- [ ] **Step 1: Create dashboard.py**

Create `backend/nadeem/dashboard.py`:

```python
def dashboard_callback(request, context):
    return context
```

- [ ] **Step 2: Scaffold firmware app**

Create `backend/firmware/__init__.py` (empty).

Create `backend/firmware/apps.py`:

```python
from django.apps import AppConfig

class FirmwareConfig(AppConfig):
    default_auto_field = "django.db.models.BigAutoField"
    name = "firmware"
```

Create `backend/firmware/models.py`:

```python
from django.db import models
```

Create `backend/firmware/migrations/__init__.py` (empty).

- [ ] **Step 3: Run Django check**

Run: `cd backend && python manage.py check`
Expected: `System check identified no issues (0 silenced).`

- [ ] **Step 4: Commit**

```bash
git add backend/nadeem/dashboard.py backend/firmware/
git commit -m "chore(firmware): scaffold firmware app stub"
```
