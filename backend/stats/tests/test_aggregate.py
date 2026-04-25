import pytest
from django.contrib.auth import get_user_model
from django.utils import timezone

from devices.models import Device
from semsems.models import Semsem
from stats.models import UsageStats

pytestmark = pytest.mark.django_db

def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")

def test_aggregates_across_my_devices(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    d1 = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    d2 = Device.objects.create(device_id="b"*12, user=me, token_hash="b"*64)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=False)
    Semsem.objects.create(uid_hex="bb", title="y", is_pro=True, role="doctor")
    UsageStats.objects.create(device=d1, uid_hex="aa", play_count=1, total_play_ms=60_000,
                              last_played_unix=1, pro_session_count=0, pro_total_ms=0)
    UsageStats.objects.create(device=d2, uid_hex="bb", play_count=0, total_play_ms=0,
                              last_played_unix=2, pro_session_count=3, pro_total_ms=120_000)
    r = api_client.get("/api/users/me/stats")
    assert r.status_code == 200
    assert r.data == {
        "total_listening_ms": 60_000,
        "unique_semsems": 2,
        "pro_total_ms": 120_000,
        "device_count": 2,
        "online_device_count": 0,
    }

def test_excludes_other_users(api_client):
    _login(api_client)
    other = get_user_model().objects.create_user(email="x@y.com", password="x")
    d = Device.objects.create(device_id="c"*12, user=other, token_hash="c"*64)
    UsageStats.objects.create(device=d, uid_hex="aa", play_count=10, total_play_ms=999_999,
                              last_played_unix=1, pro_session_count=0, pro_total_ms=0)
    r = api_client.get("/api/users/me/stats")
    assert r.data["total_listening_ms"] == 0
    assert r.data["device_count"] == 0

def test_excludes_phantom_uids_without_semsem(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    d = Device.objects.create(device_id="d"*12, user=me, token_hash="d"*64)
    UsageStats.objects.create(device=d, uid_hex="ff", play_count=5, total_play_ms=5000,
                              last_played_unix=1, pro_session_count=0, pro_total_ms=0)
    r = api_client.get("/api/users/me/stats")
    assert r.data["unique_semsems"] == 0
    assert r.data["total_listening_ms"] == 0

def test_online_device_count_uses_db_query(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    Device.objects.create(device_id="e"*12, user=me, token_hash="e"*64, last_seen_at=timezone.now())
    Device.objects.create(device_id="f"*12, user=me, token_hash="f"*64)
    r = api_client.get("/api/users/me/stats")
    assert r.data["device_count"] == 2
    assert r.data["online_device_count"] == 1
