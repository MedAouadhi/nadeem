# Nadeem Admin Platform Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Build a full business operations platform on top of Django admin — covering semsem catalog management (with groups), user/device fleet ops, AI chat review with flagging, platform analytics dashboard, stats explorer, and a firmware OTA release management skeleton.

**Architecture:** Extend Django admin with `django-unfold` (Tailwind-based modern skin), add new domain models (`SemsemGroup`, `FirmwareRelease`, `ReleaseGroup`), enhance all existing admin classes with computed fields / inline views / bulk actions, add two custom admin views (Platform Dashboard replacing the default index, Stats Explorer), and scaffold a `firmware` Django app with a device-facing OTA check endpoint.

**Tech Stack:** Django 5, django-unfold ≥0.40, django-admin-sortable2 ≥2.1, Chart.js (CDN), PostgreSQL, MinIO/S3 (firmware file storage).

---

## File Map

**New files:**
- `backend/nadeem/admin_site.py` — `NadeemAdminSite` (subclasses `UnfoldAdminSite`), registers stats-explorer URL
- `backend/nadeem/dashboard.py` — dashboard callback (platform KPIs, top semsems)
- `backend/nadeem/templates/admin/index.html` — custom dashboard template
- `backend/nadeem/templates/admin/stats_explorer.html` — stats explorer template
- `backend/firmware/__init__.py`
- `backend/firmware/apps.py`
- `backend/firmware/models.py` — `FirmwareRelease`, `ReleaseGroup`
- `backend/firmware/admin.py`
- `backend/firmware/views.py` — OTA check endpoint
- `backend/firmware/urls.py`
- `backend/firmware/migrations/0001_initial.py`
- `backend/firmware/tests/__init__.py`
- `backend/firmware/tests/test_models.py`
- `backend/firmware/tests/test_views.py`
- `backend/accounts/tests/test_admin.py`
- `backend/devices/tests/test_admin.py`
- `backend/semsems/tests/test_admin.py`
- `backend/chat/tests/test_admin.py`
- `backend/firmware/tests/test_admin.py`

**Modified files:**
- `backend/pyproject.toml` — add unfold, sortable2
- `backend/nadeem/settings.py` — INSTALLED_APPS, TEMPLATES dirs, UNFOLD config
- `backend/nadeem/urls.py` — swap `admin.site` → `admin_site`, add firmware URLs
- `backend/semsems/models.py` — add `SemsemGroup`, M2M on `Semsem`
- `backend/semsems/admin.py` — groups filter, track reorder, audio preview
- `backend/chat/models.py` — add `flagged`, `flag_reason` to `ProChatSession`
- `backend/chat/admin.py` — flagging actions, transcript view, duration
- `backend/devices/models.py` — add `release_group` FK to `ReleaseGroup`
- `backend/devices/admin.py` — online badge, stats inline, token cleanup action
- `backend/accounts/admin.py` — device count + listen time computed columns
- `backend/stats/admin.py` — full registered admin with listen time display

---

## Task 1: Install dependencies

**Files:**
- Modify: `backend/pyproject.toml`
- Modify: `backend/nadeem/settings.py`

- [ ] **Step 1: Add dependencies to pyproject.toml**

In `backend/pyproject.toml`, replace the `dependencies` list:

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

- [ ] **Step 2: Install new packages**

```bash
cd backend && pip install "django-unfold>=0.40" "django-admin-sortable2>=2.1"
```

Expected: both install without error.

- [ ] **Step 3: Update INSTALLED_APPS in settings.py**

`unfold` must appear **before** `django.contrib.admin`. Replace `INSTALLED_APPS` in `backend/nadeem/settings.py`:

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

- [ ] **Step 4: Update TEMPLATES to include project-level template dir**

Replace the `TEMPLATES` setting in `backend/nadeem/settings.py`:

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

- [ ] **Step 5: Add UNFOLD config to settings.py**

Append to the bottom of `backend/nadeem/settings.py` (before the SSL block):

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
                    {"title": "Release Groups", "icon": "group_work", "link": "/admin/firmware/releasegroup/"},
                ],
            },
        ]
    },
}
```

- [ ] **Step 6: Stub out dashboard.py and firmware app so manage.py check passes**

Create `backend/nadeem/dashboard.py`:

```python
def dashboard_callback(request, context):
    return context
```

Create `backend/firmware/__init__.py` (empty file).

Create `backend/firmware/apps.py`:

```python
from django.apps import AppConfig


class FirmwareConfig(AppConfig):
    default_auto_field = "django.db.models.BigAutoField"
    name = "firmware"
```

Create `backend/firmware/models.py` (stub — full content added in Task 5):

```python
from django.db import models
```

Create `backend/firmware/migrations/__init__.py` (empty).

- [ ] **Step 7: Run Django check**

```bash
cd backend && python manage.py check
```

Expected: `System check identified no issues (0 silenced).`

- [ ] **Step 8: Commit**

```bash
git add backend/pyproject.toml backend/nadeem/settings.py backend/nadeem/dashboard.py \
        backend/firmware/__init__.py backend/firmware/apps.py backend/firmware/models.py \
        backend/firmware/migrations/__init__.py
git commit -m "chore(admin): add django-unfold and adminsortable2; scaffold firmware app"
```

---

## Task 2: Custom admin site

**Files:**
- Create: `backend/nadeem/admin_site.py`
- Modify: `backend/nadeem/urls.py`

- [ ] **Step 1: Create NadeemAdminSite**

Create `backend/nadeem/admin_site.py`:

```python
from django.template.response import TemplateResponse
from django.urls import path
from unfold.sites import UnfoldAdminSite


class NadeemAdminSite(UnfoldAdminSite):
    site_header = "نديم"
    site_title = "Nadeem Admin"
    index_title = "Management Platform"

    def get_urls(self):
        urls = super().get_urls()
        custom = [
            path(
                "stats-explorer/",
                self.admin_view(self.stats_explorer_view),
                name="stats_explorer",
            ),
        ]
        return custom + urls

    def stats_explorer_view(self, request):
        from django.db.models import Count, Sum

        from accounts.models import User
        from semsems.models import Semsem
        from stats.models import UsageStats

        semsem_stats = (
            UsageStats.objects.values("uid_hex")
            .annotate(
                total_plays=Sum("play_count"),
                total_listen_ms=Sum("total_play_ms"),
                total_pro_sessions=Sum("pro_session_count"),
                device_count=Count("device", distinct=True),
            )
            .order_by("-total_plays")
        )
        uid_to_title = {
            s.uid_hex: s.title
            for s in Semsem.objects.filter(uid_hex__in=[s["uid_hex"] for s in semsem_stats])
        }
        semsem_rows = [
            {
                "uid_hex": s["uid_hex"],
                "title": uid_to_title.get(s["uid_hex"], s["uid_hex"]),
                "plays": s["total_plays"] or 0,
                "listen_hours": round((s["total_listen_ms"] or 0) / 3_600_000, 2),
                "pro_sessions": s["total_pro_sessions"] or 0,
                "devices": s["device_count"] or 0,
            }
            for s in semsem_stats
        ]

        user_stats = User.objects.annotate(
            device_count=Count("devices", distinct=True),
            total_play_ms=Sum("devices__stats__total_play_ms"),
            total_pro=Sum("devices__stats__pro_session_count"),
        ).order_by("-total_play_ms")

        context = {
            **self.each_context(request),
            "title": "Stats Explorer",
            "semsem_rows": semsem_rows,
            "user_stats": user_stats,
        }
        return TemplateResponse(request, "admin/stats_explorer.html", context)


