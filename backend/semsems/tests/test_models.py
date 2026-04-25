import pytest
from django.core.exceptions import ValidationError
from semsems.models import Semsem, Track

pytestmark = pytest.mark.django_db


def test_uid_hex_lowercase_2_to_20():
    Semsem.objects.create(uid_hex="04a3f91b2c", title="ok", is_pro=False)
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="04A3", title="bad", is_pro=False)
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="zz", title="bad", is_pro=False)
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="0"*22, title="too long", is_pro=False)


def test_pro_requires_role():
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="01", title="x", is_pro=True, role="")


def test_role_max_31_bytes():
    with pytest.raises((ValidationError, Exception)):
        Semsem.objects.create(uid_hex="02", title="x", is_pro=True, role="a"*32)


def test_track_ordering():
    s = Semsem.objects.create(uid_hex="03", title="x", is_pro=False)
    Track.objects.create(semsem=s, name="b.mp3", file="audio/b.mp3", position=2)
    Track.objects.create(semsem=s, name="a.mp3", file="audio/a.mp3", position=1)
    assert [t.name for t in s.tracks.order_by("position")] == ["a.mp3", "b.mp3"]
