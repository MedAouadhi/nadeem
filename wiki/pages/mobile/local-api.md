# Device-Local HTTP API

> **Type:** Endpoint set
> **Status:** implemented in firmware (`onboarding` component)

Open, unauthenticated HTTP on `http://192.168.4.1:80`. Only available while the device is in onboarding mode and broadcasting `Nadeem-XXXX`. Trust boundary = physical proximity.

## `GET /info`

```json
{ "device_id": "aabbccddeeff", "fw": "0.1.0", "status": "onboarding" }
```

- `device_id`: 12 hex, lowercase (STA MAC).
- `status`: always `"onboarding"` here.

Use this as the "am I on the device AP?" probe.

## `GET /scan`

Returns up to 16 surrounding APs. Blocks ~2 s on a real Wi-Fi scan.

```json
[
  { "ssid": "HomeWifi",    "rssi": -48, "auth": 3 },
  { "ssid": "Neighbour5G", "rssi": -71, "auth": 4 }
]
```

`auth` follows ESP-IDF `wifi_auth_mode_t`:
0 OPEN · 1 WEP · 2 WPA-PSK · 3 WPA2-PSK · 4 WPA/WPA2-PSK · 5 ENT · 6 WPA3-PSK · 7 WPA2/WPA3-PSK · 8 WAPI · 9 OWE.

Repeated calls within 2 s should be coalesced by the app — the device may not lock up but it's wasteful.

## `POST /provision`

```http
Content-Type: application/x-www-form-urlencoded

ssid=HomeWifi&password=hunter2&provision_token=eyJhbGciOi...
```

| Field | Required | Notes |
|-------|----------|-------|
| `ssid` | yes | URL-encoded; `+` decodes to space |
| `password` | no | empty for open |
| `provision_token` | yes | from `POST /api/provisioning-tokens` |

Response 200 `{"ok":true}`, then **device reboots ~500 ms later** — TCP reset is normal, do not retry on it.

400 `missing ssid` if `ssid` absent.

> ⚠ The form does not currently include `backend_url` — known gap in v0.1 (see [../firmware/limitations.md](../firmware/limitations.md)).

## `POST /reset`

Factory reset path from the device's local HTTP. Clears `nadeem` NVS namespace, reboots back into onboarding. Useful to re-link a device that was bound to a different account.

```http
POST /reset
Content-Length: 0
```

Response 200 `{"ok":true}`, then reboots.

---
confidence: 0.9
sources: [S2, S4, S5]
last_confirmed: 2026-04-25
status: implemented
