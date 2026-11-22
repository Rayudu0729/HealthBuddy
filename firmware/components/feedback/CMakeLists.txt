idf_component_register(
    SRCS "feedback.c" "drv2605.c"
    INCLUDE_DIRS "include"
    REQUIRES binduos
    PRIV_REQUIRES i2c_bus esp_driver_gpio
)
