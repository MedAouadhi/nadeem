from django.contrib import admin
from django.utils.html import format_html
from django.utils.safestring import mark_safe
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import ProChatSession, TranscriptEntry


class TranscriptInline(admin.TabularInline):
    model = TranscriptEntry
    readonly_fields = ["speaker", "text", "timestamp"]
    can_delete = False
    extra = 0
    ordering = ["timestamp"]


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
            return mark_safe('<span style="color:#ef4444;font-weight:bold">⚑ Flagged</span>')
        return "—"

    @admin.display(description="Duration")
    def duration_display(self, obj):
        if obj.ended_at and obj.started_at:
            delta = obj.ended_at - obj.started_at
            minutes = int(delta.total_seconds() // 60)
            seconds = int(delta.total_seconds() % 60)
            return f"{minutes}m {seconds}s"
        return mark_safe('<span style="color:#16a34a">Live</span>') if not obj.ended_at else "—"

    @admin.action(description="Flag selected sessions for review")
    def flag_sessions(self, request, queryset):
        updated = queryset.update(flagged=True)
        self.message_user(request, f"Flagged {updated} session(s).")

    @admin.action(description="Unflag selected sessions")
    def unflag_sessions(self, request, queryset):
        updated = queryset.update(flagged=False, flag_reason="")
        self.message_user(request, f"Unflagged {updated} session(s).")


admin_site.register(ProChatSession, ProChatSessionAdmin)
