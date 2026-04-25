from rest_framework import serializers
from .models import ProChatSession, TranscriptEntry

class TranscriptEntrySerializer(serializers.ModelSerializer):
    class Meta:
        model = TranscriptEntry
        fields = ["speaker", "text", "timestamp"]

class ProChatSessionListSerializer(serializers.ModelSerializer):
    device_id = serializers.CharField(source="device.device_id", read_only=True)
    class Meta:
        model = ProChatSession
        fields = ["id", "device_id", "uid_hex", "role", "started_at", "ended_at"]

class ProChatSessionDetailSerializer(ProChatSessionListSerializer):
    entries = TranscriptEntrySerializer(many=True, read_only=True)
    class Meta(ProChatSessionListSerializer.Meta):
        fields = ProChatSessionListSerializer.Meta.fields + ["entries"]
