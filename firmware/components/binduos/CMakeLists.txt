idf_component_register(
    SRCS "event_bus.c" "power_mode.c"
    INCLUDE_DIRS "include"
    PRIV_REQUIRES esp_timer
)
