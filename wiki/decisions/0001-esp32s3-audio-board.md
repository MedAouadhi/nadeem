# 0001 — Waveshare ESP32-S3-AUDIO-Board as devkit

**Status:** accepted
**Date:** 2026-04-25 (recorded; original choice predates wiki)

## Context

Need a devkit with: I2S audio in/out, SD card, enough GPIO for NFC + LEDs + IO expander, ESP-IDF support, low BOM cost for v0.1 hardware iteration.

## Decision

Use Waveshare ESP32-S3-AUDIO-Board. Built-in ES7210 mic codec + ES8311 DAC + SDMMC + LED strip pin + I2C bus. Pin map already encoded in `firmware/components/bsp/include/bsp.h`.

## Consequences

- All firmware is written against ESP-IDF ≥ 5.0 + Espressif's component manager.
- Codec drivers (`esp_codec_dev`) are vendored via component manager.
- Future board changes require updating only `bsp` component.
- Locks us to ESP32-S3 family (mostly fine — same vendor across S3 variants).

## Sources

- S2 (`firmware/components/bsp/include/bsp.h`)
- S6 (`firmware/requirements.md` — devkit explicitly named)
