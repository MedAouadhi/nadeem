from django.db.models import Sum
from django.utils import timezone
from rest_framework import serializers

from .models import Device


class DeviceSerializer(serializers.ModelSerializer):
    online = serializers.BooleanField(read_only=True)
    bootstrapped = serializers.SerializerMethodField()
    today_listening_ms = serializers.SerializerMethodField()
    total_listening_ms = serializers.SerializerMethodField()
    total_semsems = serializers.SerializerMethodField()

    class Meta:
        model = Device
        fields = [
            "device_id",
            "online",
            "bootstrapped",
            "last_seen_at",
            "created_at",
            "today_listening_ms",
            "total_listening_ms",
            "total_semsems",
        ]
        read_only_fields = fields

    def get_bootstrapped(self, obj):
        return True

    def get_today_listening_ms(self, obj):
        value = getattr(obj, "today_listening_ms", None)
        if value is not None:
            return value
        return obj.daily_stats.filter(day=timezone.localdate()).aggregate(total=Sum("play_ms_delta"))["total"] or 0

    def get_total_listening_ms(self, obj):
        value = getattr(obj, "total_listening_ms", None)
        if value is not None:
            return value
        return obj.stats.aggregate(total=Sum("total_play_ms"))["total"] or 0

    def get_total_semsems(self, obj):
        value = getattr(obj, "total_semsems", None)
        if value is not None:
            return value
        return obj.stats.values("uid_hex").distinct().count()
