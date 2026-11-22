idf_component_register(
    SRCS "buttons.c"
    INCLUDE_DIRS "include"
    REQUIRES binduos
    PRIV_REQUIRES esp_driver_gpio
)
