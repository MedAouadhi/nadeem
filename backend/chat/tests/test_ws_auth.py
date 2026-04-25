import pytest
from channels.db import database_sync_to_async
from channels.testing import WebsocketCommunicator
from django.contrib.auth import get_user_model

from chat import gemini_client
from devices.models import Device
from devices.tokens import generate_token, hash_token
from nadeem.asgi import application

pytestmark = [pytest.mark.django_db(transaction=True), pytest.mark.asyncio]


class _FakeSession:
    async def __aenter__(self):
        return self

    async def __aexit__(self, *a):
        return False

    async def send_audio(self, b):
        pass

    async def stream_responses(self):
        return
        yield


@pytest.fixture
async def device_token():
    user = await database_sync_to_async(get_user_model().objects.create_user)(email="a@b.com", password="x")
    raw = generate_token()
    await database_sync_to_async(Device.objects.create)(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    return raw


async def test_ws_rejects_without_token():
    comm = WebsocketCommunicator(application, "/chat?role=doctor&device=aabbccddeeff&semsem=aa")
    connected, _ = await comm.connect()
    assert not connected


async def test_ws_accepts_with_query_token(device_token, monkeypatch):
    monkeypatch.setattr(gemini_client, "GeminiLiveSession", lambda role: _FakeSession())
    url = f"/chat?role=doctor&device=aabbccddeeff&semsem=aa&token={device_token}"
    comm = WebsocketCommunicator(application, url)
    connected, _ = await comm.connect()
    assert connected
    await comm.disconnect()


async def test_ws_rejects_invalid_uid_hex(device_token, monkeypatch):
    monkeypatch.setattr(gemini_client, "GeminiLiveSession", lambda role: _FakeSession())
    url = f"/chat?role=doctor&device=aabbccddeeff&semsem=ZZZZ&token={device_token}"
    comm = WebsocketCommunicator(application, url)
    connected, _ = await comm.connect()
    assert not connected


async def test_ws_rejects_oversized_role(device_token, monkeypatch):
    monkeypatch.setattr(gemini_client, "GeminiLiveSession", lambda role: _FakeSession())
    long_role = "x" * 32
    url = f"/chat?role={long_role}&device=aabbccddeeff&semsem=aa&token={device_token}"
    comm = WebsocketCommunicator(application, url)
    connected, _ = await comm.connect()
    assert not connected
