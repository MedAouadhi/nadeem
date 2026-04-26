from django.contrib import admin
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import UsageStats


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


admin_site.register(UsageStats, UsageStatsAdmin)
