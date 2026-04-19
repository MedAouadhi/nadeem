#include "pn532.h"
#include "pn532_internal.h"
#include <string.h>
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "pn532";

#define PN532_RX_BUF 128
#define PN532_TX_BUF 64

static uart_port_t s_port = UART_NUM_1;

static esp_err_t wait_for_ack(uint32_t timeout_ms)
{
    uint8_t buf[6];
    int read = uart_read_bytes(s_port, buf, 6, pdMS_TO_TICKS(timeout_ms));
    if (read != 6) {
        ESP_LOGW(TAG, "no ACK (got %d bytes)", read);
        return ESP_ERR_TIMEOUT;
    }
    return pn532_is_ack(buf) ? ESP_OK : ESP_FAIL;
}

static esp_err_t read_response(uint8_t *buf, size_t buf_cap,
                               const uint8_t **body_out, size_t *body_len,
                               uint32_t timeout_ms)
{
    int read = uart_read_bytes(s_port, buf, buf_cap, pdMS_TO_TICKS(timeout_ms));
    if (read <= 0) return ESP_ERR_TIMEOUT;
    return pn532_parse_frame(buf, (size_t)read, body_out, body_len)
        ? ESP_OK : ESP_ERR_INVALID_RESPONSE;
}

static esp_err_t send_command(const uint8_t *body, size_t body_len,
                              uint32_t ack_timeout_ms)
{
    uint8_t frame[PN532_TX_BUF];
    size_t n = pn532_build_frame(body, body_len, frame, sizeof frame);
    if (n == 0) return ESP_ERR_INVALID_SIZE;

    uart_flush_input(s_port);
    int written = uart_write_bytes(s_port, (const char *)frame, n);
    if (written != (int)n) return ESP_FAIL;
    return wait_for_ack(ack_timeout_ms);
}

esp_err_t pn532_init(uart_port_t port, int tx_gpio, int rx_gpio)
{
    s_port = port;

    uart_config_t cfg = {
        .baud_rate = PN532_DEFAULT_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };

    ESP_ERROR_CHECK(uart_driver_install(port, 512, 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(port, &cfg));
    ESP_ERROR_CHECK(uart_set_pin(port, tx_gpio, rx_gpio,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    /* Wake the PN532: spec says send 16+ 0x55 bytes after reset. */
    uint8_t wake[20];
    memset(wake, 0x55, sizeof wake);
    wake[0] = 0x00;
    uart_write_bytes(port, (const char *)wake, sizeof wake);
    vTaskDelay(pdMS_TO_TICKS(50));
    uart_flush_input(port);

    ESP_LOGI(TAG, "PN532 UART %d initialized (tx=%d rx=%d)", port, tx_gpio, rx_gpio);
    return ESP_OK;
}

esp_err_t pn532_sam_configure(void)
{
    uint8_t body[] = {
        PN532_CMD_SAM_CONFIGURATION,
        0x01,   /* normal mode */
        0x14,   /* timeout * 50ms = 1s */
        0x00,   /* IRQ disabled */
    };
    esp_err_t err = send_command(body, sizeof body, 100);
    if (err != ESP_OK) return err;

    uint8_t resp[32];
    const uint8_t *rbody;
    size_t rlen;
    err = read_response(resp, sizeof resp, &rbody, &rlen, 200);
    if (err != ESP_OK) return err;
    /* Expected response: 0x15 */
    if (rlen < 1 || rbody[0] != 0x15) return ESP_ERR_INVALID_RESPONSE;
    return ESP_OK;
}

esp_err_t pn532_get_firmware_version(uint8_t buf_out[4])
{
    uint8_t body[] = { PN532_CMD_GET_FIRMWARE_VERSION };
    esp_err_t err = send_command(body, sizeof body, 100);
    if (err != ESP_OK) return err;
    uint8_t resp[32];
    const uint8_t *rbody;
    size_t rlen;
    err = read_response(resp, sizeof resp, &rbody, &rlen, 200);
    if (err != ESP_OK) return err;
    if (rlen < 5 || rbody[0] != 0x03) return ESP_ERR_INVALID_RESPONSE;
    memcpy(buf_out, &rbody[1], 4);
    return ESP_OK;
}

esp_err_t pn532_read_passive_target(pn532_uid_t *out, uint32_t timeout_ms)
{
    if (!out) return ESP_ERR_INVALID_ARG;
    uint8_t body[] = {
        PN532_CMD_IN_LIST_PASSIVE_TGT,
        0x01,   /* MaxTg = 1 (poll only one target) */
        0x00,   /* 106 kbps type A */
    };
    esp_err_t err = send_command(body, sizeof body, 100);
    if (err != ESP_OK) return err;

    uint8_t resp[64];
    const uint8_t *rbody;
    size_t rlen;
    err = read_response(resp, sizeof resp, &rbody, &rlen, timeout_ms);
    if (err != ESP_OK) return err;

    /* Response body layout for InListPassiveTarget (106 kbps A):
     *   [0]    = 0x4B (response code)
     *   [1]    = NbTg (number of targets, 0 or 1)
     *   [2]    = Tg (logical number, typically 1)
     *   [3..4] = SENS_RES (ATQA)
     *   [5]    = SEL_RES (SAK)
     *   [6]    = NFCIDLength
     *   [7..]  = NFCID bytes
     */
    if (rlen < 2 || rbody[0] != 0x4B) return ESP_ERR_INVALID_RESPONSE;
    if (rbody[1] == 0) {
        out->uid_len = 0;
        return ESP_ERR_TIMEOUT;     /* no target present */
    }
    if (rlen < 7) return ESP_ERR_INVALID_RESPONSE;
    uint8_t uid_len = rbody[6];
    if (uid_len > sizeof(out->uid) || rlen < 7u + uid_len) return ESP_ERR_INVALID_RESPONSE;
    out->uid_len = uid_len;
    memcpy(out->uid, &rbody[7], uid_len);
    return ESP_OK;
}
