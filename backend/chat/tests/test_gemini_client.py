import pytest
from chat.gemini_client import GeminiLiveSession, GeminiEvent

pytestmark = pytest.mark.asyncio


class _FakeSDKSession:
    def __init__(self):
        self.sent = []

    async def send_realtime_input(self, audio):
        self.sent.append(audio)

    async def receive(self):
        yield {"server_content": {"input_transcription": {"text": "hello"}}}
        yield {"server_content": {"model_turn": {"parts": [{"inline_data": {"data": b"\x00\x01", "mime_type": "audio/pcm;rate=16000"}}]}}}
        yield {"server_content": {"turn_complete": True}}


@pytest.fixture
def fake_sdk(monkeypatch):
    sess = _FakeSDKSession()

    class _LiveContext:
        async def __aenter__(self_):
            return sess
        async def __aexit__(self_, *a):
            return False

    class _Live:
        def connect(self, **_):
            return _LiveContext()

    class _Client:
        def __init__(self, **_):
            self.aio = type("X", (), {"live": _Live()})()

    monkeypatch.setattr("chat.gemini_client._build_client", lambda: _Client())
    return sess


async def test_session_yields_events(fake_sdk, settings):
    settings.GEMINI_API_KEY = "k"
    async with GeminiLiveSession(role="doctor") as s:
        await s.send_audio(b"\x00" * 640)
        events = []
        async for e in s.stream_responses():
            events.append(e)
        kinds = [e.kind for e in events]
        assert "transcript_user" in kinds
        assert "audio" in kinds
        assert "turn_completed" in kinds
        assert fake_sdk.sent
