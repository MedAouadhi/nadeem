#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_err.h"
#include "driver/uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * PN532 (v3, HSU / UART) driver — bare minimum for passive-target (ISO14443A) polling.
 *
 * Wiring to the Waveshare ESP32-S3-AUDIO-Board:
 *   PN532 TX  -> ESP32 GPIO 17 (UART1 RX)
 *   PN532 RX  <- ESP32 GPIO 18 (UART1 TX)
 *   PN532 IRQ -> ESP32 GPIO 8  (unused in polling mode, wired for future use)
 *   VCC       -> 3.3 V
 *   GND       -> common GND
 *   SEL0/SEL1 -> both tied LOW to select HSU mode
 *   Baud rate: 115200 8N1
 */

#define PN532_DEFAULT_UART_PORT   UART_NUM_1
#define PN532_DEFAULT_TX_GPIO     18
#define PN532_DEFAULT_RX_GPIO     17
#define PN532_DEFAULT_IRQ_GPIO    8
#define PN532_DEFAULT_BAUD        115200

typedef struct {
    uint8_t uid[10];
    uint8_t uid_len;
} pn532_uid_t;

/** Configure UART and wake the PN532. Must be called before any other API. */
esp_err_t pn532_init(uart_port_t port, int tx_gpio, int rx_gpio);

/** Configure SAM in normal mode (no IRQ). */
esp_err_t pn532_sam_configure(void);

/** Poll for one ISO14443A target. Returns ESP_OK with uid_len>0 on detection,
 *  ESP_ERR_TIMEOUT when no card is present. */
esp_err_t pn532_read_passive_target(pn532_uid_t *out, uint32_t timeout_ms);

/** Read firmware version (for smoke-test). Returns 4 bytes in buf_out. */
esp_err_t pn532_get_firmware_version(uint8_t buf_out[4]);

#ifdef __cplusplus
}
#endif
