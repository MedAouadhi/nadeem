idf_component_register(
    SRCS "led.c" "led_patterns.c"
    INCLUDE_DIRS "include"
    REQUIRES nadeem_events led_strip_hw esp_timer
)
