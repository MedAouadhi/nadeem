from datetime import UTC, datetime

from django.db.models import Sum
from rest_framework import generics
from rest_framework.response import Response
from rest_framework_simplejwt.authentication import JWTAuthentication

from devices.auth import DeviceTokenAuthentication
from django.utils import timezone

from stats.models import DailyUsageStats, UsageStats

from .models import Semsem
from .serializers import (
    FirmwareManifestSerializer,
    SemsemWebDetailSerializer,
    SemsemWebListSerializer,
)


class FirmwareManifestView(generics.RetrieveAPIView):
    authentication_classes = [DeviceTokenAuthentication]
    serializer_class = FirmwareManifestSerializer
    queryset = Semsem.objects.prefetch_related("tracks")
    lookup_field = "uid_hex"
    lookup_url_kwarg = "uid_hex"

    def get_object(self):
        self.kwargs["uid_hex"] = self.kwargs["uid_hex"].lower()
        return super().get_object()

def _bound_uids(user):
    return UsageStats.objects.filter(device__user=user).values_list("uid_hex", flat=True).distinct()

class SemsemListWebView(generics.ListAPIView):
    authentication_classes = [JWTAuthentication]
    serializer_class = SemsemWebListSerializer

    def get_queryset(self):
        return Semsem.objects.filter(uid_hex__in=_bound_uids(self.request.user)).order_by("uid_hex")

class SemsemDetailWebView(generics.RetrieveAPIView):
    authentication_classes = [JWTAuthentication]
    serializer_class = SemsemWebDetailSerializer
    lookup_field = "uid_hex"

    def get_queryset(self):
        return Semsem.objects.filter(uid_hex__in=_bound_uids(self.request.user)).prefetch_related("tracks")

    def retrieve(self, request, *args, **kwargs):
        instance = self.get_object()
        usage_agg = UsageStats.objects.filter(device__user=request.user, uid_hex=instance.uid_hex).aggregate(
            total_play_count=Sum("play_count"),
            total_listening_ms=Sum("total_play_ms"),
            pro_session_count=Sum("pro_session_count"),
            pro_total_ms=Sum("pro_total_ms"),
        )
        today_agg = DailyUsageStats.objects.filter(
            device__user=request.user,
            uid_hex=instance.uid_hex,
            day=timezone.localdate(),
        ).aggregate(
            today_play_count=Sum("play_count_delta"),
            today_listening_ms=Sum("play_ms_delta"),
        )

        last_played_at = None
        candidate_unix_values = UsageStats.objects.filter(
            device__user=request.user,
            uid_hex=instance.uid_hex,
            last_played_unix__gt=0,
        ).values_list("last_played_unix", flat=True).order_by("-last_played_unix")
        for last_played_unix in candidate_unix_values:
            try:
                last_played_at = datetime.fromtimestamp(last_played_unix, tz=UTC)
                break
            except (OverflowError, OSError, ValueError):
                continue

        merged = {
            **usage_agg,
            **today_agg,
        }

        for k, v in merged.items():
            setattr(instance, k, v or 0)
        instance.last_played_at = last_played_at
        return Response(self.get_serializer(instance).data)
