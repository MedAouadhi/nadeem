import pytest
from django.contrib.auth import get_user_model
from django.test import Client

from semsems.models import Semsem, SemsemGroup

User = get_user_model()


@pytest.fixture
def superuser(db):
    return User.objects.create_superuser(email="admin@nadeem.com", password="adminpass")


@pytest.fixture
def admin_client(superuser):
    c = Client()
    c.force_login(superuser)
    return c


@pytest.mark.django_db
def test_semsem_group_creation():
    group = SemsemGroup.objects.create(name="Animals", description="Animal characters", icon="🐻")
    assert group.name == "Animals"
    assert group.icon == "🐻"
    assert str(group) == "Animals"


@pytest.mark.django_db
def test_semsem_belongs_to_group():
    group = SemsemGroup.objects.create(name="Professions", icon="👩‍⚕️")
    semsem = Semsem.objects.create(
        uid_hex="04a3f91b2c", title="Dr. Salim", is_pro=True, role="doctor"
    )
    semsem.groups.add(group)
    assert group in semsem.groups.all()
    assert semsem in group.semsems.all()


@pytest.mark.django_db
def test_semsem_can_have_multiple_groups():
    g1 = SemsemGroup.objects.create(name="Animals", icon="🐻")
    g2 = SemsemGroup.objects.create(name="Favorites", icon="⭐")
    semsem = Semsem.objects.create(uid_hex="04a3f91b2d", title="Polar Bear", is_pro=False)
    semsem.groups.add(g1, g2)
    assert semsem.groups.count() == 2


@pytest.mark.django_db
def test_semsem_group_list_admin(admin_client):
    response = admin_client.get("/admin/semsems/semsemgroup/")
    assert response.status_code == 200


@pytest.mark.django_db
def test_semsem_list_admin(admin_client):
    response = admin_client.get("/admin/semsems/semsem/")
    assert response.status_code == 200
