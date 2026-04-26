import pytest
from django.contrib.auth import get_user_model
from django.test import Client
from django.utils import timezone

from devices.models import Device, ProvisioningToken

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
def test_device_list_loads(admin_client):
    response = admin_client.get("/admin/devices/device/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_provisioning_token_list_loads(admin_client):
    response = admin_client.get("/admin/devices/provisioningtoken/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_provisioning_token_cleanup_action_deletes_only_expired(admin_client, superuser):
    from datetime import timedelta

    expired = ProvisioningToken.objects.create(
        user=superuser,
        token_hash="a" * 64,
        expires_at=timezone.now() - timedelta(hours=1),
    )
    active = ProvisioningToken.objects.create(
        user=superuser,
        token_hash="b" * 64,
        expires_at=timezone.now() + timedelta(hours=1),
    )
    response = admin_client.post(
        "/admin/devices/provisioningtoken/",
        {"action": "delete_expired_tokens", "_selected_action": [active.pk]},
    )
    assert not ProvisioningToken.objects.filter(pk=expired.pk).exists()
    assert ProvisioningToken.objects.filter(pk=active.pk).exists()
