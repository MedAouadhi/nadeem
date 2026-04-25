from django.contrib import admin
from .models import ProChatSession, TranscriptEntry

class TranscriptInline(admin.TabularInline):
    model = TranscriptEntry
    readonly_fields = ["speaker", "text", "timestamp"]
    can_delete = False
    extra = 0

@admin.register(ProChatSession)
class ProChatSessionAdmin(admin.ModelAdmin):
    list_display = ["device", "uid_hex", "role", "started_at", "ended_at"]
    inlines = [TranscriptInline]
