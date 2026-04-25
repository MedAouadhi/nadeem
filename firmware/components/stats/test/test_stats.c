#include "unity.h"
#include "stats.h"
#include "nvs_flash.h"

TEST_CASE("stats: play count and total persist", "[stats]")
{
    (void)nvs_flash_init();
    TEST_ASSERT_EQUAL(ESP_OK, stats_init());

    stats_entry_t before = {0};
    stats_get("deadbeef", &before);

    TEST_ASSERT_EQUAL(ESP_OK, stats_record_play_start("deadbeef"));
    TEST_ASSERT_EQUAL(ESP_OK, stats_record_play_ms("deadbeef", 5000));
    TEST_ASSERT_EQUAL(ESP_OK, stats_flush());

    stats_entry_t after = {0};
    stats_get("deadbeef", &after);
    TEST_ASSERT_EQUAL(before.play_count + 1, after.play_count);
    TEST_ASSERT_GREATER_OR_EQUAL(before.total_play_ms + 5000, after.total_play_ms);
    TEST_ASSERT_TRUE(stats_has_dirty());

    TEST_ASSERT_EQUAL(ESP_OK, stats_mark_uploaded("deadbeef"));
    stats_get("deadbeef", &after);
    TEST_ASSERT_FALSE(after.dirty);
}
