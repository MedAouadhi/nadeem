idf_component_register(
    SRCS "nadeem_config.c"
    INCLUDE_DIRS "include"
    REQUIRES nadeem_events nvs_flash esp_wifi esp_netif esp_event
)
