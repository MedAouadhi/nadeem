from django.contrib import admin
from django.utils.translation import gettext_lazy as _
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import UsageStats


class UsageStatsAdmin(ModelAdmin):
    list_display = [
        "device", "uid_hex", "play_count", "listen_minutes",
        "pro_session_count", "updated_at"
    ]
    search_fields = ["uid_hex", "device__device_id", "device__user__email"]
    list_filter = ["device__user"]
    readonly_fields = [
        "device", "uid_hex", "play_count", "total_play_ms",
        "last_played_unix", "pro_session_count", "pro_total_ms", "updated_at",
    ]

    @admin.display(description=_("Listen Minutes"))
    def listen_minutes(self, obj):
        minutes = obj.total_play_ms / 60_000
        return f"{minutes:.1f}m"


admin_site.register(UsageStats, UsageStatsAdmin)