admin_site = NadeemAdminSite(name="admin")
```

- [ ] **Step 2: Swap admin site in urls.py**

Replace `backend/nadeem/urls.py` entirely:

```python
from django.urls import include, path

from nadeem.admin_site import admin_site

urlpatterns = [
    path("admin/", admin_site.urls),
    path("", include("devices.urls")),
    path("", include("semsems.urls")),
    path("", include("stats.urls")),
    path("", include("chat.urls")),
    path("api/auth/", include("accounts.urls")),
    path("firmware/", include("firmware.urls")),
]
```

- [ ] **Step 3: Create stub firmware/urls.py so the include doesn't break**

Create `backend/firmware/urls.py`:

```python
from django.urls import path

urlpatterns: list = []
```

- [ ] **Step 4: Run Django check**

```bash
cd backend && python manage.py check
```

Expected: no issues.

- [ ] **Step 5: Verify admin login page loads**

```bash
cd backend && python manage.py runserver 8001 &
sleep 2 && curl -s -o /dev/null -w "%{http_code}" http://localhost:8001/admin/login/
```

Expected: `200`.

```bash
kill %1
```

- [ ] **Step 6: Commit**

```bash
git add backend/nadeem/admin_site.py backend/nadeem/urls.py backend/firmware/urls.py
git commit -m "feat(admin): add NadeemAdminSite with unfold skin and stats-explorer URL"
```

---

## Task 3: SemsemGroup model

**Files:**
- Modify: `backend/semsems/models.py`
- Create: `backend/semsems/tests/test_admin.py` (partial — groups model tests)

- [ ] **Step 1: Write failing tests**

Create `backend/semsems/tests/test_admin.py`:

```python
import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from semsems.models import Semsem, SemsemGroup

User = get_user_model()


@pytest.fixture
def superuser(db):
    return User.objects.create_superuser(email="admin@nadeem.com", password="adminpass")


@pytest.fixture
def admin_client(superuser):
    c = Client()
    c.force_login(superuser)
    return c


@pytest.mark.django_db
def test_semsem_group_creation():
    group = SemsemGroup.objects.create(name="Animals", description="Animal characters", icon="🐻")
    assert group.name == "Animals"
    assert group.icon == "🐻"
    assert str(group) == "Animals"


@pytest.mark.django_db
def test_semsem_belongs_to_group():
    group = SemsemGroup.objects.create(name="Professions", icon="👩‍⚕️")
    semsem = Semsem.objects.create(
        uid_hex="04a3f91b2c", title="Dr. Salim", is_pro=True, role="doctor"
    )
    semsem.groups.add(group)
    assert group in semsem.groups.all()
    assert semsem in group.semsems.all()


@pytest.mark.django_db
def test_semsem_can_have_multiple_groups():
    g1 = SemsemGroup.objects.create(name="Animals", icon="🐻")
    g2 = SemsemGroup.objects.create(name="Favorites", icon="⭐")
    semsem = Semsem.objects.create(uid_hex="04a3f91b2d", title="Polar Bear", is_pro=False)
    semsem.groups.add(g1, g2)
    assert semsem.groups.count() == 2


