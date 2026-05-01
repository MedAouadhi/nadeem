from django.conf import settings
from rest_framework import serializers

from .models import Semsem, Track


class TrackManifestSerializer(serializers.ModelSerializer):
    url = serializers.SerializerMethodField()

    class Meta:
        model = Track
        fields = ["name", "url"]

    def get_url(self, obj):
        public = settings.S3_PUBLIC_ENDPOINT_URL.rstrip("/")
        bucket = settings.STORAGES["default"]["OPTIONS"]["bucket_name"]
        return f"{public}/{bucket}/{obj.file.name}"


class FirmwareManifestSerializer(serializers.ModelSerializer):
    uid = serializers.CharField(source="uid_hex")
    pro = serializers.BooleanField(source="is_pro")
    tracks = TrackManifestSerializer(many=True, read_only=True)

    class Meta:
        model = Semsem
        fields = ["uid", "title", "pro", "role", "tracks"]

class SemsemWebListSerializer(serializers.ModelSerializer):
    class Meta:
        model = Semsem
        fields = ["uid_hex", "title", "is_pro", "role"]

class SemsemWebDetailSerializer(serializers.ModelSerializer):
    today_play_count = serializers.IntegerField(read_only=True)
    total_play_count = serializers.IntegerField(read_only=True)
    today_listening_ms = serializers.IntegerField(read_only=True)
    total_listening_ms = serializers.IntegerField(read_only=True)
    pro_session_count = serializers.IntegerField(read_only=True)
    pro_total_ms = serializers.IntegerField(read_only=True)
    last_played_at = serializers.DateTimeField(read_only=True)
    tracks = TrackManifestSerializer(many=True, read_only=True)

    class Meta:
        model = Semsem
        fields = ["uid_hex", "title", "is_pro", "role", "tracks",
                  "today_play_count", "total_play_count", "today_listening_ms", "total_listening_ms",
                  "pro_session_count", "pro_total_ms", "last_played_at"]
