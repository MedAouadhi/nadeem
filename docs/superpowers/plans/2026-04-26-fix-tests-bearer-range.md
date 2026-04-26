# Fix tests — remove Bearer rejection test, add new auth + Range tests Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Fix existing tests that reject `Bearer` auth, add comprehensive tests for `Bearer` scheme across all components, and add S3 Range request integration tests.

**Architecture:** Update Django and Channels tests to support both `Device` and `Bearer` authentication keywords. Add integration tests for S3 Range headers using `pytest.mark.skipif` to check for MinIO availability.

**Tech Stack:** Python, Django, pytest, DRF, Channels, S3/MinIO.

---

### Task 1: Update Device Auth Tests

**Files:**
- Modify: `backend/devices/tests/test_auth.py`

- [ ] **Step 1: Remove `test_rejects_bearer_scheme_for_device_auth`**

```python
<<<<
def test_rejects_bearer_scheme_for_device_auth():
    raw = _make()
    rf = APIRequestFactory()
    req = rf.get("/", HTTP_AUTHORIZATION=f"Bearer {raw}")
    assert DeviceTokenAuthentication().authenticate(req) is None
====
>>>>
```

- [ ] **Step 2: Add `test_authenticates_valid_bearer_keyword`**

```python
def test_authenticates_valid_bearer_keyword():
    raw = _make()
    rf = APIRequestFactory()
    req = rf.get("/", HTTP_AUTHORIZATION=f"Bearer {raw}")
    user, dev = DeviceTokenAuthentication().authenticate(req)
    assert dev.device_id == "aabbccddeeff"
```

- [ ] **Step 3: Add `test_bearer_and_device_return_same_device`**

```python
def test_bearer_and_device_return_same_device():
    raw = _make()
    rf = APIRequestFactory()
    
    req_device = rf.get("/", HTTP_AUTHORIZATION=f"Device {raw}")
    _, dev1 = DeviceTokenAuthentication().authenticate(req_device)
    
    req_bearer = rf.get("/", HTTP_AUTHORIZATION=f"Bearer {raw}")
    _, dev2 = DeviceTokenAuthentication().authenticate(req_bearer)
    
    assert dev1 == dev2
```

- [ ] **Step 4: Run tests to verify**

Run: `pytest backend/devices/tests/test_auth.py`
Expected: ALL PASS

- [ ] **Step 5: Commit**

```bash
git add backend/devices/tests/test_auth.py
git commit -m "test: update device auth tests to support Bearer keyword"
```

### Task 2: Update Semsem Manifest Tests

**Files:**
- Modify: `backend/semsems/tests/test_manifest.py`

- [ ] **Step 1: Add `test_manifest_with_bearer_auth`**

```python
def test_manifest_with_bearer_auth(api_client):
    user = get_user_model().objects.create_user(email="bearer@test.com", password="x")
    raw = generate_token()
    Device.objects.create(device_id="bearer123456", user=user, token_hash=hash_token(raw))
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {raw}")
    
    Semsem.objects.create(uid_hex="cc", title="Bearer Test", is_pro=False)
    r = api_client.get("/semsem/cc/manifest")
    assert r.status_code == 200
    assert r.json()["title"] == "Bearer Test"
```

- [ ] **Step 2: Add S3 Range tests**

