idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES
        nadeem_events
        nadeem_config
        bsp
        board_io
        hmi
        audio
        semsem
        semsem_pro
        led
        stats
        onboarding
        console_cmds
        backend_client
        nvs_flash
)
