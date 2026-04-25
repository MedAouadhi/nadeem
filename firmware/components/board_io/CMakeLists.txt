idf_component_register(
    SRCS "tca9555.c" "buttons.c"
    INCLUDE_DIRS "include"
    REQUIRES bsp
    PRIV_REQUIRES espressif__esp_io_expander_tca95xx_16bit
)
