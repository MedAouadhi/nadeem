# Relationship Graph

> **Type:** Graph edges
> **Status:** current

Typed edges for traversal. Verbs from [AGENTS.md §3](../AGENTS.md). Format: `A --[verb]--> B [confidence] # note`.

## Firmware internal

```
main.c                     --[uses]-->        nadeem_events           [0.97]
main.c                     --[uses]-->        bsp                     [0.97]
main.c                     --[uses]-->        nadeem_config           [0.97]
main.c                     --[uses]-->        backend_client          [0.97]
main.c                     --[uses]-->        onboarding              [0.97]  # only when no Wi-Fi creds
main.c                     --[uses]-->        hmi, audio, semsem, semsem_pro, led, stats, console_cmds [0.97]

bsp                        --[uses]-->        board_io                [0.95]  # I2C bus shared
hmi                        --[uses]-->        nfc_pn532               [0.95]
hmi                        --[uses]-->        board_io                [0.95]  # button callback
hmi                        --[produces]-->    NEV_HMI_*               [0.97]

audio                      --[uses]-->        bsp                     [0.95]  # I2S DAC
audio                      --[consumes]-->    NEV_AUDIO_PLAY/STOP/... [0.97]
audio                      --[produces]-->    NEV_AUDIO_STATE         [0.97]
audio                      --[uses]-->        stats                   [0.85]  # play_ms recording

semsem                     --[uses]-->        backend_client          [0.95]
semsem                     --[uses]-->        audio                   [0.95]
semsem                     --[uses]-->        stats                   [0.95]
semsem                     --[caches]-->      Manifest, audio files   [0.95]  # /sdcard/semsem/<uid>/
semsem                     --[consumes]-->    NEV_HMI_SEMSEM_PLACED   [0.97]
semsem                     --[produces]-->    NEV_AUDIO_PLAY, NEV_SYS_MODE_PRO_ENTER [0.97]

semsem_pro                 --[uses]-->        bsp                     [0.95]  # mic + DAC
semsem_pro                 --[calls]-->       WS /chat                [0.95]
semsem_pro                 --[consumes]-->    NEV_SYS_MODE_PRO_ENTER  [0.97]
semsem_pro                 --[produces]-->    NEV_SYS_PRO_LISTENING/SPEAKING [0.95]

led                        --[uses]-->        led_strip_hw            [0.97]
led                        --[consumes]-->    NEV_AUDIO_STATE, NEV_SYS_* [0.95]

onboarding                 --[uses]-->        nadeem_config           [0.95]
onboarding                 --[implements]-->  GET /info, /scan, POST /provision, /reset [0.95]

backend_client             --[calls]-->       POST /bootstrap         [0.95]
backend_client             --[calls]-->       GET /semsem/{uid}/manifest [0.95]
backend_client             --[calls]-->       Audio CDN               [0.92]
backend_client             --[calls]-->       POST /stats             [0.95]

stats                      --[caches]-->      UsageStats              [0.92]  # NVS namespace nadeem_stat
nadeem_config              --[caches]-->      Wi-Fi creds, backend_url, device_token [0.95]  # NVS namespace nadeem
```

## Cross-system

```
Mobile App                 --[calls]-->       POST /api/provisioning-tokens [0.85]
Mobile App                 --[calls]-->       GET /info, /scan, POST /provision, /reset [0.9]
ESP32-S3 (firmware)        --[calls]-->       Cloud Backend           [0.95]
ESP32-S3 (firmware)        --[calls]-->       Audio CDN               [0.9]
ESP32-S3 (firmware)        --[calls]-->       WS /chat                [0.9]
Web Frontend               --[calls]-->       /api/* endpoints        [0.7]   # spec-only
Cloud Backend              --[binds-to]-->    Device, User            [0.85]  # at /bootstrap
Cloud Backend              --[implements]-->  All device endpoints + /api/* [0.7]
Cloud Backend              --[produces]-->    Manifest, ProChatSession [0.7]
Cloud Backend              --[consumes]-->    POST /stats             [0.7]
Cloud Backend              --[implements]-->  Admin Platform (dashboard, stats explorer, fleet ops) [0.95]
Cloud Backend              --[implements]-->  GET /firmware/check     [0.95]  # OTA endpoint
NadeemAdminSite            --[uses]-->        django-unfold           [0.95]
NadeemAdminSite            --[uses]-->        django-admin-sortable2  [0.95]
Semsem                     --[binds-to]-->    SemsemGroup             [0.95]
Device                     --[binds-to]-->    ReleaseGroup            [0.95]  # optional, for OTA routing
ReleaseGroup               --[uses]-->        FirmwareRelease         [0.95]
ProChatSession             --[produces]-->    flagged, flag_reason    [0.95]  # moderation fields
```

## Hardware wiring

```
ESP32-S3 GPIO 17/18        --[uses]-->        PN532 (UART1)           [0.97]
ESP32-S3 GPIO 8            --[uses]-->        PN532 IRQ               [0.85]  # wired, unused in v0.1
ESP32-S3 GPIO 10/11        --[uses]-->        I2C (codecs + TCA9555)  [0.97]
ESP32-S3 GPIO 12-16        --[uses]-->        I2S (ES7210, ES8311)    [0.97]
ESP32-S3 GPIO 38           --[uses]-->        LED strip               [0.97]
ESP32-S3 GPIO 40-42        --[uses]-->        SDMMC                   [0.97]
TCA9555 pins 9/10/11       --[produces]-->    Button events           [0.95]
```

## Decisions / supersession

- `decisions/0003-smf-state-machines.md` --[supersedes]--> ad-hoc task design (per S6) [0.9]
- `esp_sr` removal --[supersedes]--> on-device wake-word (per S5 §6) [0.95]

## Contradictions / open

- PRD `prd_back_front.md` --[contradicts]--> firmware reality on `backend_url` plumbing [0.6]  # PRD silent on how device learns backend URL
- `prd_back_front.md` §1.1 says "Frontend (Web & Mobile-Web): Next.js" but `firmware/docs/mobile.md` describes a native mobile app. [0.7]  # Tracked in lint.md

## Caused / fixed

(empty — no incident chains yet)

---
confidence: 0.88
sources: [S2, S3, S4, S5, S7, backend/nadeem/admin_site.py, backend/firmware/views.py]
last_confirmed: 2026-04-27
status: partial
