import pytest
from django.contrib.auth import get_user_model

from devices.models import ProvisioningToken

pytestmark = pytest.mark.django_db

def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")

def test_creates_provisioning_token(api_client):
    _login(api_client)
    r = api_client.post("/api/provisioning-tokens", {}, format="json")
    assert r.status_code == 201
    assert "provision_token" in r.data and "expires_at" in r.data
    assert ProvisioningToken.objects.count() == 1
    assert ProvisioningToken.objects.first().token_hash != r.data["provision_token"]

def test_unauth_is_401(api_client):
    assert api_client.post("/api/provisioning-tokens", {}, format="json").status_code == 401
