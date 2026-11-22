idf_component_register(
    SRCS "fall_detect.c" "mpu6050.c"
    INCLUDE_DIRS "include"
    REQUIRES binduos i2c_bus
    PRIV_REQUIRES esp_timer
)
