from django.contrib import admin
from django.utils.html import format_html
from django.utils.safestring import mark_safe
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


class FirmwareReleaseAdmin(ModelAdmin):
    list_display = ["version", "stable_badge", "group_count", "created_at"]
    list_filter = ["is_stable"]
    readonly_fields = ["created_at"]
    inlines = [ReleaseGroupInline]

    @admin.display(description="Stable")
    def stable_badge(self, obj):
        if obj.is_stable:
            return mark_safe('<span style="color:#16a34a;font-weight:bold">✓ Stable</span>')
        return mark_safe('<span style="color:#f59e0b">Dev</span>')

    @admin.display(description="Groups")
    def group_count(self, obj):
        return obj.release_groups.count()


class ReleaseGroupAdmin(ModelAdmin):
    list_display = ["name", "assigned_release", "device_count", "created_at"]
    list_filter = ["assigned_release"]
    readonly_fields = ["created_at"]

    @admin.display(description="Devices")
    def device_count(self, obj):
        return obj.devices.count()


admin_site.register(FirmwareRelease, FirmwareReleaseAdmin)
admin_site.register(ReleaseGroup, ReleaseGroupAdmin)
