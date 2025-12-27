#pragma once

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t eez_ui_init(void);
void eez_ui_show_main_screen(void);

#ifdef __cplusplus
}
#endif
