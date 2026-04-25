#include "pn532_internal.h"
#include <stdbool.h>

size_t pn532_build_frame(const uint8_t *body, size_t body_len,
                         uint8_t *out, size_t out_cap)
{
    /* Frame layout (PN532 User Manual v3, §6.2.1.1):
     *   PREAMBLE (0x00)
     *   START CODE (0x00 0xFF)
     *   LEN = length of (TFI + body)
     *   LCS = two's complement of LEN (so LEN + LCS == 0x00 mod 256)
     *   TFI = 0xD4  (host -> PN532)
     *   PD0..PDn = body bytes
     *   DCS = two's complement of the sum of TFI and body bytes
     *   POSTAMBLE (0x00)
     * Total length = 8 + body_len
     */
    size_t need = 8 + body_len;
    if (out_cap < need) return 0;

    size_t i = 0;
    out[i++] = 0x00;            /* preamble */
    out[i++] = 0x00;            /* start code */
    out[i++] = 0xFF;
    uint8_t len = (uint8_t)(body_len + 1);
    out[i++] = len;
    out[i++] = (uint8_t)(0x100 - len);
    out[i++] = PN532_TFI_HOST_TO_PN532;

    uint8_t sum = PN532_TFI_HOST_TO_PN532;
    for (size_t j = 0; j < body_len; j++) {
        out[i++] = body[j];
        sum += body[j];
    }
    out[i++] = (uint8_t)(0x100 - sum);
    out[i++] = 0x00;            /* postamble */
    return i;
}

bool pn532_is_ack(const uint8_t *buf)
{
    static const uint8_t ack[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
    for (int i = 0; i < 6; i++) if (buf[i] != ack[i]) return false;
    return true;
}

bool pn532_parse_frame(const uint8_t *buf, size_t buf_len,
                       const uint8_t **body_out, size_t *body_len_out)
{
    /* Locate start code; there may be leading 0x00 preamble bytes. */
    if (buf_len < 7) return false;
    size_t i = 0;
    while (i + 2 < buf_len && !(buf[i] == 0x00 && buf[i+1] == 0x00 && buf[i+2] == 0xFF)) {
        i++;
    }
    if (i + 7 > buf_len) return false;
    i += 3;                                                /* past 00 00 FF */
    uint8_t len = buf[i++];
    uint8_t lcs = buf[i++];
    if ((uint8_t)(len + lcs) != 0x00) return false;
    if (i + len + 1 > buf_len) return false;
    uint8_t tfi = buf[i++];
    if (tfi != PN532_TFI_PN532_TO_HOST) return false;

    uint8_t sum = tfi;
    const uint8_t *body = &buf[i];
    size_t body_len = len - 1;
    for (size_t j = 0; j < body_len; j++) sum += body[j];
    uint8_t dcs = buf[i + body_len];
    if ((uint8_t)(sum + dcs) != 0x00) return false;

    *body_out = body;
    *body_len_out = body_len;
    return true;
}
