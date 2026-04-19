#include "unity.h"
#include "nadeem_events.h"
#include "freertos/event_groups.h"

static EventGroupHandle_t s_eg;

static void on_btn_next(void *arg, esp_event_base_t base, int32_t id, void *data) {
    (void)arg; (void)base; (void)data;
    if (id == NEV_HMI_BTN_NEXT) xEventGroupSetBits(s_eg, 0x1);
}

TEST_CASE("nadeem_events posts and dispatches", "[nadeem_events]") {
    s_eg = xEventGroupCreate();
    TEST_ASSERT_EQUAL(ESP_OK, nadeem_events_init());
    TEST_ASSERT_EQUAL(ESP_OK, nadeem_events_on(NEV_HMI_BTN_NEXT, on_btn_next, NULL));
    TEST_ASSERT_EQUAL(ESP_OK, nadeem_events_post(NEV_HMI_BTN_NEXT, NULL, 0, 100));
    EventBits_t bits = xEventGroupWaitBits(s_eg, 0x1, pdTRUE, pdTRUE, pdMS_TO_TICKS(200));
    TEST_ASSERT_TRUE((bits & 0x1) != 0);
    vEventGroupDelete(s_eg);
}

TEST_CASE("nadeem_events: UID → hex", "[nadeem_events]") {
    nev_semsem_uid_t uid = { .uid = {0xde, 0xad, 0xbe, 0xef}, .uid_len = 4 };
    char buf[16];
    nadeem_uid_to_hex(&uid, buf, sizeof buf);
    TEST_ASSERT_EQUAL_STRING("deadbeef", buf);
}
