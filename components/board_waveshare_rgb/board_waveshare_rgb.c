#include "board_waveshare_rgb.h"
#include "board_lcd.h"
#include "app_config.h"
#include "esp_log.h"

static const char *TAG = "BOARD";

esp_err_t board_waveshare_rgb_init(void)
{
    const app_config_t *config = app_config_get();
    ESP_LOGI(TAG, "Init - mode=%d, full_refresh=%d", config->render_mode, config->full_refresh);

    /* Initialize LCD panel and LVGL display */
    esp_err_t ret = board_lcd_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "LCD init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    /* Create test screen for VSync smoke test */
    board_lcd_create_test_screen();

    /* TODO: board_touch_init() - GT911 driver, LVGL input device */
    /* TODO: board_telemetry_start() - 1-second task (now in board_lcd.c) */

    ESP_LOGI(TAG, "Board initialization complete");
    return ESP_OK;
}

void* board_get_lvgl_display(void)
{
    return board_lcd_get_display();
}
