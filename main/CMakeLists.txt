idf_component_register(
    SRCS "main.c"
    INCLUDE_DIRS "."
    REQUIRES
        app_config
        board_waveshare_rgb
        eez_ui
        nvs_flash
)
