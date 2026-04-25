import pytest
from django.contrib.auth import get_user_model
from django.utils import timezone
from datetime import timedelta
from devices.models import Device, ProvisioningToken
from devices.tokens import generate_token, hash_token
pytestmark = pytest.mark.django_db

def _seed_pt():
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    raw = generate_token()
    ProvisioningToken.objects.create(user=user, token_hash=hash_token(raw), expires_at=timezone.now() + timedelta(minutes=10))
    return raw, user

def test_bootstrap_exchanges_token(api_client):
    raw, user = _seed_pt()
    r = api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 200
    assert r.data["device_id"] == "aabbccddeeff"
    dt = r.data["device_token"]
    assert dt and len(dt) >= 40
    assert Device.objects.filter(device_id="aabbccddeeff", user=user, token_hash=hash_token(dt)).exists()

def test_bootstrap_marks_token_used(api_client):
    raw, _ = _seed_pt()
    api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    pt = ProvisioningToken.objects.first()
    assert pt.used_at is not None

def test_bootstrap_rejects_reused_token(api_client):
    raw, _ = _seed_pt()
    api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    r = api_client.post("/bootstrap", {"provision_token": raw, "device_id": "112233445566"}, format="json")
    assert r.status_code == 400

def test_bootstrap_rejects_expired_token(api_client):
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    raw = generate_token()
    ProvisioningToken.objects.create(user=user, token_hash=hash_token(raw), expires_at=timezone.now() - timedelta(seconds=1))
    r = api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 400

def test_bootstrap_rejects_unknown_token(api_client):
    r = api_client.post("/bootstrap", {"provision_token": "nope", "device_id": "aabbccddeeff"}, format="json")
    assert r.status_code == 400

def test_bootstrap_rebind_same_device(api_client):
    raw, user = _seed_pt()
    r1 = api_client.post("/bootstrap", {"provision_token": raw, "device_id": "aabbccddeeff"}, format="json")
    raw2 = generate_token()
    ProvisioningToken.objects.create(user=user, token_hash=hash_token(raw2), expires_at=timezone.now() + timedelta(minutes=10))
    r2 = api_client.post("/bootstrap", {"provision_token": raw2, "device_id": "aabbccddeeff"}, format="json")
    assert r1.data["device_token"] != r2.data["device_token"]
    assert Device.objects.filter(device_id="aabbccddeeff").count() == 1
