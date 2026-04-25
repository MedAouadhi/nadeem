# devices/admin.py
from django.contrib import admin

from .models import Device, ProvisioningToken


@admin.register(Device)
class DeviceAdmin(admin.ModelAdmin):
    list_display = ["device_id", "user", "online", "last_seen_at", "created_at"]
    search_fields = ["device_id", "user__email"]


@admin.register(ProvisioningToken)
class ProvisioningTokenAdmin(admin.ModelAdmin):
    list_display = ["user", "expires_at", "used_at", "created_at"]
