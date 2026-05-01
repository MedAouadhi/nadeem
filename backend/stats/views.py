from datetime import timedelta

from django.db.models import Count, Sum
from django.utils import timezone
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response
from rest_framework.views import APIView
from rest_framework_simplejwt.authentication import JWTAuthentication

from devices.auth import DeviceTokenAuthentication
from devices.models import Device
from semsems.models import Semsem

from .models import UsageStats
from .serializers import StatsUploadSerializer


class StatsUploadView(APIView):
    authentication_classes = [DeviceTokenAuthentication]
    permission_classes = [IsAuthenticated]

    def post(self, request):
        s = StatsUploadSerializer(data=request.data)
        s.is_valid(raise_exception=True)
        device = request.auth
        uid = s.validated_data["uid"].lower()
        if not Semsem.objects.filter(uid_hex=uid).exists():
            return Response({"detail": "unknown semsem"}, status=400)
        UsageStats.objects.update_or_create(
            device=device, uid_hex=uid,
            defaults={
                "play_count": s.validated_data["play_count"],
                "total_play_ms": s.validated_data["total_play_ms"],
                "last_played_unix": s.validated_data["last_played_unix"],
                "pro_session_count": s.validated_data["pro_session_count"],
                "pro_total_ms": s.validated_data["pro_total_ms"],
            },
        )
        return Response({"ok": True})

class MyStatsView(APIView):
    authentication_classes = [JWTAuthentication]
    permission_classes = [IsAuthenticated]

    def get(self, request):
        my_stats = UsageStats.objects.filter(device__user=request.user)
        valid_uids = Semsem.objects.values_list("uid_hex", flat=True)
        agg = my_stats.filter(uid_hex__in=valid_uids).aggregate(
            total_listening_ms=Sum("total_play_ms"),
            pro_total_ms=Sum("pro_total_ms"),
            unique_semsems=Count("uid_hex", distinct=True),
        )
        user_devices = Device.objects.filter(user=request.user)
        online_cutoff = timezone.now() - timedelta(seconds=120)
        return Response({
            "total_listening_ms": agg["total_listening_ms"] or 0,
            "unique_semsems": agg["unique_semsems"] or 0,
            "pro_total_ms": agg["pro_total_ms"] or 0,
            "device_count": user_devices.count(),
            "online_device_count": user_devices.filter(last_seen_at__gte=online_cutoff).count(),
        })
