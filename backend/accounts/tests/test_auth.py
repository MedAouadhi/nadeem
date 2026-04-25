import pytest
from django.contrib.auth import get_user_model

pytestmark = pytest.mark.django_db


def test_login_returns_jwt_pair(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    r = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json")
    assert r.status_code == 200
    assert "access" in r.data and "refresh" in r.data


def test_login_rejects_wrong_password(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    r = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "wrong"}, format="json")
    assert r.status_code == 401


def test_me_returns_authenticated_user(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")
    r = api_client.get("/api/auth/me")
    assert r.status_code == 200 and r.data["email"] == "a@b.com"


def test_me_unauth_is_401(api_client):
    assert api_client.get("/api/auth/me").status_code == 401
