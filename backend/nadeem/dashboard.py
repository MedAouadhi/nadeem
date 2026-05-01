from datetime import timedelta

from django.db.models import Sum
from django.utils import timezone


def dashboard_callback(request, context):
    from accounts.models import User
    from chat.models import ProChatSession
    from devices.models import Device
    from semsems.models import Semsem
    from stats.models import UsageStats

    now = timezone.now()
    today_start = now.replace(hour=0, minute=0, second=0, microsecond=0)
    online_threshold = now - timedelta(seconds=120)

    total_users = User.objects.count()
    total_devices = Device.objects.count()
    online_devices = Device.objects.filter(last_seen_at__gte=online_threshold).count()
    total_semsems = Semsem.objects.count()

    today_listen_ms = (
        UsageStats.objects.filter(updated_at__gte=today_start)
        .aggregate(total=Sum("total_play_ms"))["total"]
        or 0
    )
    today_pro_sessions = ProChatSession.objects.filter(started_at__gte=today_start).count()

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
                "listen_hours": round(today_listen_ms / 3_600_000, 1),
                "pro_sessions": today_pro_sessions,
            },
            "top_semsems": top_semsems,
            "online_pct": round(100 * online_devices / total_devices, 1) if total_devices else 0,
        }
    )
    return context
