import pytest
from django.contrib.auth import get_user_model

pytestmark = pytest.mark.django_db


def test_user_uses_email_as_username():
    User = get_user_model()
    u = User.objects.create_user(email="parent@example.com", password="hunter2")
    assert u.email == "parent@example.com"
    assert u.check_password("hunter2")
    assert str(u) == "parent@example.com"
    assert u.USERNAME_FIELD == "email"


def test_email_is_unique():
    User = get_user_model()
    User.objects.create_user(email="a@b.com", password="x")
    with pytest.raises(Exception):
        User.objects.create_user(email="a@b.com", password="x")
