import pytest
from django.contrib.auth import get_user_model
from devices.models import Device
from chat.models import ProChatSession, TranscriptEntry

pytestmark = pytest.mark.django_db

def _login(api_client):
    get_user_model().objects.create_user(email="a@b.com", password="pw12345!")
    tok = api_client.post("/api/auth/login", {"email": "a@b.com", "password": "pw12345!"}, format="json").data["access"]
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {tok}")

def test_lists_only_my_sessions(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    other = get_user_model().objects.create_user(email="x@y.com", password="x")
    d_me = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    d_other = Device.objects.create(device_id="b"*12, user=other, token_hash="b"*64)
    ProChatSession.objects.create(device=d_me, uid_hex="aa", role="doctor")
    ProChatSession.objects.create(device=d_other, uid_hex="bb", role="doctor")
    r = api_client.get("/api/chat-sessions")
    assert r.status_code == 200 and len(r.data) == 1

def test_detail_includes_transcript(api_client):
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    d = Device.objects.create(device_id="a"*12, user=me, token_hash="a"*64)
    s = ProChatSession.objects.create(device=d, uid_hex="aa", role="doctor")
    TranscriptEntry.objects.create(session=s, speaker="child", text="hi")
    TranscriptEntry.objects.create(session=s, speaker="ai", text="hello")
    r = api_client.get(f"/api/chat-sessions/{s.id}")
    assert r.status_code == 200
    assert [e["speaker"] for e in r.data["entries"]] == ["child", "ai"]
