import pytest
from django.contrib.auth import get_user_model

from devices.models import Device
from devices.tokens import generate_token, hash_token
from semsems.models import Semsem
from stats.models import UsageStats

pytestmark = pytest.mark.django_db

def _device(api_client):
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    raw = generate_token()
    d = Device.objects.create(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    api_client.credentials(HTTP_AUTHORIZATION=f"Device {raw}")
    return d

def test_first_upload_creates_stats(api_client):
    d = _device(api_client)
    Semsem.objects.create(uid_hex="04a3f91b2c", title="x", is_pro=False)
    body = {"uid": "04a3f91b2c", "play_count": 7, "total_play_ms": 284300,
            "last_played_unix": 1734551234, "pro_session_count": 0, "pro_total_ms": 0}
    r = api_client.post("/stats", body, format="json")
    assert r.status_code == 200
    s = UsageStats.objects.get(device=d, uid_hex="04a3f91b2c")
    assert s.play_count == 7 and s.total_play_ms == 284300

def test_upload_overwrites_cumulative(api_client):
    d = _device(api_client)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=True, role="doctor")
    UsageStats.objects.create(device=d, uid_hex="aa", play_count=3, total_play_ms=1000,
                              last_played_unix=1, pro_session_count=0, pro_total_ms=0)
    body = {"uid": "aa", "play_count": 5, "total_play_ms": 2000,
            "last_played_unix": 9999, "pro_session_count": 1, "pro_total_ms": 60000}
    api_client.post("/stats", body, format="json")
    s = UsageStats.objects.get(device=d, uid_hex="aa")
    assert s.play_count == 5 and s.pro_session_count == 1

def test_unauth_is_401(api_client):
    r = api_client.post("/stats", {"uid": "aa"}, format="json")
    assert r.status_code == 401

def test_rejects_invalid_uid_hex(api_client):
    _device(api_client)
    for bad_uid in ["ZZZZ", "", "a", "g1"]:
        r = api_client.post("/stats", {"uid": bad_uid, "play_count": 0, "total_play_ms": 0,
                                        "last_played_unix": 0, "pro_session_count": 0, "pro_total_ms": 0}, format="json")
        assert r.status_code == 400, f"uid={bad_uid!r} should be rejected"

def test_rejects_unknown_semsem_uid(api_client):
    _device(api_client)
    r = api_client.post("/stats", {"uid": "deadbeef", "play_count": 1, "total_play_ms": 100,
                                    "last_played_unix": 0, "pro_session_count": 0, "pro_total_ms": 0}, format="json")
    assert r.status_code == 400
    assert "unknown semsem" in str(r.data)
