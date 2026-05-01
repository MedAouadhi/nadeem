from django.urls import include, path

from nadeem.admin_site import admin_site

urlpatterns = [
    path("i18n/", include("django.conf.urls.i18n")),
    path("", include("devices.urls")),
    path("", include("semsems.urls")),
    path("", include("stats.urls")),
    path("", include("chat.urls")),
    path("api/auth/", include("accounts.urls")),
    path("firmware/", include("firmware.urls")),
    path("admin/", admin_site.urls),
]
