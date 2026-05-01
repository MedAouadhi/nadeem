import pytest
from django.contrib.auth import get_user_model

from devices.models import Device
from semsems.models import Semsem
from stats.models import UsageStats

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
    Semsem.objects.create(uid_hex="bb", title="Doctor", is_pro=True, role="doctor")
    UsageStats.objects.create(device=d, uid_hex="bb", play_count=0, total_play_ms=0, last_played_unix=1, pro_session_count=1, pro_total_ms=60_000)
    r = api_client.get("/api/semsems/bb")
    assert r.status_code == 200
    assert r.data["title"] == "Doctor" and r.data["is_pro"] is True and r.data["role"] == "doctor"
    assert r.data["pro_total_ms"] == 60_000

def test_detail_404_when_not_bound(api_client):
    _login(api_client)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=False)
    r = api_client.get("/api/semsems/aa")
    assert r.status_code == 404
