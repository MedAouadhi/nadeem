#include "led.h"
#include <math.h>
#include <string.h>

#define N 6

static void fill(led_rgb_t out[N], uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < N; i++) out[i] = (led_rgb_t){r, g, b};
}

static uint8_t breath(uint32_t t_ms, uint32_t period_ms, uint8_t max) {
    /* Triangle wave 0..max..0 over period_ms. */
    uint32_t phase = t_ms % period_ms;
    uint32_t half = period_ms / 2;
    uint32_t v = phase < half ? phase : (period_ms - phase);
    return (uint8_t)((v * max) / half);
}

void led_pattern_boot(uint32_t t, led_rgb_t out[N]) {
    fill(out, 32, 32, 32);
    int head = (t / 80) % N;
    out[head] = (led_rgb_t){255, 255, 255};
}

void led_pattern_onboarding(uint32_t t, led_rgb_t out[N]) {
    uint8_t v = breath(t, 1200, 180);
    fill(out, 0, 0, v);
}

void led_pattern_idle(uint32_t t, led_rgb_t out[N]) {
    uint8_t v = breath(t, 4000, 40);
    fill(out, v, v / 2, 0);   /* dim amber */
}

void led_pattern_semsem_loading(uint32_t t, led_rgb_t out[N]) {
    fill(out, 8, 8, 8);
    int head = (t / 80) % N;
    for (int i = 0; i < N; i++) {
        int dist = (i - head + N) % N;
        out[i] = (led_rgb_t){ (uint8_t)(255 >> dist), (uint8_t)(255 >> dist), (uint8_t)(255 >> dist) };
    }
}

void led_pattern_playing(uint32_t t, led_rgb_t out[N]) {
    fill(out, 0, 32, 0);
    int head = (t / 120) % N;
    out[head] = (led_rgb_t){0, 255, 0};
}

void led_pattern_paused(uint32_t t, led_rgb_t out[N]) {
    (void)t;
    fill(out, 0, 80, 0);
}

void led_pattern_pro_listening(uint32_t t, led_rgb_t out[N]) {
    uint8_t v = breath(t, 900, 200);
    fill(out, v / 2, 0, v);   /* purple */
}

void led_pattern_pro_speaking(uint32_t t, led_rgb_t out[N]) {
    fill(out, 40, 0, 80);
    int head = (t / 70) % N;
    out[head] = (led_rgb_t){200, 0, 255};
}

void led_pattern_error(uint32_t t, led_rgb_t out[N]) {
    if ((t / 250) % 2) fill(out, 255, 0, 0);
    else               fill(out, 0, 0, 0);
}

void led_pattern_wifi_lost(uint32_t t, led_rgb_t out[N]) {
    uint8_t v = breath(t, 2000, 150);
    fill(out, v, v / 3, 0);   /* orange */
}
