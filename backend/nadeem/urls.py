from django.contrib import admin
from django.urls import include, path

urlpatterns = [
    path("admin/", admin.site.urls),
    path("", include("devices.urls")),
    path("", include("semsems.urls")),
    path("", include("stats.urls")),
    path("", include("chat.urls")),
    path("api/auth/", include("accounts.urls")),
]
