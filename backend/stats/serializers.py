from rest_framework import serializers

class StatsUploadSerializer(serializers.Serializer):
    uid = serializers.CharField(max_length=20)
    play_count = serializers.IntegerField(min_value=0)
    total_play_ms = serializers.IntegerField(min_value=0)
    last_played_unix = serializers.IntegerField(min_value=0)
    pro_session_count = serializers.IntegerField(min_value=0)
    pro_total_ms = serializers.IntegerField(min_value=0)
