from django.db import models
from django.utils import timezone

from devices.models import Device


class ProChatSession(models.Model):
    device = models.ForeignKey(Device, on_delete=models.CASCADE, related_name="chat_sessions")
    uid_hex = models.CharField(max_length=20)
    role = models.CharField(max_length=31)
    started_at = models.DateTimeField(default=timezone.now)
    ended_at = models.DateTimeField(null=True, blank=True)
    flagged = models.BooleanField(default=False)
    flag_reason = models.CharField(max_length=256, blank=True, default="")

    class Meta:
        ordering = ["-started_at"]

class TranscriptEntry(models.Model):
    SPEAKER_CHOICES = [("child", "child"), ("ai", "ai")]
    session = models.ForeignKey(ProChatSession, on_delete=models.CASCADE, related_name="entries")
    speaker = models.CharField(max_length=8, choices=SPEAKER_CHOICES)
    text = models.TextField()
    timestamp = models.DateTimeField(default=timezone.now)

    class Meta:
        ordering = ["timestamp", "id"]
