from urllib.parse import parse_qs

from channels.db import database_sync_to_async

from devices.models import Device
from devices.tokens import hash_token


@database_sync_to_async
def _get_device(token: str):
    try:
        return Device.objects.select_related("user").get(token_hash=hash_token(token))
    except Device.DoesNotExist:
        return None

def _extract_token(scope) -> str | None:
    qs = parse_qs(scope.get("query_string", b"").decode())
    if "token" in qs:
        return qs["token"][0]
    for name, value in scope.get("headers", []):
        if name == b"authorization":
            v = value.decode()
            vl = v.lower()
            # Accept both `Bearer <token>` (firmware) and `Device <token>`
            # (legacy). Case-insensitive keyword matching.
            if vl.startswith("bearer "):
                return v[7:].strip()
            if vl.startswith("device "):
                return v[7:].strip()
    return None

class DeviceTokenAuthMiddleware:
    def __init__(self, inner):
        self.inner = inner

    async def __call__(self, scope, receive, send):
        token = _extract_token(scope)
        scope["device"] = await _get_device(token) if token else None
        return await self.inner(scope, receive, send)
