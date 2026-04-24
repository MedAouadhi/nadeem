# Mobile Onboarding Flow

> **Type:** Workflow
> **Status:** spec-only

## Happy path

1. Family powers on a fresh Nadeem. LEDs pulse blue (`LED_ONBOARDING`).
2. Device advertises `Nadeem-XXXX` open AP.
3. App (already logged into the user's account) requests `POST /api/provisioning-tokens` from the backend → `{provision_token, expires_at}`.
4. App prompts user to join `Nadeem-XXXX`. iOS: `NEHotspotConfiguration`. Android: `WifiNetworkSpecifier` with prefix match.
5. App probes `GET http://192.168.4.1/info` → confirms it's the device.
6. App calls `GET /scan`, shows nearby networks, user picks SSID + types password.
7. App `POST /provision` (form-encoded `ssid`, `password`, `provision_token`).
8. Device replies `{ok:true}` and reboots ~500 ms later. TCP reset is expected.
9. App polls backend (e.g. `GET /api/devices/<device_id>`) until backend reports the device "bootstrapped" (typical 10–30 s).
10. App shows "Nadeem <name> is online".

## Failure modes

| Symptom | Most likely cause | Recovery |
|---------|-------------------|----------|
| No `Nadeem-XXXX` AP visible | Device already has Wi-Fi creds, or boot failure | Long-press PLAY/PAUSE ≥ 3 s on device |
| `/provision` 400 "missing ssid" | App didn't form-encode field | Validate form before send |
| Device never appears on backend within 60 s | Wrong Wi-Fi password (most common) | Long-press PLAY/PAUSE; user retries |
| Device boots but never bootstraps (`device_token` missing on backend) | `provision_token` rejected (expired, replayed) | App shows "re-link"; mints fresh token; user re-runs flow |
| `/scan` returns empty | Radio busy or no APs in range | Retry once after 2 s |

## App test checklist (per S4 §7)

- [ ] `GET /info` returns 200 and parses on `Nadeem-XXXX`.
- [ ] `GET /scan` shows the user's network with sane RSSI / auth.
- [ ] `POST /provision` succeeds; AP disappears within 5 s.
- [ ] Backend reports device online within 60 s.
- [ ] `POST /reset` brings the device back to onboarding.
- [ ] Special chars: SSID with `&`, `=`, spaces, non-ASCII (UTF-8, URL-encoded).
- [ ] App tolerates dropped TCP after `/provision`.
- [ ] Repeated `/scan` within 2 s doesn't lock up the device.

---
confidence: 0.85
sources: [S4]
last_confirmed: 2026-04-25
status: spec-only
