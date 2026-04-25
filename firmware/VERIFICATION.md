# Nadeem firmware — on-host verification

The ESP-IDF toolchain is not installed in the agent sandbox where the code was
written, so these are the commands to run on your dev machine to confirm the
firmware compiles and behaves as expected on the Waveshare ESP32-S3-AUDIO-Board
with a PN532 v3 wired via UART1.

## 1. Build

```sh
. $HOME/esp/esp-idf/export.sh      # or wherever ESP-IDF ≥5.0 lives
cd /path/to/nadeem
idf.py set-target esp32s3
idf.py fullclean
idf.py build
```

If a managed component fails to resolve (`esp_websocket_client`, `esp_audio_simple_player`,
`esp_io_expander_tca95xx_16bit`, `led_strip`), run `idf.py reconfigure` once so
the component manager pulls them from the Espressif registry.

## 2. Run unit tests (host/target unity)

```sh
cd $IDF_PATH/tools/unit-test-app
idf.py -T nadeem_events -T nfc_pn532 -T stats -T led -T onboarding build flash monitor
```

Expected passing cases:

- `nadeem_events`: post + dispatch, UID → hex
- `nfc_pn532`: SAMConfiguration framing, response parsing, ACK recognition
- `stats`: play-count persistence, dirty flag lifecycle
- `led`: rotating-head and blink patterns
- `onboarding`: form parser decodes `%20`/`+` and recognises all fields

## 3. Flash and manual smoke test

```sh
idf.py -p /dev/ttyUSB0 flash monitor
```

Plug in the devkit with a microSD inserted and the PN532 wired to GPIO 17/18/8.

### First boot (no Wi-Fi stored)

1. Serial log prints `entering onboarding mode`.
2. A Wi-Fi AP named `Nadeem-XXXX` appears.
3. From a phone:
   ```sh
   curl http://192.168.4.1/info
   curl -X POST -d 'ssid=YourNet&password=secret&provision_token=demo' \
        http://192.168.4.1/provision
   ```
4. Device reboots, connects to Wi-Fi, runs `on_wifi_up` bootstrap.

### Normal mode

- `nfc_poll` in the REPL → prints the UID of a tag held on the PN532.
- Place a cached tag with an audio folder at `/sdcard/semsem/<uid>/` → plays.
- Place an unknown tag → backend fetch + LED `SEMSEM_LOADING` → plays.
- Place a PRO-flagged UID (`{"pro": true, "role": "doctor"}` in manifest) →
  `NEV_SYS_MODE_PRO_ENTER` logged, WebSocket connects to `wss://<backend>/chat`.
- Buttons on TCA pins 9/10/11 emit `NEV_HMI_BTN_PREV/PLAY_PAUSE/NEXT`.
- `stats_dump` prints per-UID counters; `stats_upload` POSTs dirty entries.
- Long-press PLAY_PAUSE → factory reset (NVS wipe + reboot back to onboarding).

## 4. Hardware wiring checklist for PN532

| PN532 pin | ESP32-S3 pin | Note |
|-----------|--------------|------|
| TX        | GPIO 17      | UART1 RX |
| RX        | GPIO 18      | UART1 TX |
| IRQ       | GPIO 8       | unused in polling mode; wired for future ISR |
| VCC       | 3.3 V        | do NOT use 5 V |
| GND       | GND          | common with board |
| SEL0      | GND          | select HSU |
| SEL1      | GND          | select HSU |

## 5. Backend expectations

The firmware assumes a JSON REST backend at `<backend_url>` with:

- `POST /bootstrap` body `{"provision_token": "..."}` → `{"device_token": "..."}`.
- `GET /semsem/<uid_hex>/manifest` → `{"pro": bool, "role": str,
  "tracks": [{"name": "01.mp3", "url": "https://.../01.mp3"}]}`.
- `POST /stats` body: per-UID counter snapshot (see `backend_client.c`).
- WebSocket at `wss://<backend>/chat?role=<role>` accepting 16 kHz mono PCM16
  binary frames upstream and returning the same format downstream. Optional
  text control frames `{"type":"listening"}` / `{"type":"speaking"}` flip the
  LED animation.

## 6. Known limitations

- `audio` seek to `start_byte` relies on `esp_audio_simple_player` seeking,
  which is best-effort — some MP3s will restart from 0. Progress is still
  preserved for the track index.
- `esp_sr` was removed. Arabic wake-word detection is not supported locally;
  all voice understanding happens on the backend while in Pro mode.
- `backend_client` does not set a TLS cert bundle. Add
  `CONFIG_MBEDTLS_CERTIFICATE_BUNDLE=y` and `.crt_bundle_attach =
  esp_crt_bundle_attach` in `backend_client.c` for production.
