# Onboarding (firmware side)

> **Type:** Component
> **Status:** implemented

## What

Brought up at boot when no Wi-Fi creds are stored in NVS. Runs a SoftAP + HTTP server so the companion app can hand the device Wi-Fi credentials and a `provision_token`.

## SoftAP

| Property | Value |
|----------|-------|
| SSID | `Nadeem-XXXX` (last 4 hex of SoftAP MAC) |
| Auth | open |
| Channel | 1 |
| Max clients | 4 |
| Device IP | `192.168.4.1` |
| DHCP | server runs on device |

LEDs: `LED_ONBOARDING` (blue pulse) while active.

## HTTP endpoints

All on port 80, no auth.

### `GET /info`

```json
{ "device_id": "aabbccddeeff", "fw": "0.1.0", "status": "onboarding" }
```

### `GET /scan`

Triggers a real Wi-Fi scan; up to 16 entries; takes ~2 s.

```json
[ { "ssid": "HomeWifi", "rssi": -48, "auth": 3 }, ... ]
```

`auth` follows ESP-IDF `wifi_auth_mode_t`: 0 OPEN, 1 WEP, 2 WPA-PSK, 3 WPA2-PSK, 4 WPA/WPA2-PSK, 5 ENT, 6 WPA3-PSK, 7 WPA2/WPA3-PSK, 8 WAPI, 9 OWE.

### `POST /provision`

```
Content-Type: application/x-www-form-urlencoded
ssid=HomeWifi&password=hunter2&provision_token=eyJhbGciOi...
```

Parser: `onboarding_parse_form` (host-testable). Decodes `%XX` and `+`→space. Required: `ssid`, `provision_token`. Optional: `password` (empty for open). Missing `ssid` → `400 missing ssid`.

On success: persists to NVS, returns `{"ok":true}`, **reboots ~500 ms later**. App must not retry on TCP reset.

### `POST /reset`

Clears `nadeem` NVS namespace; reboots into onboarding. Useful for re-binding to a different account.

## NVS keys written

| Namespace | Key | Source |
|-----------|-----|--------|
| `nadeem` | `wifi_ssid` | form |
| `nadeem` | `wifi_pass` | form |
| `nadeem` | `prov_token` | form (cleared after `/bootstrap` succeeds in `on_wifi_up`) |
| `nadeem` | `backend_url` | currently set out-of-band; companion app should provide via future endpoint |

> ⚠ **Gap:** The form does not currently include `backend_url`. v0.1 expects it pre-baked or set via `console_cmds`. Mobile/backend integration should add this.

## Dependencies

→ [pages/mobile/local-api.md](../mobile/local-api.md) — same surface from app's view
→ [pages/system/workflows.md](../system/workflows.md) §W1

---
confidence: 0.9
sources: [S2, S4, S5]
last_confirmed: 2026-04-24
status: implemented
