idf_component_register(
    SRCS "led_strip_hw.c"
    INCLUDE_DIRS "include"
    REQUIRES driver
    PRIV_REQUIRES espressif__led_strip
)