@pytest.mark.django_db
def test_semsem_group_list_admin(admin_client):
    response = admin_client.get("/admin/semsems/semsemgroup/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_semsem_list_admin(admin_client):
    response = admin_client.get("/admin/semsems/semsem/")
    assert response.status_code == 200
```

- [ ] **Step 2: Run to verify failure**

```bash
cd backend && pytest semsems/tests/test_admin.py -v 2>&1 | head -30
```

Expected: `ImportError: cannot import name 'SemsemGroup' from 'semsems.models'`

- [ ] **Step 3: Add SemsemGroup and M2M to semsems/models.py**

Open `backend/semsems/models.py`. Add `SemsemGroup` **before** `Semsem`:

```python
class SemsemGroup(models.Model):
    name = models.CharField(max_length=64, unique=True)
    description = models.TextField(blank=True, default="")
    icon = models.CharField(max_length=8, blank=True, default="")

    class Meta:
        ordering = ["name"]

    def __str__(self) -> str:
        return self.name
```

In the `Semsem` class, add the `groups` field after the `created_at` field:

```python
    groups = models.ManyToManyField(
        SemsemGroup,
        blank=True,
        related_name="semsems",
    )
```

- [ ] **Step 4: Create and apply migration**

```bash
cd backend && python manage.py makemigrations semsems && python manage.py migrate
```

Expected: migration created + applied, no errors.

- [ ] **Step 5: Run model tests**

```bash
cd backend && pytest semsems/tests/test_admin.py -k "group_creation or belongs_to or multiple_groups" -v
```

Expected: 3 tests PASS.

- [ ] **Step 6: Commit**

```bash
git add backend/semsems/models.py backend/semsems/migrations/ backend/semsems/tests/test_admin.py
git commit -m "feat(semsems): add SemsemGroup model with M2M to Semsem"
```

---

## Task 4: ProChatSession flagging fields

**Files:**
- Modify: `backend/chat/models.py`
- Create: `backend/chat/tests/test_admin.py`

- [ ] **Step 1: Write failing tests**

Create `backend/chat/tests/test_admin.py`:

```python
import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from chat.models import ProChatSession
from devices.models import Device

User = get_user_model()


@pytest.fixture
def superuser(db):
    return User.objects.create_superuser(email="admin@nadeem.com", password="adminpass")


@pytest.fixture
def admin_client(superuser):
    c = Client()
    c.force_login(superuser)
    return c


@pytest.fixture
def session(superuser):
    device = Device.objects.create(
        device_id="aabbccddeef3", user=superuser, token_hash="c" * 64
    )
    return ProChatSession.objects.create(device=device, uid_hex="04a3f91b30", role="doctor")


@pytest.mark.django_db
def test_chat_session_flagging_defaults(session):
    assert session.flagged is False
    assert session.flag_reason == ""


@pytest.mark.django_db
def test_chat_session_can_be_flagged(session):
    session.flagged = True
    session.flag_reason = "Inappropriate content"
    session.save()
    session.refresh_from_db()
    assert session.flagged is True
    assert session.flag_reason == "Inappropriate content"


@pytest.mark.django_db
def test_chat_session_list_admin(admin_client):
    response = admin_client.get("/admin/chat/prochatsession/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_flag_session_action(admin_client, session):
    response = admin_client.post(
        "/admin/chat/prochatsession/",
        {"action": "flag_sessions", "_selected_action": [session.pk]},
    )
    assert response.status_code in (200, 302)
    session.refresh_from_db()
    assert session.flagged is True
```

- [ ] **Step 2: Run to verify failure**

```bash
cd backend && pytest chat/tests/test_admin.py -k "flagging_defaults" -v 2>&1 | head -20
```

Expected: `AttributeError: 'ProChatSession' object has no attribute 'flagged'`

- [ ] **Step 3: Add fields to ProChatSession**

In `backend/chat/models.py`, inside the `ProChatSession` class, add after `ended_at`:

```python
    flagged = models.BooleanField(default=False)
    flag_reason = models.CharField(max_length=256, blank=True, default="")
```

- [ ] **Step 4: Create and apply migration**

```bash
cd backend && python manage.py makemigrations chat && python manage.py migrate
```

Expected: migration created + applied.

- [ ] **Step 5: Run model tests**

```bash
cd backend && pytest chat/tests/test_admin.py -k "flagging" -v
```

Expected: 2 tests PASS.

- [ ] **Step 6: Commit**

```bash
git add backend/chat/models.py backend/chat/migrations/ backend/chat/tests/test_admin.py
git commit -m "feat(chat): add flagged and flag_reason fields to ProChatSession"
```

---

## Task 5: Firmware app models

**Files:**
- Modify: `backend/firmware/models.py`
- Create: `backend/firmware/migrations/0001_initial.py` (via makemigrations)
- Modify: `backend/devices/models.py`
- Create: `backend/firmware/tests/__init__.py`
- Create: `backend/firmware/tests/test_models.py`

- [ ] **Step 1: Write failing tests**

Create `backend/firmware/tests/__init__.py` (empty).

Create `backend/firmware/tests/test_models.py`:

```python
import pytest
from django.contrib.auth import get_user_model

from devices.models import Device
from firmware.models import FirmwareRelease, ReleaseGroup

User = get_user_model()


@pytest.mark.django_db
def test_firmware_release_creation():
    release = FirmwareRelease.objects.create(
        version="1.0.0",
        changelog="Initial release",
        is_stable=True,
    )
    assert release.version == "1.0.0"
    assert release.is_stable is True
    assert str(release) == "v1.0.0"


@pytest.mark.django_db
def test_release_group_creation():
    release = FirmwareRelease.objects.create(version="1.0.0", is_stable=True)
    group = ReleaseGroup.objects.create(
        name="Beta Testers",
        description="Early adopter devices",
        assigned_release=release,
    )
    assert group.name == "Beta Testers"
    assert group.assigned_release == release
    assert str(group) == "Beta Testers"


@pytest.mark.django_db
def test_release_group_without_release():
    group = ReleaseGroup.objects.create(name="Unassigned")
    assert group.assigned_release is None


@pytest.mark.django_db
def test_device_can_be_assigned_to_release_group():
    user = User.objects.create_user(email="fw@test.com", password="pass")
    device = Device.objects.create(
        device_id="aabbccddeef1", user=user, token_hash="z" * 64
    )
    release = FirmwareRelease.objects.create(version="1.0.0", is_stable=True)
    group = ReleaseGroup.objects.create(name="Stable", assigned_release=release)
    device.release_group = group
    device.save()
    device.refresh_from_db()
    assert device.release_group == group
    assert device in group.devices.all()
```

- [ ] **Step 2: Run to verify failure**

```bash
cd backend && pytest firmware/tests/test_models.py -v 2>&1 | head -20
```

Expected: `ImportError: cannot import name 'FirmwareRelease' from 'firmware.models'`

- [ ] **Step 3: Implement firmware/models.py**

Replace `backend/firmware/models.py`:

```python
from django.db import models


class FirmwareRelease(models.Model):
    version = models.CharField(max_length=32, unique=True)
    file = models.FileField(upload_to="firmware/", blank=True, null=True)
    changelog = models.TextField(blank=True, default="")
    is_stable = models.BooleanField(default=False)
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        ordering = ["-created_at"]

    def __str__(self) -> str:
        return f"v{self.version}"


class ReleaseGroup(models.Model):
    name = models.CharField(max_length=64, unique=True)
    description = models.TextField(blank=True, default="")
    assigned_release = models.ForeignKey(
        FirmwareRelease,
        on_delete=models.SET_NULL,
        null=True,
        blank=True,
        related_name="release_groups",
    )
    created_at = models.DateTimeField(auto_now_add=True)

    class Meta:
        ordering = ["name"]

    def __str__(self) -> str:
        return self.name
```

- [ ] **Step 4: Add release_group FK to Device**

In `backend/devices/models.py`, add to the `Device` class after `created_at`:

```python
    release_group = models.ForeignKey(
        "firmware.ReleaseGroup",
        on_delete=models.SET_NULL,
        null=True,
        blank=True,
        related_name="devices",
    )
```

- [ ] **Step 5: Create and apply migrations**

```bash
cd backend && python manage.py makemigrations firmware devices && python manage.py migrate
```

Expected: `firmware/migrations/0001_initial.py` and `devices/migrations/000X_device_release_group.py` created + applied.

- [ ] **Step 6: Run tests**

```bash
cd backend && pytest firmware/tests/test_models.py -v
```

Expected: 4 tests PASS.

- [ ] **Step 7: Commit**

```bash
git add backend/firmware/models.py backend/firmware/migrations/ \
        backend/devices/models.py backend/devices/migrations/ \
        backend/firmware/tests/
git commit -m "feat(firmware): add FirmwareRelease and ReleaseGroup; add release_group FK to Device"
```

---

## Task 6: Enhanced User Admin

**Files:**
- Modify: `backend/accounts/admin.py`
- Create: `backend/accounts/tests/test_admin.py`

- [ ] **Step 1: Write failing tests**

Create `backend/accounts/tests/test_admin.py`:

```python
import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from devices.models import Device

User = get_user_model()


@pytest.fixture
def superuser(db):
    return User.objects.create_superuser(email="admin@nadeem.com", password="adminpass")


@pytest.fixture
def admin_client(superuser):
    c = Client()
    c.force_login(superuser)
    return c


@pytest.mark.django_db
def test_user_list_loads(admin_client):
    response = admin_client.get("/admin/accounts/user/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_user_list_shows_device_count_column(admin_client, superuser):
    Device.objects.create(device_id="aabbccddeef2", user=superuser, token_hash="a" * 64)
    response = admin_client.get("/admin/accounts/user/")
    assert response.status_code == 200
    # column header present
    assert b"Devices" in response.content or b"device_count" in response.content


@pytest.mark.django_db
def test_user_admin_search_by_email(admin_client, superuser):
    response = admin_client.get("/admin/accounts/user/?q=admin%40nadeem.com")
    assert response.status_code == 200
    assert b"admin@nadeem.com" in response.content
```

- [ ] **Step 2: Run to verify failure**

```bash
cd backend && pytest accounts/tests/test_admin.py -v 2>&1 | head -20
```

Expected: 404 or model not registered on custom admin site.

- [ ] **Step 3: Rewrite accounts/admin.py**

```python
from django.contrib import admin
from django.contrib.auth.admin import UserAdmin as BaseUserAdmin
from django.db.models import Count, Sum
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import User


@admin_site.register(User)
class UserAdmin(BaseUserAdmin, ModelAdmin):
    ordering = ["email"]
    list_display = ["email", "device_count", "total_listen_hours", "is_staff", "date_joined"]
    search_fields = ["email"]
    list_filter = ["is_staff", "is_active"]
    fieldsets = (
        (None, {"fields": ("email", "password")}),
        (
            "Permissions",
            {
                "fields": (
                    "is_active",
                    "is_staff",
                    "is_superuser",
                    "groups",
                    "user_permissions",
                )
            },
        ),
        ("Dates", {"fields": ("last_login", "date_joined")}),
    )
    add_fieldsets = (
        (None, {"classes": ("wide",), "fields": ("email", "password1", "password2")}),
    )

    def get_queryset(self, request):
        qs = super().get_queryset(request)
        return qs.annotate(
            _device_count=Count("devices", distinct=True),
            _total_listen_ms=Sum("devices__stats__total_play_ms"),
        )

    @admin.display(description="Devices", ordering="_device_count")
    def device_count(self, obj):
        return obj._device_count

    @admin.display(description="Listen Time", ordering="_total_listen_ms")
    def total_listen_hours(self, obj):
        ms = obj._total_listen_ms or 0
        hours = ms / 3_600_000
        return f"{hours:.1f}h"
```

- [ ] **Step 4: Run tests**

```bash
cd backend && pytest accounts/tests/test_admin.py -v
```

Expected: 3 tests PASS.

- [ ] **Step 5: Commit**

```bash
git add backend/accounts/admin.py backend/accounts/tests/test_admin.py
git commit -m "feat(admin): enhance User admin with device count and listen time columns"
```

---

## Task 7: Enhanced Device Admin

**Files:**
- Modify: `backend/devices/admin.py`
- Create: `backend/devices/tests/test_admin.py`

- [ ] **Step 1: Write failing tests**

Create `backend/devices/tests/test_admin.py`:

```python
import pytest
from django.contrib.auth import get_user_model
from django.test import Client
from django.utils import timezone

from devices.models import Device, ProvisioningToken

User = get_user_model()


@pytest.fixture
def superuser(db):
    return User.objects.create_superuser(email="admin@nadeem.com", password="adminpass")


@pytest.fixture
def admin_client(superuser):
    c = Client()
    c.force_login(superuser)
    return c


@pytest.mark.django_db
def test_device_list_loads(admin_client):
    response = admin_client.get("/admin/devices/device/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_provisioning_token_list_loads(admin_client):
    response = admin_client.get("/admin/devices/provisioningtoken/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_provisioning_token_cleanup_action_deletes_only_expired(admin_client, superuser):
    from datetime import timedelta

    expired = ProvisioningToken.objects.create(
        user=superuser,
        token_hash="a" * 64,
        expires_at=timezone.now() - timedelta(hours=1),
    )
    active = ProvisioningToken.objects.create(
        user=superuser,
        token_hash="b" * 64,
        expires_at=timezone.now() + timedelta(hours=1),
    )
    admin_client.post(
        "/admin/devices/provisioningtoken/",
        {"action": "delete_expired_tokens", "_selected_action": []},
    )
    assert not ProvisioningToken.objects.filter(pk=expired.pk).exists()
    assert ProvisioningToken.objects.filter(pk=active.pk).exists()
```

- [ ] **Step 2: Run to verify failure**

```bash
cd backend && pytest devices/tests/test_admin.py -v 2>&1 | head -20
```

Expected: FAIL (not registered on custom site).

- [ ] **Step 3: Rewrite devices/admin.py**

```python
from django.contrib import admin
from django.utils import timezone
from django.utils.html import format_html
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site
from stats.models import UsageStats

from .models import Device, ProvisioningToken


class UsageStatsInline(admin.TabularInline):
    model = UsageStats
    extra = 0
    readonly_fields = ["uid_hex", "play_count", "listen_hours", "pro_session_count", "updated_at"]
    can_delete = False
    max_num = 0

    @admin.display(description="Listen Time")
    def listen_hours(self, obj):
        hours = obj.total_play_ms / 3_600_000
        return f"{hours:.2f}h"


@admin_site.register(Device)
class DeviceAdmin(ModelAdmin):
    list_display = [
        "device_id", "user", "status_badge", "release_group", "last_seen_at", "created_at"
    ]
    search_fields = ["device_id", "user__email"]
    list_filter = ["release_group"]
    readonly_fields = ["device_id", "token_hash", "last_seen_at", "created_at"]
    inlines = [UsageStatsInline]

    @admin.display(description="Status")
    def status_badge(self, obj):
        if obj.last_seen_at and obj.online:
            return format_html('<span style="color:green;font-weight:bold">● Online</span>')
        return format_html('<span style="color:#9ca3af">○ Offline</span>')


@admin_site.register(ProvisioningToken)
class ProvisioningTokenAdmin(ModelAdmin):
    list_display = ["user", "token_status", "expires_at", "used_at", "created_at"]
    list_filter = ["used_at"]
    readonly_fields = ["token_hash", "expires_at", "used_at", "created_at"]
    actions = ["delete_expired_tokens"]

    @admin.display(description="Status")
    def token_status(self, obj):
        if obj.is_used():
            return format_html('<span style="color:#9ca3af">Used</span>')
        if obj.is_expired():
            return format_html('<span style="color:#ef4444">Expired</span>')
        return format_html('<span style="color:#16a34a">Active</span>')

    @admin.action(description="Delete all expired tokens (ignores selection)")
    def delete_expired_tokens(self, request, queryset):
        deleted, _ = ProvisioningToken.objects.filter(expires_at__lt=timezone.now()).delete()
        self.message_user(request, f"Deleted {deleted} expired token(s).")
```

- [ ] **Step 4: Run tests**

```bash
cd backend && pytest devices/tests/test_admin.py -v
```

Expected: 3 tests PASS.

- [ ] **Step 5: Commit**

```bash
git add backend/devices/admin.py backend/devices/tests/test_admin.py
git commit -m "feat(admin): enhance Device admin with online badge, stats inline, and token cleanup"
```

---

## Task 8: Enhanced Semsem Admin

**Files:**
- Modify: `backend/semsems/admin.py`
- Modify: `backend/semsems/tests/test_admin.py` (add admin list tests)

- [ ] **Step 1: Verify admin list tests in test_admin.py**

The admin list tests (`test_semsem_group_list_admin`, `test_semsem_list_admin`) were written in Task 3's `test_admin.py`. Run them now to confirm they still fail (not yet registered on custom site):

```bash
cd backend && pytest semsems/tests/test_admin.py -k "admin" -v 2>&1 | head -20
```

Expected: FAIL (404).

- [ ] **Step 2: Rewrite semsems/admin.py**

```python
from django.contrib import admin
from django.db.models import Count
from django.utils.html import format_html
from adminsortable2.admin import SortableInlineAdminMixin
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import Semsem, SemsemGroup, Track


class TrackInline(SortableInlineAdminMixin, admin.TabularInline):
    model = Track
    extra = 1
    fields = ["name", "file", "position", "audio_preview"]
    readonly_fields = ["audio_preview"]

    @admin.display(description="Preview")
    def audio_preview(self, obj):
        if obj.pk and obj.file:
            return format_html(
                '<audio controls style="height:32px" src="{}"></audio>', obj.file.url
            )
        return "—"


@admin_site.register(Semsem)
class SemsemAdmin(ModelAdmin):
    list_display = ["uid_hex", "title", "pro_badge", "group_list", "track_count", "created_at"]
    search_fields = ["uid_hex", "title"]
    list_filter = ["is_pro", "groups"]
    filter_horizontal = ["groups"]
    inlines = [TrackInline]

    def get_queryset(self, request):
        return super().get_queryset(request).annotate(_track_count=Count("tracks"))

    @admin.display(description="Type")
    def pro_badge(self, obj):
        if obj.is_pro:
            return format_html('<span style="color:#7c3aed;font-weight:bold">✦ Pro</span>')
        return "Regular"

    @admin.display(description="Groups")
    def group_list(self, obj):
        return ", ".join(g.name for g in obj.groups.all()) or "—"

    @admin.display(description="Tracks", ordering="_track_count")
    def track_count(self, obj):
        return obj._track_count


@admin_site.register(SemsemGroup)
class SemsemGroupAdmin(ModelAdmin):
    list_display = ["icon", "name", "description", "semsem_count"]
    search_fields = ["name"]

    def get_queryset(self, request):
        return super().get_queryset(request).annotate(_semsem_count=Count("semsems"))

    @admin.display(description="Semsems", ordering="_semsem_count")
    def semsem_count(self, obj):
        return obj._semsem_count


@admin_site.register(Track)
class TrackAdmin(ModelAdmin):
    list_display = ["semsem", "name", "position", "audio_preview"]
    list_filter = ["semsem"]
    readonly_fields = ["audio_preview"]

    @admin.display(description="Preview")
    def audio_preview(self, obj):
        if obj.file:
            return format_html(
                '<audio controls style="height:32px" src="{}"></audio>', obj.file.url
            )
        return "—"
```

- [ ] **Step 3: Run all semsem admin tests**

```bash
cd backend && pytest semsems/tests/test_admin.py -v
```

Expected: 5 tests PASS.

- [ ] **Step 4: Commit**

```bash
git add backend/semsems/admin.py
git commit -m "feat(admin): enhance Semsem admin with groups, drag-to-reorder tracks, audio preview"
```

---

## Task 9: Enhanced Chat Admin

**Files:**
- Modify: `backend/chat/admin.py`

- [ ] **Step 1: Verify chat admin tests fail**

```bash
cd backend && pytest chat/tests/test_admin.py -k "list_admin or flag_session" -v 2>&1 | head -20
```

Expected: FAIL (not registered on custom site).

- [ ] **Step 2: Rewrite chat/admin.py**

```python
from django.contrib import admin
from django.utils.html import format_html
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import ProChatSession, TranscriptEntry


class TranscriptInline(admin.TabularInline):
    model = TranscriptEntry
    readonly_fields = ["speaker", "text", "timestamp"]
    can_delete = False
    extra = 0
    ordering = ["timestamp"]


@admin_site.register(ProChatSession)
class ProChatSessionAdmin(ModelAdmin):
    list_display = [
        "device", "uid_hex", "role", "flagged_badge", "duration_display", "started_at"
    ]
    search_fields = ["uid_hex", "role", "device__device_id", "entries__text"]
    list_filter = ["role", "flagged"]
    readonly_fields = ["device", "uid_hex", "role", "started_at", "ended_at", "flagged", "flag_reason"]
    inlines = [TranscriptInline]
    actions = ["flag_sessions", "unflag_sessions"]

    @admin.display(description="Flagged")
    def flagged_badge(self, obj):
        if obj.flagged:
            return format_html('<span style="color:#ef4444;font-weight:bold">⚑ Flagged</span>')
        return "—"

    @admin.display(description="Duration")
    def duration_display(self, obj):
        if obj.ended_at and obj.started_at:
            delta = obj.ended_at - obj.started_at
            minutes = int(delta.total_seconds() // 60)
            seconds = int(delta.total_seconds() % 60)
            return f"{minutes}m {seconds}s"
        return format_html('<span style="color:#16a34a">Live</span>') if not obj.ended_at else "—"

    @admin.action(description="Flag selected sessions for review")
    def flag_sessions(self, request, queryset):
        updated = queryset.update(flagged=True)
        self.message_user(request, f"Flagged {updated} session(s).")

    @admin.action(description="Unflag selected sessions")
    def unflag_sessions(self, request, queryset):
        updated = queryset.update(flagged=False, flag_reason="")
        self.message_user(request, f"Unflagged {updated} session(s).")
```

- [ ] **Step 3: Run all chat admin tests**

```bash
cd backend && pytest chat/tests/test_admin.py -v
```

Expected: 4 tests PASS.

- [ ] **Step 4: Commit**

```bash
git add backend/chat/admin.py
git commit -m "feat(admin): enhance Chat admin with flagging actions, duration display, transcript inline"
```

---

## Task 10: Stats Admin

**Files:**
- Modify: `backend/stats/admin.py`

- [ ] **Step 1: Rewrite stats/admin.py**

```python
from django.contrib import admin
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import UsageStats


@admin_site.register(UsageStats)
class UsageStatsAdmin(ModelAdmin):
    list_display = [
        "device", "uid_hex", "play_count", "listen_hours",
        "pro_session_count", "updated_at"
    ]
    search_fields = ["uid_hex", "device__device_id", "device__user__email"]
    list_filter = ["device__user"]
    readonly_fields = [
        "device", "uid_hex", "play_count", "total_play_ms",
        "last_played_unix", "pro_session_count", "pro_total_ms", "updated_at",
    ]

    @admin.display(description="Listen Time")
    def listen_hours(self, obj):
        hours = obj.total_play_ms / 3_600_000
        return f"{hours:.2f}h"
```

- [ ] **Step 2: Verify Django check passes**

```bash
cd backend && python manage.py check
```

Expected: no issues.

- [ ] **Step 3: Commit**

```bash
git add backend/stats/admin.py
git commit -m "feat(admin): add UsageStats admin with listen time display"
```

---

## Task 11: Firmware Admin

**Files:**
- Create: `backend/firmware/admin.py`
- Create: `backend/firmware/tests/test_admin.py`

- [ ] **Step 1: Write failing tests**

Create `backend/firmware/tests/test_admin.py`:

```python
import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from firmware.models import FirmwareRelease, ReleaseGroup

User = get_user_model()


@pytest.fixture
def superuser(db):
    return User.objects.create_superuser(email="admin@nadeem.com", password="adminpass")


@pytest.fixture
def admin_client(superuser):
    c = Client()
    c.force_login(superuser)
    return c


@pytest.mark.django_db
def test_firmware_release_list_loads(admin_client):
    response = admin_client.get("/admin/firmware/firmwarerelease/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_release_group_list_loads(admin_client):
    response = admin_client.get("/admin/firmware/releasegroup/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_firmware_release_detail_loads(admin_client):
    release = FirmwareRelease.objects.create(version="1.0.0", is_stable=True)
    response = admin_client.get(f"/admin/firmware/firmwarerelease/{release.pk}/change/")
    assert response.status_code == 200
```

- [ ] **Step 2: Run to verify failure**

```bash
cd backend && pytest firmware/tests/test_admin.py -v 2>&1 | head -20
```

Expected: 404 (not registered).

- [ ] **Step 3: Create firmware/admin.py**

```python
from django.contrib import admin
from django.utils.html import format_html
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import FirmwareRelease, ReleaseGroup


class ReleaseGroupInline(admin.TabularInline):
    model = ReleaseGroup
    extra = 0
    fields = ["name", "description"]
    readonly_fields = ["name", "description"]
    can_delete = False
    max_num = 0
    verbose_name_plural = "Release Groups using this release"


@admin_site.register(FirmwareRelease)
class FirmwareReleaseAdmin(ModelAdmin):
    list_display = ["version", "stable_badge", "group_count", "created_at"]
    list_filter = ["is_stable"]
    readonly_fields = ["created_at"]
    inlines = [ReleaseGroupInline]

    @admin.display(description="Stable")
    def stable_badge(self, obj):
        if obj.is_stable:
            return format_html('<span style="color:#16a34a;font-weight:bold">✓ Stable</span>')
        return format_html('<span style="color:#f59e0b">Dev</span>')

    @admin.display(description="Groups")
    def group_count(self, obj):
        return obj.release_groups.count()


@admin_site.register(ReleaseGroup)
class ReleaseGroupAdmin(ModelAdmin):
    list_display = ["name", "assigned_release", "device_count", "created_at"]
    list_filter = ["assigned_release"]
    readonly_fields = ["created_at"]

    @admin.display(description="Devices")
    def device_count(self, obj):
        return obj.devices.count()
```

- [ ] **Step 4: Run tests**

```bash
cd backend && pytest firmware/tests/test_admin.py -v
```

Expected: 3 tests PASS.

- [ ] **Step 5: Commit**

```bash
git add backend/firmware/admin.py backend/firmware/tests/test_admin.py
git commit -m "feat(admin): add Firmware admin for releases and release groups"
```

---

## Task 12: Platform Analytics Dashboard

**Files:**
- Modify: `backend/nadeem/dashboard.py`
- Create: `backend/nadeem/templates/admin/index.html`

- [ ] **Step 1: Implement dashboard callback**

Replace `backend/nadeem/dashboard.py`:

```python
from datetime import timedelta

from django.db.models import Sum
from django.utils import timezone


def dashboard_callback(request, context):
    from accounts.models import User
    from chat.models import ProChatSession
    from devices.models import Device
    from semsems.models import Semsem
    from stats.models import UsageStats

    now = timezone.now()
    today_start = now.replace(hour=0, minute=0, second=0, microsecond=0)
    online_threshold = now - timedelta(seconds=120)

    total_users = User.objects.count()
    total_devices = Device.objects.count()
    online_devices = Device.objects.filter(last_seen_at__gte=online_threshold).count()
    total_semsems = Semsem.objects.count()

    today_listen_ms = (
        UsageStats.objects.filter(updated_at__gte=today_start)
        .aggregate(total=Sum("total_play_ms"))["total"]
        or 0
    )
    today_pro_sessions = ProChatSession.objects.filter(started_at__gte=today_start).count()

    top_stats = (
        UsageStats.objects.values("uid_hex")
        .annotate(total_plays=Sum("play_count"))
        .order_by("-total_plays")[:10]
    )
    uid_to_title = {
        s.uid_hex: s.title
        for s in Semsem.objects.filter(uid_hex__in=[s["uid_hex"] for s in top_stats])
    }
    top_semsems = [
        {
            "uid_hex": s["uid_hex"],
            "title": uid_to_title.get(s["uid_hex"], s["uid_hex"]),
            "plays": s["total_plays"] or 0,
        }
        for s in top_stats
    ]

    context.update(
        {
            "kpis": {
                "total_users": total_users,
                "total_devices": total_devices,
                "online_devices": online_devices,
                "total_semsems": total_semsems,
            },
            "today": {
                "listen_hours": round(today_listen_ms / 3_600_000, 1),
                "pro_sessions": today_pro_sessions,
            },
            "top_semsems": top_semsems,
            "online_pct": round(100 * online_devices / total_devices, 1) if total_devices else 0,
        }
    )
    return context
```

- [ ] **Step 2: Create custom dashboard template**

Create `backend/nadeem/templates/admin/index.html`:

```html
{% extends "unfold/index.html" %}
{% load i18n %}

{% block content %}
<div style="display:grid;grid-template-columns:repeat(4,1fr);gap:1rem;margin-bottom:2rem">
  <div style="background:#fff;border-radius:8px;padding:1.5rem;box-shadow:0 1px 3px rgba(0,0,0,.08)">
    <div style="font-size:2rem;font-weight:700;color:#7c3aed">{{ kpis.total_users }}</div>
    <div style="color:#6b7280;margin-top:.25rem;font-size:.875rem">Total Users</div>
  </div>
  <div style="background:#fff;border-radius:8px;padding:1.5rem;box-shadow:0 1px 3px rgba(0,0,0,.08)">
    <div style="font-size:2rem;font-weight:700;color:#2563eb">{{ kpis.total_devices }}</div>
    <div style="color:#6b7280;margin-top:.25rem;font-size:.875rem">Total Devices</div>
  </div>
  <div style="background:#fff;border-radius:8px;padding:1.5rem;box-shadow:0 1px 3px rgba(0,0,0,.08)">
    <div style="font-size:2rem;font-weight:700;color:#16a34a">{{ kpis.online_devices }}</div>
    <div style="color:#6b7280;margin-top:.25rem;font-size:.875rem">Online Now &mdash; {{ online_pct }}%</div>
  </div>
  <div style="background:#fff;border-radius:8px;padding:1.5rem;box-shadow:0 1px 3px rgba(0,0,0,.08)">
    <div style="font-size:2rem;font-weight:700;color:#d97706">{{ kpis.total_semsems }}</div>
    <div style="color:#6b7280;margin-top:.25rem;font-size:.875rem">Total Semsems</div>
  </div>
</div>

<div style="display:grid;grid-template-columns:1fr 1fr;gap:1rem;margin-bottom:2rem">
  <div style="background:#fff;border-radius:8px;padding:1.5rem;box-shadow:0 1px 3px rgba(0,0,0,.08)">
    <h3 style="margin:0 0 1rem;font-size:1rem;font-weight:600">Today</h3>
    <div style="display:flex;gap:2.5rem">
      <div>
        <div style="font-size:1.75rem;font-weight:700">{{ today.listen_hours }}h</div>
        <div style="color:#6b7280;font-size:.875rem">Listening Time</div>
      </div>
      <div>
        <div style="font-size:1.75rem;font-weight:700">{{ today.pro_sessions }}</div>
        <div style="color:#6b7280;font-size:.875rem">Pro Chat Sessions</div>
      </div>
    </div>
  </div>

  <div style="background:#fff;border-radius:8px;padding:1.5rem;box-shadow:0 1px 3px rgba(0,0,0,.08)">
    <h3 style="margin:0 0 1rem;font-size:1rem;font-weight:600">
      Top Semsems
      <a href="/admin/stats-explorer/" style="font-size:.75rem;font-weight:normal;color:#6b7280;margin-right:.5rem">View all &rarr;</a>
    </h3>
    {% if top_semsems %}
    <table style="width:100%;font-size:.875rem;border-collapse:collapse">
      {% for s in top_semsems %}
      <tr>
        <td style="padding:.2rem 0;color:#6b7280">{{ forloop.counter }}.</td>
        <td style="padding:.2rem .5rem">{{ s.title }}</td>
        <td style="padding:.2rem 0;text-align:right;color:#6b7280">{{ s.plays }} plays</td>
      </tr>
      {% endfor %}
    </table>
    {% else %}
    <p style="color:#9ca3af;font-size:.875rem">No usage data yet.</p>
    {% endif %}
  </div>
</div>

{{ block.super }}
{% endblock %}
```

- [ ] **Step 3: Verify dashboard renders**

```bash
cd backend && python manage.py runserver 8001 &
sleep 2
# Create superuser if not already created:
echo "from django.contrib.auth import get_user_model; U=get_user_model(); U.objects.filter(email='admin@nadeem.com').exists() or U.objects.create_superuser('admin@nadeem.com', 'adminpass')" | python manage.py shell
curl -s -o /dev/null -w "%{http_code}" http://localhost:8001/admin/
kill %1
```

Expected: `200`.

- [ ] **Step 4: Commit**

```bash
git add backend/nadeem/dashboard.py backend/nadeem/templates/admin/index.html
git commit -m "feat(admin): add platform analytics dashboard with KPIs, today stats, top semsems"
```

---

## Task 13: Stats Explorer template

**Files:**
- Create: `backend/nadeem/templates/admin/stats_explorer.html`

The view was already implemented in Task 2's `NadeemAdminSite.stats_explorer_view`. This task adds the template.

- [ ] **Step 1: Create stats_explorer.html**

Create `backend/nadeem/templates/admin/stats_explorer.html`:

```html
{% extends "unfold/base_site.html" %}
{% load i18n %}

{% block content %}
<h2 style="margin:0 0 1rem;font-size:1.25rem;font-weight:600">Semsem Popularity</h2>
<div style="background:#fff;border-radius:8px;box-shadow:0 1px 3px rgba(0,0,0,.08);overflow:hidden;margin-bottom:2rem">
  <table style="width:100%;border-collapse:collapse;font-size:.875rem">
    <thead>
      <tr style="background:#f9fafb;border-bottom:1px solid #e5e7eb">
        <th style="padding:.75rem 1rem;text-align:left;font-weight:600">#</th>
        <th style="padding:.75rem 1rem;text-align:left;font-weight:600">Semsem</th>
        <th style="padding:.75rem 1rem;text-align:left;font-weight:600;font-family:monospace">UID</th>
        <th style="padding:.75rem 1rem;text-align:right;font-weight:600">Plays</th>
        <th style="padding:.75rem 1rem;text-align:right;font-weight:600">Listen Hours</th>
        <th style="padding:.75rem 1rem;text-align:right;font-weight:600">Pro Sessions</th>
        <th style="padding:.75rem 1rem;text-align:right;font-weight:600">Devices</th>
      </tr>
    </thead>
    <tbody>
      {% for row in semsem_rows %}
      <tr style="border-bottom:1px solid #f3f4f6">
        <td style="padding:.75rem 1rem;color:#9ca3af">{{ forloop.counter }}</td>
        <td style="padding:.75rem 1rem;font-weight:500">{{ row.title }}</td>
        <td style="padding:.75rem 1rem;font-family:monospace;color:#6b7280;font-size:.8rem">{{ row.uid_hex }}</td>
        <td style="padding:.75rem 1rem;text-align:right">{{ row.plays }}</td>
        <td style="padding:.75rem 1rem;text-align:right">{{ row.listen_hours }}h</td>
        <td style="padding:.75rem 1rem;text-align:right">{{ row.pro_sessions }}</td>
        <td style="padding:.75rem 1rem;text-align:right">{{ row.devices }}</td>
      </tr>
      {% empty %}
      <tr>
        <td colspan="7" style="padding:2rem;text-align:center;color:#9ca3af">No usage data yet.</td>
      </tr>
      {% endfor %}
    </tbody>
  </table>
</div>

<h2 style="margin:0 0 1rem;font-size:1.25rem;font-weight:600">User Engagement</h2>
<div style="background:#fff;border-radius:8px;box-shadow:0 1px 3px rgba(0,0,0,.08);overflow:hidden">
  <table style="width:100%;border-collapse:collapse;font-size:.875rem">
    <thead>
      <tr style="background:#f9fafb;border-bottom:1px solid #e5e7eb">
        <th style="padding:.75rem 1rem;text-align:left;font-weight:600">User</th>
        <th style="padding:.75rem 1rem;text-align:right;font-weight:600">Devices</th>
        <th style="padding:.75rem 1rem;text-align:right;font-weight:600">Total Listen Hours</th>
        <th style="padding:.75rem 1rem;text-align:right;font-weight:600">Pro Sessions</th>
      </tr>
    </thead>
    <tbody>
      {% for u in user_stats %}
      <tr style="border-bottom:1px solid #f3f4f6">
        <td style="padding:.75rem 1rem">{{ u.email }}</td>
        <td style="padding:.75rem 1rem;text-align:right">{{ u.device_count }}</td>
        <td style="padding:.75rem 1rem;text-align:right">
          {% if u.total_play_ms %}
            {{ u.total_play_ms|floatformat:0 }}ms
          {% else %}—{% endif %}
        </td>
        <td style="padding:.75rem 1rem;text-align:right">{{ u.total_pro|default:"0" }}</td>
      </tr>
      {% empty %}
      <tr>
        <td colspan="4" style="padding:2rem;text-align:center;color:#9ca3af">No users.</td>
      </tr>
      {% endfor %}
    </tbody>
  </table>
</div>
{% endblock %}
```

- [ ] **Step 2: Verify stats explorer URL resolves**

```bash
cd backend && python manage.py shell -c "
from django.test import RequestFactory
from django.contrib.auth import get_user_model
U = get_user_model()
print('Stats explorer view registered:', 'stats_explorer' in [u.name for u in __import__('nadeem.admin_site', fromlist=['admin_site']).admin_site.get_urls() if hasattr(u, 'name')])
"
```

Expected: `True` or no error.

- [ ] **Step 3: Commit**

```bash
git add backend/nadeem/templates/admin/stats_explorer.html
git commit -m "feat(admin): add Stats Explorer template"
```

---

## Task 14: OTA Check Endpoint

**Files:**
- Modify: `backend/firmware/views.py`
- Modify: `backend/firmware/urls.py`
- Create: `backend/firmware/tests/test_views.py`

- [ ] **Step 1: Write failing tests**

Create `backend/firmware/tests/test_views.py`:

```python
import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from devices.models import Device
from devices.tokens import generate_token, hash_token
from firmware.models import FirmwareRelease, ReleaseGroup

User = get_user_model()


@pytest.fixture
def device_in_stable_group(db):
    user = User.objects.create_user(email="hw@test.com", password="pass")
    release = FirmwareRelease.objects.create(version="2.0.0", is_stable=True)
    group = ReleaseGroup.objects.create(name="Stable", assigned_release=release)
    raw = generate_token()
    device = Device.objects.create(
        device_id="aabbccddeef4",
        user=user,
        token_hash=hash_token(raw),
        release_group=group,
    )
    return device, raw, release


@pytest.fixture
def device_no_group(db):
    user = User.objects.create_user(email="hw2@test.com", password="pass")
    raw = generate_token()
    device = Device.objects.create(
        device_id="aabbccddeef5",
        user=user,
        token_hash=hash_token(raw),
    )
    return device, raw


def bearer(raw_token):
    return {"HTTP_AUTHORIZATION": f"Bearer {raw_token}"}


@pytest.mark.django_db
def test_ota_update_available(device_in_stable_group):
    device, raw, release = device_in_stable_group
    response = Client().get("/firmware/check", {"current_version": "1.0.0"}, **bearer(raw))
    assert response.status_code == 200
    data = response.json()
    assert data["has_update"] is True
    assert data["version"] == "2.0.0"


@pytest.mark.django_db
def test_ota_already_up_to_date(device_in_stable_group):
    device, raw, _ = device_in_stable_group
    response = Client().get("/firmware/check", {"current_version": "2.0.0"}, **bearer(raw))
    assert response.status_code == 200
    assert response.json()["has_update"] is False


@pytest.mark.django_db
def test_ota_no_group_returns_no_update(device_no_group):
    device, raw = device_no_group
    response = Client().get("/firmware/check", {"current_version": "1.0.0"}, **bearer(raw))
    assert response.status_code == 200
    assert response.json()["has_update"] is False


@pytest.mark.django_db
def test_ota_unauthenticated_returns_403():
    response = Client().get("/firmware/check", {"current_version": "1.0.0"})
    assert response.status_code in (401, 403)
```

- [ ] **Step 2: Run to verify failure**

```bash
cd backend && pytest firmware/tests/test_views.py -v 2>&1 | head -20
```

Expected: 404 (endpoint empty).

- [ ] **Step 3: Implement firmware/views.py**

```python
from rest_framework.decorators import api_view, authentication_classes, permission_classes
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response

from devices.auth import DeviceTokenAuthentication


def _version_tuple(v: str) -> tuple:
    try:
        return tuple(int(x) for x in v.strip().split("."))
    except (ValueError, AttributeError):
        return (0,)


@api_view(["GET"])
@authentication_classes([DeviceTokenAuthentication])
@permission_classes([IsAuthenticated])
def ota_check(request):
    # DeviceTokenAuthentication returns (device.user, device) — device is request.auth
    device = request.auth
    current_version = request.query_params.get("current_version", "0.0.0")

    group = getattr(device, "release_group", None)
    if not group or not group.assigned_release:
        return Response({"has_update": False})

    target = group.assigned_release
    if _version_tuple(target.version) > _version_tuple(current_version):
        return Response(
            {
                "has_update": True,
                "version": target.version,
                "download_url": target.file.url if target.file else None,
                "changelog": target.changelog,
            }
        )

    return Response({"has_update": False})
```

- [ ] **Step 4: Update firmware/urls.py**

```python
from django.urls import path

from . import views

urlpatterns = [
    path("check", views.ota_check),
]
```

- [ ] **Step 5: Run tests**

```bash
cd backend && pytest firmware/tests/test_views.py -v
```

Expected: 4 tests PASS.

- [ ] **Step 6: Commit**

```bash
git add backend/firmware/views.py backend/firmware/urls.py backend/firmware/tests/test_views.py
git commit -m "feat(firmware): add OTA check endpoint skeleton with release group routing"
```

---

## Task 15: Full test suite + verification

- [ ] **Step 1: Run full test suite**

```bash
cd backend && pytest -v 2>&1 | tail -30
```

Expected: all existing tests + all new tests PASS. Zero failures.

- [ ] **Step 2: Run Django system check**

```bash
cd backend && python manage.py check
```

Expected: `System check identified no issues (0 silenced).`

- [ ] **Step 3: Verify all admin pages load**

Start the server and manually verify each URL (logged in as superuser):

| URL | Expected |
|-----|---------|
| `/admin/` | Dashboard with KPI cards |
| `/admin/accounts/user/` | User list with Devices + Listen Time columns |
| `/admin/devices/device/` | Device list with Online/Offline badge |
| `/admin/devices/provisioningtoken/` | Token list with status badge |
| `/admin/semsems/semsem/` | Semsem list with groups + track count |
| `/admin/semsems/semsemgroup/` | Group list with semsem count |
| `/admin/chat/prochatsession/` | Session list with flagged column |
| `/admin/stats/usagestats/` | Stats list with listen hours |
| `/admin/firmware/firmwarerelease/` | Release list with stable badge |
| `/admin/firmware/releasegroup/` | Group list with device count |
| `/admin/stats-explorer/` | Two tables (semsem + user) |

- [ ] **Step 4: Final commit**

```bash
git add -A
git commit -m "chore(admin): complete Nadeem admin platform v1"
```
