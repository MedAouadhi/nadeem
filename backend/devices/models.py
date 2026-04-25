# devices/models.py
from django.conf import settings
from django.core.exceptions import ValidationError
from django.core.validators import RegexValidator
from django.db import models
from django.utils import timezone

from .tokens import hash_token

DEVICE_ID_RE = RegexValidator(r"^[0-9a-f]{12}$", "device_id must be 12 lowercase hex chars")


class Device(models.Model):
    device_id = models.CharField(max_length=12, unique=True, validators=[DEVICE_ID_RE])
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.CASCADE, related_name="devices")
    token_hash = models.CharField(max_length=64, unique=True)
    last_seen_at = models.DateTimeField(null=True, blank=True)
    created_at = models.DateTimeField(default=timezone.now)

    def clean(self):
        if self.device_id != self.device_id.lower():
            raise ValidationError("device_id must be lowercase")

    def save(self, *a, **kw):
        self.full_clean()
        return super().save(*a, **kw)

    @property
    def online(self) -> bool:
        if not self.last_seen_at:
            return False
        return (timezone.now() - self.last_seen_at).total_seconds() < 120


class ProvisioningToken(models.Model):
    user = models.ForeignKey(settings.AUTH_USER_MODEL, on_delete=models.CASCADE)
    token_hash = models.CharField(max_length=64, unique=True)
    expires_at = models.DateTimeField()
    used_at = models.DateTimeField(null=True, blank=True)
    created_at = models.DateTimeField(default=timezone.now)

    @staticmethod
    def hash(raw: str) -> str:
        return hash_token(raw)

    def is_expired(self) -> bool:
        return timezone.now() >= self.expires_at

    def is_used(self) -> bool:
        return self.used_at is not None
