import json
import pytest
from channels.db import database_sync_to_async
from channels.testing import WebsocketCommunicator
from django.contrib.auth import get_user_model
from devices.models import Device
from devices.tokens import generate_token, hash_token
from nadeem.asgi import application
from chat.models import ProChatSession, TranscriptEntry
from chat import gemini_client

pytestmark = [pytest.mark.django_db(transaction=True), pytest.mark.asyncio]


class _FakeSession:
    def __init__(self):
        self.sent = []

    async def __aenter__(self):
        return self

    async def __aexit__(self, *a):
        return False

    async def send_audio(self, b):
        self.sent.append(b)

    async def stream_responses(self):
        yield gemini_client.GeminiEvent("transcript_user", text="hello")
        yield gemini_client.GeminiEvent("audio", audio_pcm16=b"\x10\x00" * 320)
        yield gemini_client.GeminiEvent("transcript_ai", text="hi child")
        yield gemini_client.GeminiEvent("turn_completed")


async def _seed():
    user = await database_sync_to_async(get_user_model().objects.create_user)(email="a@b.com", password="x")
    raw = generate_token()
    await database_sync_to_async(Device.objects.create)(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    return raw


async def test_consumer_bridges_audio_and_persists_transcript(monkeypatch):
    raw = await _seed()
    monkeypatch.setattr(gemini_client, "GeminiLiveSession", lambda role: _FakeSession())
    url = f"/chat?role=doctor&device=aabbccddeeff&semsem=aa&token={raw}"
    comm = WebsocketCommunicator(application, url)
    ok, _ = await comm.connect()
    assert ok
    await comm.send_to(bytes_data=b"\x00" * 640)
    
    # status: listening (initial)
    msg = await comm.receive_from()
    assert json.loads(msg) == {"status": "listening"}

    received_binary = await comm.receive_from()
    assert isinstance(received_binary, bytes)
    
    text_frame = await comm.receive_from()
    assert json.loads(text_frame) == {"status": "speaking"}
    
    status_listening = await comm.receive_from()
    assert json.loads(status_listening) == {"status": "listening"}

    await comm.disconnect()
    sessions = await database_sync_to_async(list)(ProChatSession.objects.all())
    assert len(sessions) == 1 and sessions[0].uid_hex == "aa" and sessions[0].role == "doctor"
    entries = await database_sync_to_async(list)(TranscriptEntry.objects.values_list("speaker", "text"))
    assert ("child", "hello") in entries
    assert ("ai", "hi child") in entries
