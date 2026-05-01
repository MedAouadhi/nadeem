import pytest
from django.contrib.auth import get_user_model
from django.utils import timezone

from devices.models import Device
from semsems.models import Semsem
from stats.models import DailyUsageStats, UsageStats

pytestmark = pytest.mark.django_db

def _login(api_client, email="a@b.com"):
    get_user_model().objects.create_user(email=email, password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": email, "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")

def test_lists_only_my_devices(api_client):
    _login(api_client, "a@b.com")
    me = get_user_model().objects.get(email="a@b.com")
    other = get_user_model().objects.create_user(email="x@y.com", password="x")
    Device.objects.create(device_id="aaaaaaaaaaaa", user=me, token_hash="a"*64)
    Device.objects.create(device_id="bbbbbbbbbbbb", user=other, token_hash="b"*64)
    r = api_client.get("/api/devices")
    assert r.status_code == 200 and len(r.data) == 1 and r.data[0]["device_id"] == "aaaaaaaaaaaa"

def test_delete_revokes_my_device(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    Device.objects.create(device_id="aaaaaaaaaaaa", user=me, token_hash="a"*64)
    r = api_client.delete("/api/devices/aaaaaaaaaaaa")
    assert r.status_code == 204
    assert not Device.objects.filter(device_id="aaaaaaaaaaaa").exists()

def test_cannot_delete_other_users_device(api_client):
    _login(api_client)
    other = get_user_model().objects.create_user(email="x@y.com", password="x")
    Device.objects.create(device_id="cccccccccccc", user=other, token_hash="c"*64)
    r = api_client.delete("/api/devices/cccccccccccc")
    assert r.status_code == 404

def test_device_serializer_exposes_online_key(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    device = Device.objects.create(
        device_id="111111111111",
        user=me,
        token_hash="1" * 64,
        last_seen_at=timezone.now(),
    )

    Semsem.objects.create(uid_hex="aa", title="Alpha", is_pro=False)
    Semsem.objects.create(uid_hex="bb", title="Bravo", is_pro=False)

    UsageStats.objects.create(
        device=device,
        uid_hex="aa",
        play_count=1,
        total_play_ms=100_000,
        last_played_unix=1,
        pro_session_count=0,
        pro_total_ms=0,
    )
    UsageStats.objects.create(
        device=device,
        uid_hex="bb",
        play_count=2,
        total_play_ms=60_000,
        last_played_unix=2,
        pro_session_count=0,
        pro_total_ms=0,
    )

    DailyUsageStats.objects.create(
        device=device,
        uid_hex="aa",
        day=timezone.localdate(),
        play_count_delta=1,
        play_ms_delta=40_000,
        pro_session_count_delta=0,
        pro_total_ms_delta=0,
    )
    DailyUsageStats.objects.create(
        device=device,
        uid_hex="bb",
        day=timezone.localdate(),
        play_count_delta=1,
        play_ms_delta=20_000,
        pro_session_count_delta=0,
        pro_total_ms_delta=0,
    )

    r = api_client.get("/api/devices")
    assert r.status_code == 200
    assert r.data == [
        {
            "device_id": "111111111111",
            "online": True,
            "bootstrapped": True,
            "last_seen_at": r.data[0]["last_seen_at"],
            "created_at": r.data[0]["created_at"],
            "today_listening_ms": 60_000,
            "total_listening_ms": 160_000,
            "total_semsems": 2,
        }
    ]
