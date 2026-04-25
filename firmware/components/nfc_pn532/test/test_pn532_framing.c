#include "unity.h"
#include "pn532_internal.h"
#include <string.h>

TEST_CASE("pn532 frame: SAMConfiguration encodes correctly", "[pn532]")
{
    /* SAMConfiguration(normal, 20, disable IRQ) per PN532 UM v3 §7.2.10 */
    uint8_t body[] = { 0x14, 0x01, 0x14, 0x00 };
    uint8_t out[16];
    size_t n = pn532_build_frame(body, sizeof body, out, sizeof out);

    /* Expected: 00 00 FF 05 FB D4 14 01 14 00 03 00 */
    const uint8_t want[] = {
        0x00, 0x00, 0xFF, 0x05, 0xFB,
        0xD4, 0x14, 0x01, 0x14, 0x00,
        0x03, 0x00
    };
    TEST_ASSERT_EQUAL(sizeof want, n);
    TEST_ASSERT_EQUAL_MEMORY(want, out, n);
}

TEST_CASE("pn532 frame: parse valid response", "[pn532]")
{
    /* Response to GetFirmwareVersion: 00 00 FF 06 FA D5 03 32 01 06 07 E8 00 */
    uint8_t resp[] = {
        0x00, 0x00, 0xFF, 0x06, 0xFA,
        0xD5, 0x03, 0x32, 0x01, 0x06, 0x07,
        0xE8, 0x00
    };
    const uint8_t *body;
    size_t body_len;
    TEST_ASSERT_TRUE(pn532_parse_frame(resp, sizeof resp, &body, &body_len));
    TEST_ASSERT_EQUAL(5, body_len);
    TEST_ASSERT_EQUAL(0x03, body[0]);   /* response code */
    TEST_ASSERT_EQUAL(0x32, body[1]);   /* IC */
}

TEST_CASE("pn532 frame: ACK recognized", "[pn532]")
{
    const uint8_t ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
    TEST_ASSERT_TRUE(pn532_is_ack(ack));
    const uint8_t not_ack[] = {0x00, 0x00, 0xFF, 0x00, 0xAA, 0x00};
    TEST_ASSERT_FALSE(pn532_is_ack(not_ack));
}
