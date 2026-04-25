import pytest
from io import StringIO
from django.core.management import call_command
from django.test import override_settings
from django.contrib.auth import get_user_model
from devices.models import Device
from devices.tokens import hash_token
pytestmark = pytest.mark.django_db

def test_management_command_creates_device():
    get_user_model().objects.create_user(email="a@b.com", password="x")
    out = StringIO()
    call_command("provision_dev_device", "--email", "a@b.com", "--device-id", "aabbccddeeff", stdout=out)
    raw = out.getvalue().strip().split()[-1]
    assert Device.objects.filter(device_id="aabbccddeeff", token_hash=hash_token(raw)).exists()

def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")

@override_settings(DEV_PROVISIONING_ENABLED=True)
def test_dev_endpoint_creates_device_when_flag_on(api_client):
    _login(api_client)
    r = api_client.post("/api/dev/devices", {"device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 201
    assert Device.objects.filter(device_id="aabbccddeeff").exists()

@override_settings(DEV_PROVISIONING_ENABLED=False)
def test_dev_endpoint_returns_404_when_flag_off(api_client):
    _login(api_client)
    r = api_client.post("/api/dev/devices", {"device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 404
