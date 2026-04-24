# Developer Console

> **Type:** Component
> **Status:** implemented

`esp_console` REPL on UART0. Started at boot via `console_cmds_start()`. Use during development (`idf.py monitor`) to drive every flow without app/backend.

## Known commands (per S5)

| Command | Effect |
|---------|--------|
| `nfc_poll` | One-shot read of a tag held on the PN532; prints UID. |
| `semsem_trigger <uid_hex>` | Simulates `NEV_HMI_SEMSEM_PLACED`; exercises full manifest path. |
| `stats_dump` | Print all per-UID counters. |
| `stats_upload` | Force `POST /stats` for any dirty entry. |
| `wifi_set <ssid> <pw>` | Write Wi-Fi creds to NVS without onboarding. |
| `wifi_forget` | Clear creds; reboot into onboarding mode. |
| `led mode <name>` | Force a specific `led_mode_t` for visual testing. |

Other commands likely exist (volume, factory reset, audio play). Confirm by reading `firmware/components/console_cmds/console_cmds.c`.

## Use-cases

- Backend self-test matrix (S5 §10) — every test goes through the console.
- Field debugging without the companion app.

---
confidence: 0.75
sources: [S5, S2]
last_confirmed: 2026-04-24
status: partial
