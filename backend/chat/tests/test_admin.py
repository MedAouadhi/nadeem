import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from chat.models import ProChatSession
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


@pytest.fixture
def session(superuser):
    device = Device.objects.create(
        device_id="aabbccddeef3", user=superuser, token_hash="c" * 64
    )
    return ProChatSession.objects.create(device=device, uid_hex="04a3f91b30", role="doctor")


@pytest.mark.django_db
def test_chat_session_flagging_defaults(session):
    assert session.flagged is False
    assert session.flag_reason == ""


@pytest.mark.django_db
def test_chat_session_can_be_flagged(session):
    session.flagged = True
    session.flag_reason = "Inappropriate content"
    session.save()
    session.refresh_from_db()
    assert session.flagged is True
    assert session.flag_reason == "Inappropriate content"


@pytest.mark.django_db
def test_chat_session_list_admin(admin_client):
    response = admin_client.get("/admin/chat/prochatsession/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_flag_session_action(admin_client, session):
    response = admin_client.post(
        "/admin/chat/prochatsession/",
        {"action": "flag_sessions", "_selected_action": [session.pk]},
    )
    assert response.status_code in (200, 302)
    session.refresh_from_db()
    assert session.flagged is True
