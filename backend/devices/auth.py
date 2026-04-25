# devices/auth.py
from django.utils import timezone
from rest_framework import authentication, exceptions

from .models import Device
from .tokens import hash_token


class DeviceTokenAuthentication(authentication.BaseAuthentication):
    keyword = "Device"

    def authenticate(self, request):
        header = request.META.get("HTTP_AUTHORIZATION", "")
        if not header.startswith(self.keyword + " "):
            return None
        raw = header[len(self.keyword) + 1:].strip()
        if not raw:
            return None
        try:
            device = Device.objects.select_related("user").get(token_hash=hash_token(raw))
        except Device.DoesNotExist as e:
            raise exceptions.AuthenticationFailed("invalid device token") from e
        Device.objects.filter(pk=device.pk).update(last_seen_at=timezone.now())
        return (device.user, device)

    def authenticate_header(self, request):
        return self.keyword
