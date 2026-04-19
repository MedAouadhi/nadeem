#include "unity.h"
#include "led.h"

TEST_CASE("led_pattern_playing: brightest pixel rotates", "[led]")
{
    led_rgb_t a[6], b[6];
    led_pattern_playing(0,   a);
    led_pattern_playing(120, b);
    /* Find head (brightest green) in each frame and confirm it moved. */
    int head_a = 0, head_b = 0;
    for (int i = 0; i < 6; i++) if (a[i].g > a[head_a].g) head_a = i;
    for (int i = 0; i < 6; i++) if (b[i].g > b[head_b].g) head_b = i;
    TEST_ASSERT_NOT_EQUAL(head_a, head_b);
}

TEST_CASE("led_pattern_error: blinks", "[led]")
{
    led_rgb_t on[6], off[6];
    led_pattern_error(0,   on);
    led_pattern_error(250, off);
    TEST_ASSERT_NOT_EQUAL(on[0].r, off[0].r);
}
