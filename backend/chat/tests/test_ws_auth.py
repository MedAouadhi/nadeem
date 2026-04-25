import pytest
from channels.testing import WebsocketCommunicator
from django.contrib.auth import get_user_model
from devices.models import Device
from devices.tokens import generate_token, hash_token
from nadeem.asgi import application

pytestmark = [pytest.mark.django_db, pytest.mark.asyncio]

@pytest.fixture
async def device_token(db):
    from asgiref.sync import sync_to_async
    user = await sync_to_async(get_user_model().objects.create_user)(email="a@b.com", password="x")
    raw = generate_token()
    await sync_to_async(Device.objects.create)(device_id="aabbccddeeff", user=user, token_hash=hash_token(raw))
    return raw

async def test_ws_rejects_without_token():
    comm = WebsocketCommunicator(application, "/chat?role=doctor&device=aabbccddeeff&semsem=aa")
    connected, _ = await comm.connect()
    assert not connected

async def test_ws_accepts_with_query_token(device_token):
    url = f"/chat?role=doctor&device=aabbccddeeff&semsem=aa&token={device_token}"
    comm = WebsocketCommunicator(application, url)
    connected, _ = await comm.connect()
    assert connected
    await comm.disconnect()
