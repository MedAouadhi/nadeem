import pytest
from django.contrib.auth import get_user_model

from devices.models import Device
from firmware.models import FirmwareRelease, ReleaseGroup

User = get_user_model()


@pytest.mark.django_db
def test_firmware_release_creation():
    release = FirmwareRelease.objects.create(
        version="1.0.0",
        changelog="Initial release",
        is_stable=True,
    )
    assert release.version == "1.0.0"
    assert release.is_stable is True
    assert str(release) == "v1.0.0"


@pytest.mark.django_db
def test_release_group_creation():
    release = FirmwareRelease.objects.create(version="1.0.0", is_stable=True)
    group = ReleaseGroup.objects.create(
        name="Beta Testers",
        description="Early adopter devices",
        assigned_release=release,
    )
    assert group.name == "Beta Testers"
    assert group.assigned_release == release
    assert str(group) == "Beta Testers"


@pytest.mark.django_db
def test_release_group_without_release():
    group = ReleaseGroup.objects.create(name="Unassigned")
    assert group.assigned_release is None


@pytest.mark.django_db
def test_device_can_be_assigned_to_release_group():
    user = User.objects.create_user(email="fw@test.com", password="pass")
    device = Device.objects.create(
        device_id="aabbccddeef1", user=user, token_hash="z" * 64
    )
    release = FirmwareRelease.objects.create(version="1.0.0", is_stable=True)
    group = ReleaseGroup.objects.create(name="Stable", assigned_release=release)
    device.release_group = group
    device.save()
    device.refresh_from_db()
    assert device.release_group == group
    assert device in group.devices.all()
