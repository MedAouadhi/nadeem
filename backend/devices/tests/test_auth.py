# devices/tests/test_auth.py
import pytest
from django.contrib.auth import get_user_model
from rest_framework.test import APIRequestFactory

from devices.auth import DeviceTokenAuthentication
from devices.models import Device
from devices.tokens import generate_token, hash_token

pytestmark = pytest.mark.django_db


def _make(user_email="p@x.com"):
    user = get_user_model().objects.create_user(email=user_email, password="x")
    raw = generate_token()
    Device.objects.create(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    return raw


def test_authenticates_valid_bearer():
    raw = _make()
    rf = APIRequestFactory()
    req = rf.get("/", HTTP_AUTHORIZATION=f"Device {raw}")
    user, dev = DeviceTokenAuthentication().authenticate(req)
    assert dev.device_id == "aabbccddeeff"


def test_rejects_missing_header():
    rf = APIRequestFactory()
    req = rf.get("/")
    assert DeviceTokenAuthentication().authenticate(req) is None


def test_rejects_unknown_token():
    rf = APIRequestFactory()
    req = rf.get("/", HTTP_AUTHORIZATION="Device not-a-real-token")
    from rest_framework.exceptions import AuthenticationFailed
    with pytest.raises(AuthenticationFailed):
        DeviceTokenAuthentication().authenticate(req)


def test_authenticates_valid_bearer_keyword():
    raw = _make()
    rf = APIRequestFactory()
    req = rf.get("/", HTTP_AUTHORIZATION=f"Bearer {raw}")
    user, dev = DeviceTokenAuthentication().authenticate(req)
    assert dev.device_id == "aabbccddeeff"


def test_bearer_and_device_return_same_device():
    raw = _make()
    rf = APIRequestFactory()
    req_bearer = rf.get("/", HTTP_AUTHORIZATION=f"Bearer {raw}")
    req_device = rf.get("/", HTTP_AUTHORIZATION=f"Device {raw}")
    _, dev_bearer = DeviceTokenAuthentication().authenticate(req_bearer)
    _, dev_device = DeviceTokenAuthentication().authenticate(req_device)
    assert dev_bearer.pk == dev_device.pk
