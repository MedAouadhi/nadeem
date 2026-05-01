import os
import urllib.parse as up
from datetime import timedelta
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent

SECRET_KEY = os.environ["DJANGO_SECRET_KEY"]
DEBUG = os.environ.get("DJANGO_DEBUG", "false").lower() == "true"
ALLOWED_HOSTS = os.environ.get("DJANGO_ALLOWED_HOSTS", "").split(",")

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

STATIC_ROOT = BASE_DIR / "staticfiles"
STATICFILES_STORAGE = "whitenoise.storage.CompressedManifestStaticFilesStorage"

MIDDLEWARE = [
    "corsheaders.middleware.CorsMiddleware",
    "django.middleware.security.SecurityMiddleware",
    "whitenoise.middleware.WhiteNoiseMiddleware",
    "django.contrib.sessions.middleware.SessionMiddleware",
    "django.middleware.locale.LocaleMiddleware",
    "django.middleware.common.CommonMiddleware",
    "django.middleware.csrf.CsrfViewMiddleware",
    "django.contrib.auth.middleware.AuthenticationMiddleware",
    "django.contrib.messages.middleware.MessageMiddleware",
]

ROOT_URLCONF = "nadeem.urls"
TEMPLATES = [{
    "BACKEND": "django.template.backends.django.DjangoTemplates",
    "DIRS": [BASE_DIR / "nadeem" / "templates"],
    "APP_DIRS": True,
    "OPTIONS": {"context_processors": [
        "django.template.context_processors.debug",
        "django.template.context_processors.request",
        "django.contrib.auth.context_processors.auth",
        "django.contrib.messages.context_processors.messages",
        "django.template.context_processors.i18n",
    ]},
}]

ASGI_APPLICATION = "nadeem.asgi.application"
WSGI_APPLICATION = "nadeem.wsgi.application"

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
    "DEFAULT_THROTTLE_RATES": {
        "bootstrap": "10/hour",
    },
}

SIMPLE_JWT = {
    "ACCESS_TOKEN_LIFETIME": timedelta(minutes=60),
    "REFRESH_TOKEN_LIFETIME": timedelta(days=14),
}

STORAGES = {
    "default": {
        "BACKEND": "nadeem.storage.PublicS3Storage",
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

LANGUAGE_CODE = "en-us"
LANGUAGES = [
    ("en", "English"),
    ("ar", "العربية"),
]
TIME_ZONE = "UTC"
USE_I18N = True
USE_L10N = True
USE_TZ = True

LOCALE_PATHS = [BASE_DIR / "locale"]

DEFAULT_AUTO_FIELD = "django.db.models.BigAutoField"
STATIC_URL = "static/"

from django.utils.translation import gettext_lazy as _

UNFOLD = {
    "SITE_TITLE": "Nadeem Admin",
    "SITE_HEADER": _("Nadeem — Administration Panel"),
    "SITE_URL": "/",
    "SHOW_LANGUAGES": True,
    "DASHBOARD_CALLBACK": "nadeem.dashboard.dashboard_callback",
    "SIDEBAR": {
        "navigation": [
            {
                "title": _("Content"),
                "separator": True,
                "items": [
                    {"title": _("Semsems"), "icon": "toys", "link": "/admin/semsems/semsem/"},
                    {"title": _("Semsem Groups"), "icon": "folder", "link": "/admin/semsems/semsemgroup/"},
                    {"title": _("Tracks"), "icon": "music_note", "link": "/admin/semsems/track/"},
                ],
            },
            {
                "title": _("Users & Devices"),
                "separator": True,
                "items": [
                    {"title": _("Users"), "icon": "person", "link": "/admin/accounts/user/"},
                    {"title": _("Devices"), "icon": "devices", "link": "/admin/devices/device/"},
                    {"title": _("Provisioning Tokens"), "icon": "key", "link": "/admin/devices/provisioningtoken/"},
                ],
            },
            {
                "title": _("AI & Chat"),
                "separator": True,
                "items": [
                    {"title": _("Chat Sessions"), "icon": "chat", "link": "/admin/chat/prochatsession/"},
                ],
            },
            {
                "title": _("Analytics"),
                "separator": True,
                "items": [
                    {"title": _("Usage Stats"), "icon": "bar_chart", "link": "/admin/stats/usagestats/"},
                    {"title": _("Stats Explorer"), "icon": "analytics", "link": "/admin/stats-explorer/"},
                ],
            },
            {
                "title": _("Firmware"),
                "separator": True,
                "items": [
                    {"title": _("Releases"), "icon": "system_update", "link": "/admin/firmware/firmwarerelease/"},
                    {"title": _("Release Groups"), "icon": "group_work", "link": "/admin/firmware/releasegroup/"},
                ],
            },
        ]
    },
}

if not DEBUG:
    SECURE_SSL_REDIRECT = True
    SESSION_COOKIE_SECURE = True
    CSRF_COOKIE_SECURE = True
    SECURE_HSTS_SECONDS = 60 * 60 * 24 * 30
    SECURE_HSTS_INCLUDE_SUBDOMAINS = True
    SECURE_PROXY_SSL_HEADER = ("HTTP_X_FORWARDED_PROTO", "https")
    SECURE_CONTENT_TYPE_NOSNIFF = True
