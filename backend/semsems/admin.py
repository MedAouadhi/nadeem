from django.contrib import admin
from .models import Semsem, Track

class TrackInline(admin.TabularInline):
    model = Track

@admin.register(Semsem)
class SemsemAdmin(admin.ModelAdmin):
    list_display = ["uid_hex", "title", "is_pro", "role"]
    inlines = [TrackInline]

@admin.register(Track)
class TrackAdmin(admin.ModelAdmin):
    list_display = ["semsem", "name", "position"]
