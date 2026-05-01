from rest_framework import serializers

from .models import Device


class DeviceSerializer(serializers.ModelSerializer):
    online = serializers.BooleanField(read_only=True)
    class Meta:
        model = Device
        fields = ["device_id", "online", "last_seen_at", "created_at"]
        read_only_fields = fields
