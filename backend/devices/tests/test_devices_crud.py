import pytest
from django.contrib.auth import get_user_model

from devices.models import Device

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
