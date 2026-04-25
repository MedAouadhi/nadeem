# Nadeem Backend — Task Checklist

Progress tracker for the 21-task backend implementation plan
(`docs/superpowers/plans/2026-04-25-nadeem-backend.md`).
Branch: `feat/backend-v0.1`.

Legend: `[x]` done · `[~]` in progress · `[ ]` pending.

---

- [x] **1. Repo scaffold + Docker + Django project**
  - 46 files: `pyproject.toml`, `Dockerfile`, `docker-compose.yml`
    (postgres / redis / minio + bootstrap / backend), `.env.example`,
    `.gitignore`, `manage.py`, `pytest.ini`, `conftest.py`,
    `nadeem/{settings,urls,asgi,wsgi}.py`, 5 empty apps
    (`accounts`, `devices`, `semsems`, `stats`, `chat`) each with
    `apps.py`, `urls.py`, `models.py`, `admin.py`, `tests/`,
    placeholder `chat/routing.py` + `chat/auth.py`.
  - JWT auth wired via SimpleJWT; S3 storage points at MinIO;
    Channels routing through `DeviceTokenAuthMiddleware` placeholder.
  - Commits: `e1fb317` (scaffold) + `1f4eef1` (PEP 517 build-system).
  - Boot check skipped (port conflict with another local MinIO,
    not a code defect).

- [x] **2. Custom User model**
  - Email-based `AbstractBaseUser` + `UserManager` in
    `accounts/models.py`. Admin registration. Initial migration.
  - TDD: `test_user_uses_email_as_username`,
    `test_email_is_unique`.

- [x] **3. JWT login + `/api/auth/me`**
  - `EmailTokenObtainPairSerializer`, `LoginView`,
    `TokenRefreshView`, `MeView`. Tests for token pair, wrong
    password, authed `/me`, unauth `/me`.

- [ ] **4. Token gen + sha256 hashing**
  - `devices/tokens.py`: `generate_token` (`secrets.token_urlsafe`)
    and `hash_token` (sha256). Tests for url-safety + determinism.

- [ ] **5. Device + ProvisioningToken models**
  - `Device` (12-hex `device_id` unique, `user` FK, `token_hash`,
    `last_seen_at`, `online` property).
  - `ProvisioningToken` (user FK, `token_hash` unique, `expires_at`,
    `used_at`, `is_expired` / `is_used`).
  - Validators reject uppercase / non-hex / wrong length. Migration.

- [ ] **6. `DeviceTokenAuthentication` (DRF)**
  - DRF auth class: read `Authorization: Bearer <raw>`,
    look up `Device` by `hash_token(raw)`, return `(user, device)`,
    update `last_seen_at`. Tests: valid / missing / unknown.

- [ ] **7. `POST /api/provisioning-tokens`**
  - JWT-authed parent endpoint. Generate raw token, store sha256
    hash, 10-min TTL, return `{provision_token, expires_at}`.
    401 unauth.

- [ ] **8. `POST /bootstrap` (firmware)**
  - No-auth. Validate token (lookup-by-hash, not used,
    not expired). Validate `device_id` 12 hex. Create / update
    `Device` with new `device_token` (hashed). Mark token used.
    Tests: success, mark-used, reuse-rejected, expired-rejected,
    unknown-rejected, rebind-replaces-token.

- [ ] **9. Dev provisioning helper**
  - `python manage.py provision_dev_device --email --device-id`.
  - `POST /api/dev/devices` gated by
    `settings.DEV_PROVISIONING_ENABLED` (404 when off).
  - README documents usage + removal steps.

- [ ] **10. `GET` / `DELETE /api/devices`**
  - List current user's devices only.
  - `DELETE /api/devices/<device_id>` revokes by deleting.
  - Cross-user access → 404.

- [ ] **11. Semsem + Track models**
  - `Semsem` (`uid_hex` 2..20 lower hex unique, `title`, `is_pro`,
    `role` ≤ 31 bytes).
  - `Track` (`semsem` FK, `name`, `file` FileField, `position`,
    ordering).
  - Validation: pro requires role; regular forbids role; role byte
    length. Admin with `TrackInline`.

- [ ] **12. `GET /semsem/<uid_hex>/manifest`**
  - Device-auth firmware endpoint. Return
    `{uid, title, pro, role, tracks:[{name, url}]}`. URLs built from
    `S3_PUBLIC_ENDPOINT_URL` + bucket + `file.name`. Lowercase
    normalisation. 404 unknown. 401 unauth.

- [ ] **13. `POST /stats` (firmware)**
  - `UsageStats` (device FK, `uid_hex`, `play_count`,
    `total_play_ms`, `last_played_unix`, `pro_session_count`,
    `pro_total_ms`, unique `(device, uid_hex)`).
  - Cumulative-snapshot via `update_or_create`.
  - Device-auth, 401 unauth.

- [ ] **14. `GET /api/users/me/stats` aggregator**
  - JWT-authed dashboard aggregator. Return
    `{total_listening_ms, unique_semsems, pro_total_ms,
    device_count, online_device_count}`. Aggregate only the
    current user's devices.

- [ ] **15. Web semsem list + detail**
  - `GET /api/semsems` (bound = user has a `UsageStats` row for the
    uid).
  - `GET /api/semsems/<uid>` with aggregated counters + tracks.
  - 404 if not bound.

- [ ] **16. `ProChatSession` + `TranscriptEntry` models**
  - `ProChatSession` (device FK, `uid_hex`, `role`, `started_at`,
    `ended_at`).
  - `TranscriptEntry` (session FK, `speaker` `child` | `ai`, `text`,
    `timestamp`).
  - Admin with read-only inline.

- [ ] **17. Channels device-token middleware**
  - `chat/auth.py` `DeviceTokenAuthMiddleware`: extract `?token=` or
    `Authorization` header, look up `Device` by hash, attach
    `scope["device"]`. Placeholder `ChatConsumer` rejects unauth
    (4401), accepts otherwise. `routing.py` exposes `/chat`.

- [ ] **18. Gemini Live client wrapper**
  - `chat/gemini_client.py`: `GeminiLiveSession` async ctx mgr
    around `google-genai live.connect(model=settings.GEMINI_MODEL)`.
  - `send_audio(pcm16_le_16khz)`, `stream_responses()` yielding
    `GeminiEvent(audio | transcript_user | transcript_ai |
    turn_completed)`. Mockable via `_build_client`.

- [ ] **19. WebSocket `/chat` consumer**
  - Replace placeholder. Bridge: client PCM16 binary → Gemini,
    Gemini audio → client binary, transcripts → DB rows, status
    text frames `{"status":"listening" | "speaking"}`.
  - Create `ProChatSession` on connect; stamp `ended_at` on
    disconnect.

- [ ] **20. Chat sessions web API**
  - `GET /api/chat-sessions` (current user's sessions).
  - `GET /api/chat-sessions/<id>` with full transcript entries.
  - JWT auth.

- [ ] **21. Production hardening + smoke**
  - `DEBUG=false` guards: HSTS, secure cookies, SSL redirect,
    proxy header.
  - README: run / test / dev-bind / prod checklist.
  - Makefile.
  - Full pytest suite passes.
  - End-to-end smoke: `createsuperuser` →
    `provision_dev_device` → `curl manifest`.
