import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from devices.models import Device
from devices.tokens import generate_token, hash_token
from firmware.models import FirmwareRelease, ReleaseGroup

User = get_user_model()


@pytest.fixture
def device_in_stable_group(db):
    user = User.objects.create_user(email="hw@test.com", password="pass")
    release = FirmwareRelease.objects.create(version="2.0.0", is_stable=True)
    group = ReleaseGroup.objects.create(name="Stable", assigned_release=release)
    raw = generate_token()
    device = Device.objects.create(
        device_id="aabbccddeef4",
        user=user,
        token_hash=hash_token(raw),
        release_group=group,
    )
    return device, raw, release


@pytest.fixture
def device_no_group(db):
    user = User.objects.create_user(email="hw2@test.com", password="pass")
    raw = generate_token()
    device = Device.objects.create(
        device_id="aabbccddeef5",
        user=user,
        token_hash=hash_token(raw),
    )
    return device, raw


def bearer(raw_token):
    return {"HTTP_AUTHORIZATION": f"Bearer {raw_token}"}


@pytest.mark.django_db
def test_ota_update_available(device_in_stable_group):
    device, raw, release = device_in_stable_group
    response = Client().get("/firmware/check", {"current_version": "1.0.0"}, **bearer(raw))
    assert response.status_code == 200
    data = response.json()
    assert data["has_update"] is True
    assert data["version"] == "2.0.0"


@pytest.mark.django_db
def test_ota_already_up_to_date(device_in_stable_group):
    device, raw, _ = device_in_stable_group
    response = Client().get("/firmware/check", {"current_version": "2.0.0"}, **bearer(raw))
    assert response.status_code == 200
    assert response.json()["has_update"] is False


@pytest.mark.django_db
def test_ota_no_group_returns_no_update(device_no_group):
    device, raw = device_no_group
    response = Client().get("/firmware/check", {"current_version": "1.0.0"}, **bearer(raw))
    assert response.status_code == 200
    assert response.json()["has_update"] is False


@pytest.mark.django_db
def test_ota_unauthenticated_returns_403():
    response = Client().get("/firmware/check", {"current_version": "1.0.0"})
    assert response.status_code in (401, 403)
