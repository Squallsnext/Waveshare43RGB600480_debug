#include "board_waveshare_rgb.h"
#include "app_config.h"
#include "esp_log.h"

static const char *TAG = "BOARD";

esp_err_t board_waveshare_rgb_init(void)
{
    const app_config_t *config = app_config_get();
    ESP_LOGI(TAG, "Init - mode=%d, full_refresh=%d", config->render_mode, config->full_refresh);
    /* TODO: board_lcd_init() - RGB panel, LVGL flush callback, VSync gating */
    /* TODO: board_touch_init() - GT911 driver, LVGL input device */
    /* TODO: board_telemetry_start() - 1-second task, atomic counters */
    return ESP_OK;
}

void* board_get_lvgl_display(void)
{
    return NULL;  /* TODO: return lv_display_t* */
}
