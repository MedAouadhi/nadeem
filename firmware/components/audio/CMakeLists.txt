idf_component_register(
    SRCS "audio.c"
    INCLUDE_DIRS "include"
    REQUIRES
        nadeem_events
        bsp
        board_io
        smf
    PRIV_REQUIRES
        espressif__esp_audio_simple_player
)
