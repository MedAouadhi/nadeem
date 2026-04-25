idf_component_register(
    SRCS "pn532.c" "pn532_frame.c"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "."
    REQUIRES driver esp_timer
)
