import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from firmware.models import FirmwareRelease, ReleaseGroup

User = get_user_model()


@pytest.fixture
def superuser(db):
    return User.objects.create_superuser(email="admin@nadeem.com", password="adminpass")


@pytest.fixture
def admin_client(superuser):
    c = Client()
    c.force_login(superuser)
    return c


@pytest.mark.django_db
def test_firmware_release_list_loads(admin_client):
    response = admin_client.get("/admin/firmware/firmwarerelease/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_release_group_list_loads(admin_client):
    response = admin_client.get("/admin/firmware/releasegroup/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_firmware_release_detail_loads(admin_client):
    release = FirmwareRelease.objects.create(version="1.0.0", is_stable=True)
    response = admin_client.get(f"/admin/firmware/firmwarerelease/{release.pk}/change/")
    assert response.status_code == 200
