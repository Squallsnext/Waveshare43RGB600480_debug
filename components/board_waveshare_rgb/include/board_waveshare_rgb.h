#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t board_waveshare_rgb_init(void);
void* board_get_lvgl_display(void);

#ifdef __cplusplus
}
#endif
