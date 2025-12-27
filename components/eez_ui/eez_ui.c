#include "eez_ui.h"
#include "esp_log.h"

static const char *TAG = "EEZ_UI";

esp_err_t eez_ui_init(void)
{
    ESP_LOGI(TAG, "Init (stub - user provides EEZ Studio generated assets)");
    /* TODO: Load EEZ-generated UI code from components/eez_ui/ui directory */
    return ESP_OK;
}

void eez_ui_show_main_screen(void)
{
    ESP_LOGI(TAG, "Show main screen (stub)");
}
