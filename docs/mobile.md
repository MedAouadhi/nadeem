# Nadeem Companion App — Integration Spec

This document is everything a mobile developer needs to build the Nadeem
companion app. The app's single responsibility is **onboarding** a brand-new
Nadeem device: join it to the family's Wi-Fi and hand it a one-time
`provision_token` that the device will exchange with the Nadeem backend for
a long-lived `device_token`.

Once provisioning succeeds the device reboots into normal mode and the app
has no further direct communication with it — all subsequent management (stats,
parental control, catalog) happens through the Nadeem backend.

---

## 1. User flow (happy path)

1. Family powers on the Nadeem for the first time.
2. Device has no Wi-Fi creds → enters onboarding mode; its LEDs pulse blue.
3. Device advertises an open Wi-Fi AP named `Nadeem-XXXX` (where `XXXX` is the
   last 4 hex digits of its SoftAP MAC).
4. User opens the companion app, which is already logged into the user's
   Nadeem account. The app:
   - Asks the user to join `Nadeem-XXXX` (iOS: deep-link to Wi-Fi settings;
     Android: programmatic join is allowed with `WifiNetworkSpecifier`).
   - Once joined, talks to `http://192.168.4.1`.
5. App requests a fresh `provision_token` from **its own backend session**
   (see `backend.md §Provisioning`). That token identifies which user/account
   the device should belong to.
6. App calls `GET /scan` on the device, shows the user a list of nearby
   networks, and has the user pick one + enter its password.
7. App POSTs `ssid`, `password`, and `provision_token` to `POST /provision`.
8. Device responds `{"ok":true}`, then reboots within ~500 ms.
9. The Nadeem-XXXX AP disappears. The app waits for its backend to report that
   the device completed bootstrap (polling `GET /devices/<device_id>` works;
   see `backend.md`). Typical wall time: 10–30 s.
10. App shows "Nadeem <name> is online".

---

## 2. SoftAP characteristics

| Property        | Value                                           |
|-----------------|-------------------------------------------------|
| SSID pattern    | `Nadeem-XXXX` (last 4 hex of SoftAP MAC)        |
| Auth            | Open (no password)                              |
| Channel         | 1                                               |
| Max clients     | 4                                               |
| Device IP       | `192.168.4.1`                                   |
| DHCP            | The device runs a DHCP server; app just requests an address in the standard way |
| Captive portal  | Not currently provided — the app is responsible for opening the right URL |

The app should **not** rely on mDNS during onboarding; the device doesn't
advertise mDNS until after it joins the user's LAN.

### Detecting the right AP

- iOS ≥14: use the `NEHotspotConfiguration` API to join a known-SSID.
- Android 10+: use `WifiNetworkSpecifier.Builder().setSsidPattern(...)` with a
  pattern match on `Nadeem-` prefix, or scan results filtered by the same.
- Detect "am I on the device AP?" by checking that `http://192.168.4.1/info`
  returns a JSON with `"fw"` before considering the user provisionable.

---

## 3. Local HTTP API (device ⇄ app)

All endpoints are on `http://192.168.4.1:80`. They are open (no auth) because
they only exist while the device is broadcasting its own SoftAP that only
someone physically nearby can join.

### `GET /info`

Health/identity check. Use this to confirm you're on the device's AP.

```http
GET /info HTTP/1.1
Host: 192.168.4.1
```

Response `200 OK`, `Content-Type: application/json`:

```json
{
  "device_id": "aabbccddeeff",
  "fw": "0.1.0",
  "status": "onboarding"
}
```

- `device_id` is the lowercase hex of the STA MAC (12 chars).
- `status` is always `"onboarding"` in this mode.

### `GET /scan`

Trigger a scan and return the list of surrounding access points.

```http
GET /scan HTTP/1.1
Host: 192.168.4.1
```

Response `200 OK`, `Content-Type: application/json`:

```json
[
  { "ssid": "HomeWifi",    "rssi": -48, "auth": 3 },
  { "ssid": "Neighbour5G", "rssi": -71, "auth": 4 }
]
```

- `rssi` is a signed dBm value.
- `auth` uses the ESP-IDF `wifi_auth_mode_t` enum:
  - `0` OPEN, `1` WEP, `2` WPA-PSK, `3` WPA2-PSK,
    `4` WPA/WPA2-PSK, `5` ENTERPRISE, `6` WPA3-PSK, `7` WPA2/WPA3-PSK,
    `8` WAPI-PSK, `9` OWE.
