#pragma once

#include <stdint.h>
#include <stddef.h>

/* PN532 command codes (selection). */
#define PN532_CMD_GET_FIRMWARE_VERSION 0x02
#define PN532_CMD_SAM_CONFIGURATION    0x14
#define PN532_CMD_IN_LIST_PASSIVE_TGT  0x4A

/* Host-to-PN532 TFI is 0xD4, PN532-to-host is 0xD5. */
#define PN532_TFI_HOST_TO_PN532 0xD4
#define PN532_TFI_PN532_TO_HOST 0xD5

/**
 * Build a normal-information frame wrapping `body` with header, length, and checksums.
 * Body layout: [CMD, PARAM, PARAM, ...] (TFI is added by this function).
 * Returns the number of bytes written into `out`, or 0 on buffer overflow.
 */
size_t pn532_build_frame(const uint8_t *body, size_t body_len,
                         uint8_t *out, size_t out_cap);

/** Validate and strip a response frame. On success, populates *body_out pointing
 *  into buf and *body_len. Returns true on a valid response (TFI = 0xD5). */
bool pn532_parse_frame(const uint8_t *buf, size_t buf_len,
                       const uint8_t **body_out, size_t *body_len_out);

/** Returns true if the 6 bytes at buf form a valid ACK frame (00 00 FF 00 FF 00). */
bool pn532_is_ack(const uint8_t *buf);
