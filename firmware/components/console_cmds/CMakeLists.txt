idf_component_register(
    SRCS "console_cmds.c"
    INCLUDE_DIRS "include"
    REQUIRES
        console
        nadeem_events
        nadeem_config
        backend_client
        audio
        semsem
        stats
        led
        nfc_pn532
)
