import pytest
from django.contrib.auth import get_user_model
from devices.models import Device
from chat.models import ProChatSession, TranscriptEntry

pytestmark = pytest.mark.django_db

def test_session_lifecycle():
    user = get_user_model().objects.create_user(email="a@b.com", password="x")
    d = Device.objects.create(device_id="a"*12, user=user, token_hash="a"*64)
    s = ProChatSession.objects.create(device=d, uid_hex="bb", role="doctor")
    assert s.ended_at is None
    TranscriptEntry.objects.create(session=s, speaker="child", text="hello")
    TranscriptEntry.objects.create(session=s, speaker="ai", text="hi!")
    assert s.entries.count() == 2
    assert list(s.entries.values_list("speaker", flat=True)) == ["child", "ai"]
