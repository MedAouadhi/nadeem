# Backend Security

> **Type:** Constraint
> **Status:** spec-only

Per S3 §9 and S7.

## Transport

- **TLS mandatory in production.** Firmware uses `esp_http_client` and `esp_websocket_client`; both support TLS via mbedTLS. v0.1 firmware does not yet attach a cert bundle (open issue — see [../firmware/limitations.md](../firmware/limitations.md)).
- WebSocket: `wss://` only in production.

## Tokens

| Token | TTL | Scope | Revocation |
|-------|-----|-------|------------|
| `provision_token` | ~10 min | one-shot bind device → user | invalidated on first `/bootstrap` |
| `device_token` | long-lived | per-device, per-user | `DELETE /api/devices/<id>` |
| user JWT / session | per-app policy | user app/web | logout / expiry |

Rules:
- `provision_token` MUST be invalidated after a successful exchange. Reject stale ones.
- `device_token` SHOULD be long-lived (firmware caches it in NVS) but MUST be revocable. After revocation, the firmware degrades to offline + cached playback until factory reset + re-bootstrap.
- Never log tokens in plaintext.

## Rate limits

- `POST /bootstrap` — should never be high-volume (once per device ever); rate-limit to prevent DoS by a lost device.
- `POST /stats` — at most ~one per UID per 5 min per device. Reject obvious abuse.
- `WS /chat` — limit concurrent sessions per device; cap session duration.

## CORS

- Device-side endpoints: not relevant (no browser origin).
- App-side endpoints (`/api/*`): set explicit allow-list for the web frontend origin.

## Sensitive data

- `WS /chat` carries **live mic from a child's bedroom**. TLS-only. Authenticate the upgrade. Log sparingly (no payload).
- Chat transcripts are PII-adjacent — define retention + parental-deletion path (open question; flagged in [../../lint.md](../../lint.md)).

## Tibber-style guardrails (org policy)

- No secrets in the repo.
- No real customer audio in wiki examples — use synthetic examples only.
- Personal-data handling for transcripts must follow org privacy review before any non-approved processor is added (e.g. ASR/TTS vendors).

---
confidence: 0.7
sources: [S3, S7, S5]
last_confirmed: 2026-04-24
status: spec-only
