idf_component_register(
    SRCS "board_waveshare_rgb.c" "board_lcd.c"
    INCLUDE_DIRS "include"
    REQUIRES
        app_config
        esp_lcd
        driver
        esp_timer
        freertos
    PRIV_REQUIRES
        lvgl
)
