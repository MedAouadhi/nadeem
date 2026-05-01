import re
from datetime import timedelta

from django.conf import settings
from django.db import transaction
from django.utils import timezone
from rest_framework import generics, permissions, status, throttling
from rest_framework.exceptions import NotFound
from rest_framework.response import Response
from rest_framework.views import APIView

from .models import Device, ProvisioningToken
from .serializers import DeviceSerializer
from .tokens import generate_token, hash_token

PROVISIONING_TTL = timedelta(minutes=10)
DEVICE_ID_PATTERN = re.compile(r"^[0-9a-f]{12}$")

class ProvisioningTokenCreateView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def post(self, request):
        raw = generate_token()
        expires_at = timezone.now() + PROVISIONING_TTL
        ProvisioningToken.objects.create(
            user=request.user, token_hash=hash_token(raw), expires_at=expires_at
        )
        return Response(
            {"provision_token": raw, "expires_at": expires_at.isoformat()},
            status=status.HTTP_201_CREATED,
        )

class BootstrapView(APIView):
    authentication_classes = []
    permission_classes = [permissions.AllowAny]
    throttle_classes = [throttling.ScopedRateThrottle]
    throttle_scope = "bootstrap"

    def post(self, request):
        raw = request.data.get("provision_token", "")
        device_id = (request.data.get("device_id") or "").lower()
        if not raw or not DEVICE_ID_PATTERN.match(device_id):
            return Response({"detail": "bad request"}, status=status.HTTP_400_BAD_REQUEST)

        device_token = generate_token()
        try:
            with transaction.atomic():
                pt = ProvisioningToken.objects.select_related("user").select_for_update().get(
                    token_hash=hash_token(raw)
                )
                if pt.is_used() or pt.is_expired():
                    return Response({"detail": "invalid token"}, status=status.HTTP_400_BAD_REQUEST)
                pt.used_at = timezone.now()
                pt.save(update_fields=["used_at"])
                Device.objects.update_or_create(
                    device_id=device_id,
                    defaults={"user": pt.user, "token_hash": hash_token(device_token)},
                )
        except ProvisioningToken.DoesNotExist:
            return Response({"detail": "invalid token"}, status=status.HTTP_400_BAD_REQUEST)
        return Response({"device_id": device_id, "device_token": device_token})

class DevProvisionView(APIView):
    permission_classes = [permissions.IsAuthenticated]

    def post(self, request):
        if not getattr(settings, "DEV_PROVISIONING_ENABLED", False):
            raise NotFound()
        device_id = (request.data.get("device_id") or "").lower()
        if not DEVICE_ID_PATTERN.match(device_id):
            return Response({"detail": "bad device_id"}, status=status.HTTP_400_BAD_REQUEST)
        raw = generate_token()
        Device.objects.update_or_create(
            device_id=device_id,
            defaults={"user": request.user, "token_hash": hash_token(raw)},
        )
        return Response(
            {"device_id": device_id, "device_token": raw}, status=status.HTTP_201_CREATED
        )

class DeviceListView(generics.ListAPIView):
    serializer_class = DeviceSerializer
    permission_classes = [permissions.IsAuthenticated]

    def get_queryset(self):
        return Device.objects.filter(user=self.request.user).order_by("created_at")

class DeviceDestroyView(generics.DestroyAPIView):
    serializer_class = DeviceSerializer
    permission_classes = [permissions.IsAuthenticated]
    lookup_field = "device_id"

    def get_queryset(self):
        return Device.objects.filter(user=self.request.user)
