idf_component_register(
    SRCS "board_waveshare_rgb.c"
    INCLUDE_DIRS "include"
    REQUIRES
        app_config
        esp_lcd
        driver
)
