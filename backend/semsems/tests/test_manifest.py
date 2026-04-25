from unittest.mock import patch

import pytest
from django.contrib.auth import get_user_model
from django.core.files.base import ContentFile

from devices.models import Device
from devices.tokens import generate_token, hash_token
from semsems.models import Semsem, Track

pytestmark = pytest.mark.django_db


def _device(api_client):
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    raw = generate_token()
    Device.objects.create(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    api_client.credentials(HTTP_AUTHORIZATION=f"Device {raw}")


def test_returns_regular_manifest_with_absolute_urls(api_client):
    _device(api_client)
    s = Semsem.objects.create(uid_hex="04a3f91b2c", title="Animal Sounds", is_pro=False)
    t = Track(semsem=s, name="01.mp3", position=1)
    
    # Mock file save to avoid S3 calls during test
    with patch("django.core.files.storage.base.Storage.save") as mock_save:
        mock_save.return_value = "audio/01.mp3"
        t.file.save("01.mp3", ContentFile(b"id3"))
    
    t.save()
    r = api_client.get("/semsem/04a3f91b2c/manifest")
    assert r.status_code == 200
    body = r.json()
    assert body["uid"] == "04a3f91b2c"
    assert body["pro"] is False
    assert body["title"] == "Animal Sounds"
    assert body["tracks"][0]["name"] == "01.mp3"
    assert body["tracks"][0]["url"].startswith("http")


def test_returns_pro_manifest(api_client):
    _device(api_client)
    Semsem.objects.create(uid_hex="aa", title="Doctor Sem", is_pro=True, role="doctor")
    r = api_client.get("/semsem/aa/manifest")
    assert r.status_code == 200
    assert r.json() == {"uid": "aa", "title": "Doctor Sem", "pro": True, "role": "doctor", "tracks": []}


def test_unknown_uid_returns_404(api_client):
    _device(api_client)
    r = api_client.get("/semsem/deadbeef/manifest")
    assert r.status_code == 404


def test_uppercase_uid_normalised_or_404(api_client):
    _device(api_client)
    Semsem.objects.create(uid_hex="aa", title="x", is_pro=True, role="doctor")
    r = api_client.get("/semsem/AA/manifest")
    assert r.status_code in (200, 404)


def test_unauth_is_401(api_client):
    r = api_client.get("/semsem/aa/manifest")
    assert r.status_code == 401
