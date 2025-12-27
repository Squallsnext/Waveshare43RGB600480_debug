cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS
    "${CMAKE_CURRENT_SOURCE_DIR}/components/app_config"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/board_waveshare_rgb"
    "${CMAKE_CURRENT_SOURCE_DIR}/components/eez_ui"
)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(waveshare_rgb_lcd)
