idf_component_register(
    SRCS "app_main.c"
    INCLUDE_DIRS "."
    REQUIRES binduos i2c_bus feedback buttons sensors fall_detect voice alerts sync power app_update
)
