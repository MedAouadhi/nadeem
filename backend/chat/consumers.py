import asyncio
import json
import re
from urllib.parse import parse_qs
from channels.db import database_sync_to_async
from channels.generic.websocket import AsyncWebsocketConsumer
from django.utils import timezone

from . import gemini_client
from .models import ProChatSession, TranscriptEntry

_UID_HEX_RE = re.compile(r"^[0-9a-f]{2,20}$")


GEMINI_CLEANUP_TIMEOUT = 5.0


class ChatConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        device = self.scope.get("device")
        if not device:
            await self.close(code=4401)
            return
        qs = parse_qs(self.scope.get("query_string", b"").decode())
        self.role = (qs.get("role") or [""])[0]
        self.uid_hex = (qs.get("semsem") or [""])[0].lower()
        if not self.role or not self.uid_hex:
            await self.close(code=4400)
            return
        if not _UID_HEX_RE.match(self.uid_hex):
            await self.close(code=4400)
            return
        if len(self.role.encode("utf-8")) > 31:
            await self.close(code=4400)
            return
        self.device = device
        self.session = await self._create_session()
        await self.accept()
        self._gemini_cm = gemini_client.GeminiLiveSession(role=self.role)
        self._gemini = await self._gemini_cm.__aenter__()
        self._reader = asyncio.create_task(self._pump_gemini_to_ws())
        await self.send(text_data=json.dumps({"status": "listening"}))

    async def disconnect(self, code):
        if hasattr(self, "_reader"):
            self._reader.cancel()
        if hasattr(self, "_gemini_cm"):
            try:
                await asyncio.wait_for(
                    self._gemini_cm.__aexit__(None, None, None),
                    timeout=GEMINI_CLEANUP_TIMEOUT,
                )
            except Exception:
                pass
        if hasattr(self, "session") and self.session:
            await self._end_session()

    async def receive(self, text_data=None, bytes_data=None):
        if bytes_data is not None:
            await self._gemini.send_audio(bytes_data)

    async def _pump_gemini_to_ws(self):
        try:
            async for ev in self._gemini.stream_responses():
                if ev.kind == "audio" and ev.audio_pcm16:
                    await self.send(bytes_data=ev.audio_pcm16)
                    await self.send(text_data=json.dumps({"status": "speaking"}))
                elif ev.kind == "transcript_user" and ev.text:
                    await self._add_entry("child", ev.text)
                elif ev.kind == "transcript_ai" and ev.text:
                    await self._add_entry("ai", ev.text)
                elif ev.kind == "turn_completed":
                    await self.send(text_data=json.dumps({"status": "listening"}))
        except asyncio.CancelledError:
            raise
        except Exception:
            try:
                await self.send(text_data=json.dumps({"status": "error"}))
            except Exception:
                pass
            await self.close(code=4500)

    @database_sync_to_async
    def _create_session(self):
        return ProChatSession.objects.create(device=self.device, uid_hex=self.uid_hex, role=self.role)

    @database_sync_to_async
    def _end_session(self):
        self.session.ended_at = timezone.now()
        self.session.save(update_fields=["ended_at"])

    @database_sync_to_async
    def _add_entry(self, speaker, text):
        TranscriptEntry.objects.create(session=self.session, speaker=speaker, text=text)
