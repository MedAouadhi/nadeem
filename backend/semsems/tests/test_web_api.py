import pytest
from django.contrib.auth import get_user_model
from django.utils import timezone

from devices.models import Device
from semsems.models import Semsem
from stats.models import DailyUsageStats, UsageStats

pytestmark = pytest.mark.django_db

def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")

def test_lists_bound_semsems(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    d = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    Semsem.objects.create(uid_hex="aa", title="Animals", is_pro=False)
    Semsem.objects.create(uid_hex="bb", title="Doctor", is_pro=True, role="doctor")
    Semsem.objects.create(uid_hex="cc", title="Unbound", is_pro=False)
    UsageStats.objects.create(device=d, uid_hex="aa", play_count=1, total_play_ms=1, last_played_unix=1, pro_session_count=0, pro_total_ms=0)
    UsageStats.objects.create(device=d, uid_hex="bb", play_count=0, total_play_ms=0, last_played_unix=1, pro_session_count=1, pro_total_ms=10)
    r = api_client.get("/api/semsems")
    assert r.status_code == 200
    uids = sorted(s["uid_hex"] for s in r.data)
    assert uids == ["aa", "bb"]

def test_detail_returns_full_record(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    d = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    d2 = Device.objects.create(device_id="b"*12, user=me, token_hash="b"*64)
    d3 = Device.objects.create(device_id="c"*12, user=me, token_hash="c"*64)
    Semsem.objects.create(uid_hex="bb", title="Doctor", is_pro=True, role="doctor")

    UsageStats.objects.create(
        device=d,
        uid_hex="bb",
        play_count=4,
        total_play_ms=300_000,
        last_played_unix=1_700_000_000,
        pro_session_count=1,
        pro_total_ms=60_000,
    )
    UsageStats.objects.create(
        device=d2,
        uid_hex="bb",
        play_count=2,
        total_play_ms=120_000,
        last_played_unix=1_700_003_600,
        pro_session_count=3,
        pro_total_ms=30_000,
    )
    UsageStats.objects.create(
        device=d3,
        uid_hex="bb",
        play_count=0,
        total_play_ms=0,
        last_played_unix=9_999_999_999_999_999,
        pro_session_count=0,
        pro_total_ms=0,
    )

    today = timezone.localdate()
    DailyUsageStats.objects.create(
        device=d,
        uid_hex="bb",
        day=today,
        play_count_delta=1,
        play_ms_delta=50_000,
        pro_session_count_delta=1,
        pro_total_ms_delta=10_000,
    )
    DailyUsageStats.objects.create(
        device=d2,
        uid_hex="bb",
        day=today,
        play_count_delta=2,
        play_ms_delta=70_000,
        pro_session_count_delta=2,
        pro_total_ms_delta=20_000,
    )

    r = api_client.get("/api/semsems/bb")
    assert r.status_code == 200
    assert r.data["title"] == "Doctor" and r.data["is_pro"] is True and r.data["role"] == "doctor"
    assert r.data["today_play_count"] == 3
    assert r.data["total_play_count"] == 6
    assert r.data["today_listening_ms"] == 120_000
    assert r.data["total_listening_ms"] == 420_000
    assert r.data["pro_session_count"] == 4
    assert r.data["pro_total_ms"] == 90_000
    assert r.data["last_played_at"] == "2023-11-14T23:13:20Z"

def test_detail_404_when_not_bound(api_client):
    _login(api_client)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=False)
    r = api_client.get("/api/semsems/aa")
    assert r.status_code == 404
