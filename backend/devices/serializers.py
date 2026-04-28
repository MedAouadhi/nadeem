from rest_framework import serializers

from .models import Device


class DeviceSerializer(serializers.ModelSerializer):
    online_status = serializers.BooleanField(source="online", read_only=True)
    bootstrapped = serializers.SerializerMethodField()

    class Meta:
        model = Device
        fields = ["device_id", "online_status", "bootstrapped", "last_seen_at", "created_at"]
        read_only_fields = fields

    def get_bootstrapped(self, obj):
        return True
