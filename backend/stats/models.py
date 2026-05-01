from django.db import models

from devices.models import Device


class UsageStats(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE, related_name="stats")
    uid_hex = models.CharField(max_length=20)
    play_count = models.PositiveIntegerField(default=0)
    total_play_ms = models.PositiveBigIntegerField(default=0)
    last_played_unix = models.BigIntegerField(default=0)
    pro_session_count = models.PositiveIntegerField(default=0)
    pro_total_ms = models.PositiveBigIntegerField(default=0)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        unique_together = [("device", "uid_hex")]

class DailyUsageStats(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE, related_name="daily_stats")
    uid_hex = models.CharField(max_length=20)
    day = models.DateField()
    play_count_delta = models.PositiveIntegerField(default=0)
    play_ms_delta = models.PositiveBigIntegerField(default=0)
    pro_session_count_delta = models.PositiveIntegerField(default=0)
    pro_total_ms_delta = models.PositiveBigIntegerField(default=0)
    created_at = models.DateTimeField(auto_now_add=True)
    updated_at = models.DateTimeField(auto_now=True)

    class Meta:
        constraints = [
            models.UniqueConstraint(
                fields=["device", "uid_hex", "day"],
                name="stats_dailyusagestats_device_uid_day_uniq",
            )
        ]
