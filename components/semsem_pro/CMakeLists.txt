idf_component_register(
    SRCS "semsem_pro.c"
    INCLUDE_DIRS "include"
    REQUIRES nadeem_events nadeem_config bsp board_io stats smf
    PRIV_REQUIRES espressif__esp_websocket_client
)
