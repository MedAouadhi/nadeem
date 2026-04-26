import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from devices.models import Device

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
def test_user_list_loads(admin_client):
    response = admin_client.get("/admin/accounts/user/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_user_list_shows_device_count_column(admin_client, superuser):
    Device.objects.create(device_id="aabbccddeef2", user=superuser, token_hash="a" * 64)
    response = admin_client.get("/admin/accounts/user/")
    assert response.status_code == 200
    # column header present
    assert b"Devices" in response.content or b"device_count" in response.content


@pytest.mark.django_db
def test_user_admin_search_by_email(admin_client, superuser):
    response = admin_client.get("/admin/accounts/user/?q=admin%40nadeem.com")
    assert response.status_code == 200
    assert b"admin@nadeem.com" in response.content
