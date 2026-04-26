from django.contrib import admin
from django.contrib.auth.admin import UserAdmin as BaseUserAdmin
from django.db.models import Count, Sum
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import User

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


admin_site.register(User, UserAdmin)
