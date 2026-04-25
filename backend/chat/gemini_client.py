from __future__ import annotations

from collections.abc import AsyncIterator
from dataclasses import dataclass
from typing import Literal

from django.conf import settings


def _build_client():
    from google import genai
    return genai.Client(api_key=settings.GEMINI_API_KEY)


SYSTEM_PROMPTS = {
    "doctor": "You are a friendly children's doctor companion. Use simple Arabic. Never give medical advice; suggest asking a parent.",
    "engineer": "You are a friendly children's engineer companion. Speak simple Arabic; explain how things work using everyday analogies.",
    "storyteller": "You are a children's storyteller. Speak simple Arabic; tell short, kind stories.",
}


@dataclass
class GeminiEvent:
    kind: Literal["audio", "transcript_user", "transcript_ai", "turn_started", "turn_completed"]
    audio_pcm16: bytes | None = None
    text: str | None = None


class GeminiLiveSession:
    def __init__(self, role: str):
        self.role = role
        self._client = None
        self._ctx = None
        self._sdk = None

    async def __aenter__(self):
        self._client = _build_client()
        config = {
            "response_modalities": ["AUDIO"],
            "system_instruction": SYSTEM_PROMPTS.get(self.role, SYSTEM_PROMPTS["storyteller"]),
            "input_audio_transcription": {},
            "output_audio_transcription": {},
        }
        self._ctx = self._client.aio.live.connect(model=settings.GEMINI_MODEL, config=config)
        self._sdk = await self._ctx.__aenter__()
        return self

    async def __aexit__(self, exc_type, exc, tb):
        await self._ctx.__aexit__(exc_type, exc, tb)

    async def send_audio(self, pcm16_le_16khz: bytes) -> None:
        await self._sdk.send_realtime_input(audio={"data": pcm16_le_16khz, "mime_type": "audio/pcm;rate=16000"})

    async def stream_responses(self) -> AsyncIterator[GeminiEvent]:
        async for msg in self._sdk.receive():
            sc = (msg or {}).get("server_content") if isinstance(msg, dict) else None
            if not sc:
                sc = getattr(msg, "server_content", None)
            if not sc:
                continue
            if isinstance(sc, dict):
                if (it := sc.get("input_transcription")):
                    yield GeminiEvent("transcript_user", text=it.get("text") or "")
                if (ot := sc.get("output_transcription")):
                    yield GeminiEvent("transcript_ai", text=ot.get("text") or "")
                if (mt := sc.get("model_turn")):
                    for part in mt.get("parts", []):
                        if (inline := part.get("inline_data")):
                            yield GeminiEvent("audio", audio_pcm16=inline["data"])
                if sc.get("turn_complete"):
                    yield GeminiEvent("turn_completed")
            else:
                if getattr(sc, "input_transcription", None):
                    yield GeminiEvent("transcript_user", text=sc.input_transcription.text or "")
                if getattr(sc, "output_transcription", None):
                    yield GeminiEvent("transcript_ai", text=sc.output_transcription.text or "")
                mt = getattr(sc, "model_turn", None)
                if mt:
                    for part in getattr(mt, "parts", []) or []:
                        inline = getattr(part, "inline_data", None)
                        if inline:
                            yield GeminiEvent("audio", audio_pcm16=inline.data)
                if getattr(sc, "turn_complete", False):
                    yield GeminiEvent("turn_completed")
