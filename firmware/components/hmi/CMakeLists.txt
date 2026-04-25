idf_component_register(
    SRCS "hmi.c"
    INCLUDE_DIRS "include"
    REQUIRES nadeem_events nfc_pn532 board_io
)
