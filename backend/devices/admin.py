from django.contrib import admin
from django.utils import timezone
from django.utils.html import format_html
from django.utils.safestring import mark_safe
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
            return mark_safe('<span style="color:green;font-weight:bold">● Online</span>')
        return mark_safe('<span style="color:#9ca3af">○ Offline</span>')


class ProvisioningTokenAdmin(ModelAdmin):
    list_display = ["user", "token_status", "expires_at", "used_at", "created_at"]
    list_filter = ["used_at"]
    readonly_fields = ["token_hash", "expires_at", "used_at", "created_at"]
    actions = ["delete_expired_tokens"]

    @admin.display(description="Status")
    def token_status(self, obj):
        if obj.is_used():
            return mark_safe('<span style="color:#9ca3af">Used</span>')
        if obj.is_expired():
            return mark_safe('<span style="color:#ef4444">Expired</span>')
        return mark_safe('<span style="color:#16a34a">Active</span>')

    @admin.action(description="Delete all expired tokens (ignores selection)")
    def delete_expired_tokens(self, request, queryset):
        deleted, _ = ProvisioningToken.objects.filter(expires_at__lt=timezone.now()).delete()
        self.message_user(request, f"Deleted {deleted} expired token(s).")


admin_site.register(Device, DeviceAdmin)
admin_site.register(ProvisioningToken, ProvisioningTokenAdmin)