```python
import requests

def _minio_running():
    try:
        requests.get("http://localhost:9002/minio/health/live", timeout=0.1)
        return True
    except:
        return False

@pytest.mark.skipif(not _minio_running(), reason="MinIO not running (docker-compose up)")
def test_s3_range_request_returns_206(api_client):
    _device(api_client)
    s = Semsem.objects.create(uid_hex="dd", title="Range Test", is_pro=False)
    t = Track(semsem=s, name="range.mp3", position=1)
    
    # We need a real file for a real S3 request
    t.file.save("range.mp3", ContentFile(b"fake mp3 content of some length for range testing"))
    t.save()
    
    r = api_client.get("/semsem/dd/manifest")
    url = r.json()["tracks"][0]["url"]
    
    # Make request to the S3 URL
    res = requests.get(url, headers={"Range": "bytes=5-"})
    assert res.status_code == 206
    assert "Content-Range" in res.headers
    assert res.headers["Accept-Ranges"] == "bytes"
    assert res.content == b"mp3 content of some length for range testing"[5:]

@pytest.mark.skipif(not _minio_running(), reason="MinIO not running (docker-compose up)")
def test_s3_no_range_returns_accept_ranges(api_client):
    _device(api_client)
    s = Semsem.objects.create(uid_hex="ee", title="No Range Test", is_pro=False)
    t = Track(semsem=s, name="norange.mp3", position=1)
    t.file.save("norange.mp3", ContentFile(b"xyz"))
    t.save()
    
    r = api_client.get("/semsem/ee/manifest")
    url = r.json()["tracks"][0]["url"]
    
    res = requests.get(url)
    assert res.status_code == 200
    assert res.headers["Accept-Ranges"] == "bytes"
```

- [ ] **Step 3: Run tests**

Run: `pytest backend/semsems/tests/test_manifest.py`
Note: Range tests will skip if MinIO is not up.

- [ ] **Step 4: Commit**

```bash
git add backend/semsems/tests/test_manifest.py
git commit -m "test: add Bearer auth and S3 Range tests to manifest"
```

### Task 3: Update Stats Upload Tests

**Files:**
- Modify: `backend/stats/tests/test_upload.py`

- [ ] **Step 1: Add `test_stats_upload_with_bearer_auth`**

```python
def test_stats_upload_with_bearer_auth(api_client):
    user = get_user_model().objects.create_user(email="stats-bearer@test.com", password="x")
    raw = generate_token()
    Device.objects.create(device_id="statsbearer12", user=user, token_hash=hash_token(raw))
    api_client.credentials(HTTP_AUTHORIZATION=f"Bearer {raw}")
    
    Semsem.objects.create(uid_hex="ff", title="Stats Bearer", is_pro=False)
    body = {"uid": "ff", "play_count": 1, "total_play_ms": 100,
            "last_played_unix": 12345, "pro_session_count": 0, "pro_total_ms": 0}
    r = api_client.post("/stats", body, format="json")
    assert r.status_code == 200
```

- [ ] **Step 2: Run tests**

Run: `pytest backend/stats/tests/test_upload.py`
Expected: ALL PASS

- [ ] **Step 3: Commit**

```bash
git add backend/stats/tests/test_upload.py
git commit -m "test: add Bearer auth test for stats upload"
```

### Task 4: Update WS Auth Tests

**Files:**
- Modify: `backend/chat/tests/test_ws_auth.py`

- [ ] **Step 1: Add `test_ws_auth_with_bearer_header`**

```python
async def test_ws_auth_with_bearer_header(device_token, monkeypatch):
    monkeypatch.setattr(gemini_client, "GeminiLiveSession", lambda role: _FakeSession())
    url = "/chat?role=doctor&device=aabbccddeeff&semsem=aa"
    # Channels test communicator doesn't easily support custom headers, 
    # but application is wrapped with TokenAuthMiddleware which checks headers.
    # WebsocketCommunicator headers are passed as subprotocols or we can pass headers kwarg if supported.
    
    headers = [(b"authorization", f"Bearer {device_token}".encode())]
    comm = WebsocketCommunicator(application, url, headers=headers)
    connected, _ = await comm.connect()
    assert connected
    await comm.disconnect()
```

- [ ] **Step 2: Run tests**

Run: `pytest backend/chat/tests/test_ws_auth.py`
Expected: ALL PASS

- [ ] **Step 3: Commit**

```bash
git add backend/chat/tests/test_ws_auth.py
git commit -m "test: add Bearer header auth test for WebSockets"
```
