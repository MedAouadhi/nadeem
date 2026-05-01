from django.template.response import TemplateResponse
from django.urls import path
from unfold.sites import UnfoldAdminSite


class NadeemAdminSite(UnfoldAdminSite):
    site_header = "Nadeem"
    site_title = "Nadeem Admin"
    index_title = "Management Platform"

    def get_urls(self):
        urls = super().get_urls()
        custom = [
            path(
                "stats-explorer/",
                self.admin_view(self.stats_explorer_view),
                name="stats_explorer",
            ),
        ]
        return custom + urls

    def stats_explorer_view(self, request):
        from django.db.models import Count, Sum
        from django.utils import timezone

        from accounts.models import User
        from semsems.models import Semsem
        from stats.models import DailyUsageStats, UsageStats

        today = timezone.localdate()

        semsem_stats = (
            UsageStats.objects.values("uid_hex")
            .annotate(
                total_plays=Sum("play_count"),
                total_listen_ms=Sum("total_play_ms"),
                total_pro_sessions=Sum("pro_session_count"),
                device_count=Count("device", distinct=True),
            )
            .order_by("-total_plays")
        )
        uid_to_title = {
            s.uid_hex: s.title
            for s in Semsem.objects.filter(uid_hex__in=[s["uid_hex"] for s in semsem_stats])
        }
        today_semsem_ms = {
            row["uid_hex"]: row["total"] or 0
            for row in DailyUsageStats.objects.filter(day=today)
            .values("uid_hex")
            .annotate(total=Sum("play_ms_delta"))
        }
        semsem_rows = [
            {
                "uid_hex": s["uid_hex"],
                "title": uid_to_title.get(s["uid_hex"], s["uid_hex"]),
                "plays": s["total_plays"] or 0,
                "listen_minutes": round((s["total_listen_ms"] or 0) / 60_000, 1),
                "today_minutes": round((today_semsem_ms.get(s["uid_hex"], 0)) / 60_000, 1),
                "pro_sessions": s["total_pro_sessions"] or 0,
                "devices": s["device_count"] or 0,
            }
            for s in semsem_stats
        ]

        user_stats = User.objects.annotate(
            device_count=Count("devices", distinct=True),
            total_play_ms=Sum("devices__stats__total_play_ms"),
            total_pro=Sum("devices__stats__pro_session_count"),
        ).order_by("-total_play_ms")

        context = {
            **self.each_context(request),
            "title": "Stats Explorer",
            "semsem_rows": semsem_rows,
            "user_stats": user_stats,
        }
        return TemplateResponse(request, "admin/stats_explorer.html", context)


admin_site = NadeemAdminSite(name="admin")
