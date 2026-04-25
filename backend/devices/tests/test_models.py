# devices/tests/test_models.py
import pytest
from django.contrib.auth import get_user_model
from django.utils import timezone
from datetime import timedelta
from devices.models import Device, ProvisioningToken
from devices.tokens import generate_token

pytestmark = pytest.mark.django_db


def _user():
    return get_user_model().objects.create_user(email="p@x.com", password="x")


def test_device_id_must_be_12_lower_hex():
    user = _user()
    Device.objects.create(device_id="aabbccddeeff", user=user, token_hash="0"*64)
    with pytest.raises(Exception):
        Device.objects.create(device_id="AABBCCDDEEFF", user=user, token_hash="1"*64)
    with pytest.raises(Exception):
        Device.objects.create(device_id="zz", user=user, token_hash="2"*64)


def test_device_id_unique():
    user = _user()
    Device.objects.create(device_id="aabbccddeeff", user=user, token_hash="0"*64)
    with pytest.raises(Exception):
        Device.objects.create(device_id="aabbccddeeff", user=user, token_hash="1"*64)


def test_provisioning_token_expiry_helper():
    user = _user()
    raw = generate_token()
    pt = ProvisioningToken.objects.create(user=user, token_hash=ProvisioningToken.hash(raw),
                                          expires_at=timezone.now() + timedelta(minutes=10))
    assert not pt.is_expired()
    pt.expires_at = timezone.now() - timedelta(seconds=1)
    assert pt.is_expired()


def test_provisioning_token_lookup_by_hash():
    user = _user()
    raw = generate_token()
    ProvisioningToken.objects.create(user=user, token_hash=ProvisioningToken.hash(raw),
                                     expires_at=timezone.now() + timedelta(minutes=10))
    found = ProvisioningToken.objects.filter(token_hash=ProvisioningToken.hash(raw)).first()
    assert found is not None and found.user == user
