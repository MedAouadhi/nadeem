# Hardware

> **Type:** Constraint
> **Status:** implemented

Devkit: **Waveshare ESP32-S3-AUDIO-Board**. Add-on: **Elechouse PN532 v3** wired via UART1.

## GPIO map (from `bsp.h` and S5)

### Audio (I2S)

| Signal | GPIO |
|--------|------|
| MCLK | 12 |
| SCLK / BCLK | 13 |
| LRCK / WS | 14 |
| SDIN (mic ← codec) | 15 |
| DOUT (DAC → speaker) | 16 |

I2S default: `STD_PHILIPS_SLOT_DEFAULT(32, STEREO)`, master clock 16 kHz.

### I2C

| Signal | GPIO |
|--------|------|
| SCL | 10 |
| SDA | 11 |

I2C bus shared between codecs, IO expander.

### Codecs

- **Mic:** ES7210 (multi-channel ADC). Configurable mic channels (3 or 4 depending on layout).
- **DAC:** ES8311.

### SD card (SDMMC, 1-bit)

| Signal | GPIO |
|--------|------|
| CLK | 40 |
| CMD | 42 |
| D0 | 41 |
| D1–D3 | NC |

Mount: `/sdcard`, ≤ 10 open files.

### LED strip

| Pin | Value |
|-----|-------|
| Data | GPIO 38 |
| Count | 7 LEDs allocated, 6 actively driven |

### IO expander (TCA9555)

Default direction: pins 0,1,5,6,8 output; 2,9,10,11 input. Buttons on pins 9/10/11 (`KEY_ID_9/10/11`) → short-press / long-press events via `board_io_buttons_register`.

### PN532 (UART1, HSU mode)

| PN532 | ESP32-S3 |
|-------|----------|
| TX | GPIO 17 (UART1 RX) |
| RX | GPIO 18 (UART1 TX) |
| IRQ | GPIO 8 (wired, unused — future ISR) |
| VCC | 3.3 V (**never 5 V**) |
| GND | common |
| SEL0, SEL1 | both GND (selects HSU) |

## Power / volume

- Default playback volume: 60 (range 0–100).
- Default record gain: 30.0.

## Source

`firmware/components/bsp/include/bsp.h`, `firmware/VERIFICATION.md` §4.

---
confidence: 0.97
sources: [S2, S5]
last_confirmed: 2026-04-24
status: implemented
