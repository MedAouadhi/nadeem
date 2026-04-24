# Constraints

> **Type:** Constraint
> **Status:** mixed

Hard limits that shape design. Violating any of these breaks integration.

## Hardware

- **MCU:** ESP32-S3 (Waveshare ESP32-S3-AUDIO-Board devkit).
- **NFC reader:** Elechouse PN532 v3 over UART1, 3.3 V only (do NOT use 5 V). SEL0/SEL1 both to GND for HSU.
- **GPIO map (PN532):** TX→GPIO 17 (UART1 RX), RX→GPIO 18 (UART1 TX), IRQ→GPIO 8 (unused, future ISR).
- **Codecs:** ES7210 (mic), ES8311 (DAC), I2S on GPIO 12/13/14/15/16, I2C on GPIO 10/11.
- **SD:** SDMMC 1-bit, GPIO 40/41/42.
- **LED strip:** 6 RGB LEDs (header allocates 7) on GPIO 38.
- **IO expander:** TCA9555 over I2C; buttons on pins 9/10/11.

Source: `firmware/components/bsp/include/bsp.h`, S5.

## Audio formats

- **Local playback (regular semsem):** MP3 default; WAV, AAC, FLAC accepted by `esp_audio_simple_player`. Recommended 44.1/48 kHz stereo MP3 @ 128–192 kbps. **Files ≤ 20 MiB** for flash/SD comfort.
- **Pro chat WebSocket:** signed 16-bit LE PCM, **mono**, **16 kHz**, **20 ms / 640 byte** binary frames upstream. Backend may send any chunk size downstream.

## Network

- **TLS mandatory in production.** Firmware uses `esp_http_client`; enable `CONFIG_MBEDTLS_CERTIFICATE_BUNDLE=y` and attach `esp_crt_bundle_attach` in `backend_client.c` before shipping (not wired in v0.1).
- **Auth:** Bearer `device_token` on every device→backend call except `/bootstrap`.
- **Stats batch:** one UID per `POST /stats` request.

## Localization

- **UI language:** Arabic only (AR).
- **Direction:** RTL strict, `dir="rtl"`.
- **Fonts:** Baloo Bhaijaan 2 (headings/logos), Cairo or Tajawal (body).

## Identifiers

- `device_id` = 12 hex chars, lowercase, STA MAC.
- `uid_hex` = 2·N hex chars, N ≤ 10, lowercase.
- `role` ≤ 31 bytes.
- Track filenames must be FAT-safe (no `/`, < 32 chars recommended).

## Onboarding SoftAP

- SSID: `Nadeem-XXXX` (last 4 hex of SoftAP MAC).
- Open auth, channel 1, max 4 clients, IP `192.168.4.1`.
- No mDNS during onboarding.

---
confidence: 0.93
sources: [S2, S3, S4, S5, S7]
last_confirmed: 2026-04-24
status: implemented
