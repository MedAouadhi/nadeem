import datetime

import pytest
from django.contrib.auth import get_user_model

from devices.models import Device
from devices.tokens import generate_token, hash_token
from semsems.models import Semsem
from stats.models import DailyUsageStats, UsageStats

pytestmark = pytest.mark.django_db


def _device(api_client):
    user = get_user_model().objects.create_user(email="rollup@b.com", password="x")
    raw = generate_token()
    d = Device.objects.create(device_id="123456abcdef", user=user, token_hash=hash_token(raw))
    api_client.credentials(HTTP_AUTHORIZATION=f"Device {raw}")
    return d


def test_first_upload_creates_daily_delta_from_cumulative(api_client):
    d = _device(api_client)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=True, role="doctor")

    body = {
        "uid": "aa",
        "play_count": 7,
        "total_play_ms": 1000,
        "last_played_unix": 1_735_430_400,
        "pro_session_count": 2,
        "pro_total_ms": 500,
    }
    r = api_client.post("/stats", body, format="json")

    assert r.status_code == 200
    s = UsageStats.objects.get(device=d, uid_hex="aa")
    assert s.play_count == 7
    assert s.total_play_ms == 1000
    assert s.pro_session_count == 2
    assert s.pro_total_ms == 500

    day = datetime.datetime.fromtimestamp(body["last_played_unix"], tz=datetime.UTC).date()
    daily = DailyUsageStats.objects.get(device=d, uid_hex="aa", day=day)
    assert daily.play_count_delta == 7
    assert daily.play_ms_delta == 1000
    assert daily.pro_session_count_delta == 2
    assert daily.pro_total_ms_delta == 500


def test_upload_adds_non_negative_deltas_and_updates_snapshot_on_regression(api_client):
    d = _device(api_client)
    Semsem.objects.create(uid_hex="bb", title="x", is_pro=True, role="doctor")

    UsageStats.objects.create(
        device=d,
        uid_hex="bb",
        play_count=10,
        total_play_ms=3000,
        last_played_unix=100,
        pro_session_count=5,
        pro_total_ms=800,
    )
    day = datetime.date(2025, 1, 10)
    DailyUsageStats.objects.create(
        device=d,
        uid_hex="bb",
        day=day,
        play_count_delta=3,
        play_ms_delta=500,
        pro_session_count_delta=1,
        pro_total_ms_delta=200,
    )

    body = {
        "uid": "bb",
        "play_count": 8,
        "total_play_ms": 3500,
        "last_played_unix": int(datetime.datetime(2025, 1, 10, tzinfo=datetime.UTC).timestamp()),
        "pro_session_count": 4,
        "pro_total_ms": 1000,
    }
    r = api_client.post("/stats", body, format="json")

    assert r.status_code == 200
    daily = DailyUsageStats.objects.get(device=d, uid_hex="bb", day=day)
    assert daily.play_count_delta == 3
    assert daily.play_ms_delta == 1000
    assert daily.pro_session_count_delta == 1
    assert daily.pro_total_ms_delta == 400

    s = UsageStats.objects.get(device=d, uid_hex="bb")
    assert s.play_count == 8
    assert s.total_play_ms == 3500
    assert s.pro_session_count == 4
    assert s.pro_total_ms == 1000
