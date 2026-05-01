from datetime import UTC, datetime, timedelta

from django.db import transaction
from django.db.models import Count, Sum
from django.utils import timezone
from rest_framework.permissions import IsAuthenticated
from rest_framework.response import Response
from rest_framework.views import APIView
from rest_framework_simplejwt.authentication import JWTAuthentication

from devices.auth import DeviceTokenAuthentication
from devices.models import Device
from semsems.models import Semsem

from .models import DailyUsageStats, UsageStats
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

        new_play_count = s.validated_data["play_count"]
        new_total_play_ms = s.validated_data["total_play_ms"]
        new_last_played_unix = s.validated_data["last_played_unix"]
        new_pro_session_count = s.validated_data["pro_session_count"]
        new_pro_total_ms = s.validated_data["pro_total_ms"]

        if new_last_played_unix > 0:
            try:
                day = datetime.fromtimestamp(new_last_played_unix, tz=UTC).date()
            except (OverflowError, OSError, ValueError):
                day = timezone.localdate()
        else:
            day = timezone.localdate()

        with transaction.atomic():
            stats = (
                UsageStats.objects.select_for_update()
                .filter(device=device, uid_hex=uid)
                .first()
            )

            old_play_count = stats.play_count if stats else 0
            old_total_play_ms = stats.total_play_ms if stats else 0
            old_pro_session_count = stats.pro_session_count if stats else 0
            old_pro_total_ms = stats.pro_total_ms if stats else 0

            play_count_delta = max(new_play_count - old_play_count, 0)
            play_ms_delta = max(new_total_play_ms - old_total_play_ms, 0)
            pro_session_count_delta = max(new_pro_session_count - old_pro_session_count, 0)
            pro_total_ms_delta = max(new_pro_total_ms - old_pro_total_ms, 0)

            daily_stats, _ = DailyUsageStats.objects.select_for_update().get_or_create(
                device=device,
                uid_hex=uid,
                day=day,
                defaults={
                    "play_count_delta": 0,
                    "play_ms_delta": 0,
                    "pro_session_count_delta": 0,
                    "pro_total_ms_delta": 0,
                },
            )
            daily_stats.play_count_delta += play_count_delta
            daily_stats.play_ms_delta += play_ms_delta
            daily_stats.pro_session_count_delta += pro_session_count_delta
            daily_stats.pro_total_ms_delta += pro_total_ms_delta
            daily_stats.save(update_fields=[
                "play_count_delta",
                "play_ms_delta",
                "pro_session_count_delta",
                "pro_total_ms_delta",
                "updated_at",
            ])

            UsageStats.objects.update_or_create(
                device=device,
                uid_hex=uid,
                defaults={
                    "play_count": new_play_count,
                    "total_play_ms": new_total_play_ms,
                    "last_played_unix": new_last_played_unix,
                    "pro_session_count": new_pro_session_count,
                    "pro_total_ms": new_pro_total_ms,
                },
            )
        return Response({"ok": True})

class MyStatsView(APIView):
    authentication_classes = [JWTAuthentication]
    permission_classes = [IsAuthenticated]

    def get(self, request):
        my_stats = UsageStats.objects.filter(device__user=request.user)
        my_daily_stats = DailyUsageStats.objects.filter(device__user=request.user)
        valid_uids = Semsem.objects.values_list("uid_hex", flat=True)
        agg = my_stats.filter(uid_hex__in=valid_uids).aggregate(
            total_listening_ms=Sum("total_play_ms"),
            total_pro_ms=Sum("pro_total_ms"),
            unique_semsems=Count("uid_hex", distinct=True),
        )
        today_agg = my_daily_stats.filter(
            uid_hex__in=valid_uids,
            day=timezone.localdate(),
        ).aggregate(
            today_listening_ms=Sum("play_ms_delta"),
            today_pro_ms=Sum("pro_total_ms_delta"),
        )
        user_devices = Device.objects.filter(user=request.user)
        online_cutoff = timezone.now() - Device.DEVICE_ONLINE_TTL
        return Response({
            "today_listening_ms": today_agg["today_listening_ms"] or 0,
            "total_listening_ms": agg["total_listening_ms"] or 0,
            "today_pro_ms": today_agg["today_pro_ms"] or 0,
            "total_pro_ms": agg["total_pro_ms"] or 0,
            "unique_semsems": agg["unique_semsems"] or 0,
            "device_count": user_devices.count(),
            "online_device_count": user_devices.filter(last_seen_at__gte=online_cutoff).count(),
        })