- Up to 16 entries; ordering is as the ESP radio returned them (not sorted).
- Call may take up to ~2 s because it blocks on a real scan.

### `POST /provision`

Persist Wi-Fi creds and the one-time provisioning token, then reboot.

```http
POST /provision HTTP/1.1
Host: 192.168.4.1
Content-Type: application/x-www-form-urlencoded

ssid=HomeWifi&password=hunter2&provision_token=eyJhbGciOi...
```

Parameters:

| Name              | Required | Notes                                                                 |
|-------------------|----------|-----------------------------------------------------------------------|
| `ssid`            | yes      | URL-encoded. `+` is decoded as space.                                 |
| `password`        | no       | Empty for open networks.                                              |
| `provision_token` | yes      | Opaque string from `POST /provisioning-tokens` on the Nadeem backend. |

Response `200 OK`:

```json
{ "ok": true }
```

If `ssid` is missing the device returns `400 Bad Request` with body `missing ssid`.

**Important:** The device reboots ~500 ms after sending the response. The
app's TCP connection will close abruptly — that is expected. Do not retry on
connection-reset.

### `POST /reset`

Factory-reset the device (clears the `nadeem` NVS namespace) and reboot.

```http
POST /reset HTTP/1.1
Host: 192.168.4.1
Content-Length: 0
```

Response `200 OK` body `{"ok":true}`, then reboots. Useful if the user wants to
re-provision a device that has already been bound to a different account.

---

## 4. Error handling / UX notes

- **No device AP visible.** The device never entered onboarding — either it
  already has Wi-Fi creds or it failed to boot. Instruct the user to hold the
  PLAY/PAUSE button ≥ 3 s (long-press reset), which clears creds and brings
  the AP back.
- **`POST /provision` returned 400.** Show the user a validation error and let
  them re-enter the SSID.
- **Device never appears on the user's LAN after 60 s.** Most common cause:
  wrong Wi-Fi password. The device will retry forever but never resurface
  its SoftAP on its own. Advise the user to long-press PLAY/PAUSE to reset.
- **Provisioning-token rejected by backend on device's side.** The device boots
  into normal mode but can't fetch manifests — stats and `GET /devices/<id>`
  will show it as "never-bootstrapped". The app should then offer "re-link".

---

## 5. Obtaining a `provision_token`

The app exchanges the logged-in user's session for a short-lived provisioning
token that the device will later present to the backend.

```http
POST /provisioning-tokens HTTP/1.1
Host: api.nadeem.example
Authorization: Bearer <user_session_jwt>
Content-Type: application/json

{}
```

Response `200 OK`:

```json
{
  "provision_token": "eyJhbGciOi...",
  "expires_at": "2026-04-19T21:00:00Z"
}
```

- Tokens are single-use.
- TTL: 10 minutes is recommended.
- Scope: "may bind a device to user `<user_id>`".

See `backend.md §4.1` for the server-side contract.

---

## 6. Post-provisioning app features (out of scope for onboarding)

Once a device is online, the companion app talks only to the Nadeem backend.
These features are not implemented in the firmware — the app-backend contract
is up to you. The typical surface the firmware assumes exists:

- **Catalog management:** add/remove semsem entries (UID → role → track list)
  for a given device. The firmware fetches the resulting manifest on demand.
- **Parental dashboard:** view `GET /devices/<id>/stats` — aggregated from the
  per-UID counters the firmware uploads every 5 minutes.
- **Content curation:** upload new audio files, mark UIDs as "PRO" (activates
  the AI-chat mode on the device).
- **Remote reset:** issue `POST /devices/<id>/reset` that the firmware could
  observe via a long-polled endpoint (not implemented in v0.1).

---

## 7. Test checklist

Before shipping the app, make sure:

- [ ] `GET /info` returns status 200 and parses when joined to `Nadeem-XXXX`.
- [ ] `GET /scan` shows at least the user's home network and values look sane.
- [ ] `POST /provision` with a valid SSID/password succeeds and the device
      disappears from the AP list within 5 s.
- [ ] The backend reports the device "online" + "bootstrapped" within 60 s.
- [ ] `POST /reset` works and brings the device back to onboarding mode.
- [ ] The app handles the special-character cases: SSID with `&`, `=`, spaces,
      and non-ASCII characters (UTF-8, URL-encoded).
- [ ] The app gracefully handles a dropped TCP connection mid-request
      (the device reboots aggressively after `/provision`).
- [ ] Repeated `/scan` calls within 2 s don't lock up the device.
