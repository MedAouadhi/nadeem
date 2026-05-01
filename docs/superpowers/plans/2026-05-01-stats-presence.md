# Stats & Presence Alignment Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Centralize backend presence TTL to 5 minutes, align API field shape with frontend, and ensure `last_seen_at` is updated during authentication.

**Architecture:** Define `DEVICE_ONLINE_TTL` in `Device` model. Use it in the `online` property and all filters for online devices. Update `DeviceSerializer` to use `online` key.

**Tech Stack:** Django, DRF, Pytest

---

### Task 1: Update Tests to reflect new requirements (RED)

**Files:**
- Modify: `backend/devices/tests/test_auth.py`
- Modify: `backend/stats/tests/test_aggregate.py`

- [ ] **Step 1: Add test for last_seen_at update in devices/tests/test_auth.py**

```python
def test_auth_updates_last_seen_at():
    from django.utils import timezone
    from datetime import timedelta
    raw = _make(user_email="update@x.com")
    device = Device.objects.get(device_id="aabbccddeeff")
    # Set to a point in the past
    past = timezone.now() - timedelta(minutes=10)
    Device.objects.filter(pk=device.pk).update(last_seen_at=past)
    
    rf = APIRequestFactory()
    req = rf.get("/", HTTP_AUTHORIZATION=f"Bearer {raw}")
    DeviceTokenAuthentication().authenticate(req)
    
    device.refresh_from_db()
    assert device.last_seen_at > past
    assert (timezone.now() - device.last_seen_at).total_seconds() < 10
```

- [ ] **Step 2: Add 5-minute TTL boundary test in stats/tests/test_aggregate.py**

```python
def test_online_device_count_uses_five_minute_ttl(api_client):
    from datetime import timedelta
    _login(api_client)
    me = get_user_model().objects.get(email="a@b.com")
    now = timezone.now()
    # 4.5 minutes ago -> should be online
    Device.objects.create(device_id="1"*12, user=me, token_hash="1"*64, last_seen_at=now - timedelta(minutes=4, seconds=30))
    # 5.5 minutes ago -> should be offline
    Device.objects.create(device_id="2"*12, user=me, token_hash="2"*64, last_seen_at=now - timedelta(minutes=5, seconds=30))
    
    r = api_client.get("/api/users/me/stats")
    assert r.data["online_device_count"] == 1
```

- [ ] **Step 3: Run tests to verify they fail**

Run: `docker compose run --rm backend pytest -v devices/tests/test_auth.py::test_auth_updates_last_seen_at stats/tests/test_aggregate.py::test_online_device_count_uses_five_minute_ttl`

Expected: 
`test_auth_updates_last_seen_at` should PASS (it's already implemented in auth.py).
`test_online_device_count_uses_five_minute_ttl` should FAIL (currently uses 120s cutoff).

### Task 2: Implement Shared TTL and Model property

**Files:**
- Modify: `backend/devices/models.py`

- [ ] **Step 1: Define DEVICE_ONLINE_TTL and update online property**

```python
from datetime import timedelta
# ...
class Device(models.Model):
    DEVICE_ONLINE_TTL = timedelta(minutes=5)
    # ...
    @property
    def online(self) -> bool:
        if not self.last_seen_at:
            return False
        return (timezone.now() - self.last_seen_at) < self.DEVICE_ONLINE_TTL
```

### Task 3: Align Serializer with Frontend

**Files:**
- Modify: `backend/devices/serializers.py`

- [ ] **Step 1: Rename online_status to online**

```python
class DeviceSerializer(serializers.ModelSerializer):
    online = serializers.BooleanField(source="online", read_only=True)
    # ...
    class Meta:
        model = Device
        fields = ["device_id", "online", "bootstrapped", "last_seen_at", "created_at"]
```

### Task 4: Replace hardcoded TTLs in Views and Dashboard

**Files:**
- Modify: `backend/stats/views.py`
- Modify: `backend/nadeem/dashboard.py`

- [ ] **Step 1: Update stats/views.py to use Device.DEVICE_ONLINE_TTL**

```python
        user_devices = Device.objects.filter(user=request.user)
        online_cutoff = timezone.now() - Device.DEVICE_ONLINE_TTL
        return Response({
            # ...
            "online_device_count": user_devices.filter(last_seen_at__gte=online_cutoff).count(),
        })
```

- [ ] **Step 2: Update nadeem/dashboard.py to use Device.DEVICE_ONLINE_TTL**

```python
    online_threshold = now - Device.DEVICE_ONLINE_TTL
```

### Task 5: Final Verification (GREEN)

- [ ] **Step 1: Run targeted tests**

Run: `docker compose run --rm backend pytest -v devices/tests/test_auth.py::test_auth_updates_last_seen_at stats/tests/test_aggregate.py::test_online_device_count_uses_five_minute_ttl`

Expected: ALL PASS.
