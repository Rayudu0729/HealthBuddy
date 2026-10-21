idf_component_register(
    SRCS "app_main.c"
    INCLUDE_DIRS "."
    REQUIRES binduos sensors fall_detect voice alerts sync power app_update
)
