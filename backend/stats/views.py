from django.db.models import Sum, Count
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response
from rest_framework.views import APIView
from rest_framework_simplejwt.authentication import JWTAuthentication
from devices.auth import DeviceTokenAuthentication
from devices.models import Device
from .models import UsageStats
from .serializers import StatsUploadSerializer

class StatsUploadView(APIView):
    authentication_classes = [DeviceTokenAuthentication]
    permission_classes = [IsAuthenticated]

    def post(self, request):
        s = StatsUploadSerializer(data=request.data)
        s.is_valid(raise_exception=True)
        device = request.auth
        UsageStats.objects.update_or_create(
            device=device, uid_hex=s.validated_data["uid"].lower(),
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
        agg = my_stats.aggregate(
            total_listening_ms=Sum("total_play_ms"),
            pro_total_ms=Sum("pro_total_ms"),
            unique_semsems=Count("uid_hex", distinct=True),
        )
        devices = Device.objects.filter(user=request.user)
        return Response({
            "total_listening_ms": agg["total_listening_ms"] or 0,
            "unique_semsems": agg["unique_semsems"] or 0,
            "pro_total_ms": agg["pro_total_ms"] or 0,
            "device_count": devices.count(),
            "online_device_count": sum(1 for d in devices if d.online),
        })
