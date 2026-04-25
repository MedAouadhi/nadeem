from django.core.validators import RegexValidator
from rest_framework import serializers

UID_HEX_RE = RegexValidator(r"^[0-9a-f]{2,20}$", "uid must be 2..20 lowercase hex chars")

class StatsUploadSerializer(serializers.Serializer):
    uid = serializers.CharField(max_length=20, validators=[UID_HEX_RE])
    play_count = serializers.IntegerField(min_value=0)
    total_play_ms = serializers.IntegerField(min_value=0)
    last_played_unix = serializers.IntegerField(min_value=0)
    pro_session_count = serializers.IntegerField(min_value=0)
    pro_total_ms = serializers.IntegerField(min_value=0)
