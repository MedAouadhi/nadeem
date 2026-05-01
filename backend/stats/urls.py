from django.urls import path

from .views import MyStatsView, StatsUploadView

urlpatterns = [
    path("stats", StatsUploadView.as_view()),
    path("api/users/me/stats", MyStatsView.as_view()),
]
