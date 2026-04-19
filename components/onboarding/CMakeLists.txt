idf_component_register(
    SRCS "onboarding.c"
    INCLUDE_DIRS "include"
    REQUIRES
        nadeem_events
        nadeem_config
        backend_client
        esp_http_server
        esp_wifi
        esp_netif
        esp_event
        nvs_flash
        json
)
