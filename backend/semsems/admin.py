from django.contrib import admin
from django.db.models import Count
from django.utils.html import format_html
from django.utils.safestring import mark_safe
from django.utils.translation import gettext_lazy as _
from adminsortable2.admin import SortableAdminBase, SortableInlineAdminMixin
from unfold.admin import ModelAdmin

from nadeem.admin_site import admin_site

from .models import Semsem, SemsemGroup, Track


class TrackInline(SortableInlineAdminMixin, admin.TabularInline):
    model = Track
    extra = 1
    fields = ["name", "file", "position", "audio_preview"]
    readonly_fields = ["audio_preview"]

    @admin.display(description=_("Preview"))
    def audio_preview(self, obj):
        if obj.pk and obj.file:
            return format_html(
                '<audio controls style="height:32px" src="{}"></audio>', obj.file.url
            )
        return "—"


class SemsemAdmin(SortableAdminBase, ModelAdmin):
    list_display = ["uid_hex", "title", "pro_badge", "group_list", "track_count", "created_at"]
    search_fields = ["uid_hex", "title"]
    list_filter = ["is_pro", "groups"]
    filter_horizontal = ["groups"]
    inlines = [TrackInline]

    def get_queryset(self, request):
        return super().get_queryset(request).annotate(_track_count=Count("tracks"))

    @admin.display(description=_("Type"))
    def pro_badge(self, obj):
        if obj.is_pro:
            return mark_safe('<span style="color:#7c3aed;font-weight:bold">✦ Pro</span>')
        return "Regular"

    @admin.display(description=_("Groups"))
    def group_list(self, obj):
        return ", ".join(g.name for g in obj.groups.all()) or "—"

    @admin.display(description=_("Tracks"), ordering="_track_count")
    def track_count(self, obj):
        return obj._track_count


class SemsemGroupAdmin(ModelAdmin):
    list_display = ["icon", "name", "description", "semsem_count"]
    search_fields = ["name"]

    def get_queryset(self, request):
        return super().get_queryset(request).annotate(_semsem_count=Count("semsems"))

    @admin.display(description=_("Semsems"), ordering="_semsem_count")
    def semsem_count(self, obj):
        return obj._semsem_count


class TrackAdmin(ModelAdmin):
    list_display = ["semsem", "name", "position", "audio_preview"]
    list_filter = ["semsem"]
    readonly_fields = ["audio_preview"]

    @admin.display(description=_("Preview"))
    def audio_preview(self, obj):
        if obj.file:
            return format_html(
                '<audio controls style="height:32px" src="{}"></audio>', obj.file.url
            )
        return "—"


admin_site.register(Semsem, SemsemAdmin)
admin_site.register(SemsemGroup, SemsemGroupAdmin)
admin_site.register(Track, TrackAdmin)
