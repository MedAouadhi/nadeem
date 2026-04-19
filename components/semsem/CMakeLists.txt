idf_component_register(
    SRCS "semsem.c"
    INCLUDE_DIRS "include"
    REQUIRES nadeem_events backend_client stats smf nvs_flash json
)
