from django.db.models import Sum
from django.utils import timezone


def dashboard_callback(request, context):
    from accounts.models import User
    from devices.models import Device
    from semsems.models import Semsem
    from stats.models import DailyUsageStats, UsageStats

    now = timezone.now()
    online_threshold = now - Device.DEVICE_ONLINE_TTL

    total_users = User.objects.count()
    total_devices = Device.objects.count()
    online_devices = Device.objects.filter(last_seen_at__gte=online_threshold).count()
    total_semsems = Semsem.objects.count()

    today = timezone.localdate()
    today_rollup = DailyUsageStats.objects.filter(day=today).aggregate(
        total_listen_ms=Sum("play_ms_delta"),
        total_pro_ms=Sum("pro_total_ms_delta"),
    )

    top_stats = (
        UsageStats.objects.values("uid_hex")
        .annotate(total_plays=Sum("play_count"))
        .order_by("-total_plays")[:10]
    )
    uid_to_title = {
        s.uid_hex: s.title
        for s in Semsem.objects.filter(uid_hex__in=[s["uid_hex"] for s in top_stats])
    }
    top_semsems = [
        {
            "uid_hex": s["uid_hex"],
            "title": uid_to_title.get(s["uid_hex"], s["uid_hex"]),
            "plays": s["total_plays"] or 0,
        }
        for s in top_stats
    ]

    context.update(
        {
            "kpis": {
                "total_users": total_users,
                "total_devices": total_devices,
                "online_devices": online_devices,
                "total_semsems": total_semsems,
            },
            "today": {
                "listen_minutes": round((today_rollup["total_listen_ms"] or 0) / 60_000, 1),
                "pro_minutes": round((today_rollup["total_pro_ms"] or 0) / 60_000, 1),
            },
            "top_semsems": top_semsems,
            "online_pct": round(100 * online_devices / total_devices, 1) if total_devices else 0,
        }
    )
    return context
