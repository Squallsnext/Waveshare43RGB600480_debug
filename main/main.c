/*
 * Waveshare 4.3" RGB LCD + ESP32-S3 + LVGL 9.4
 *
 * CONTEXT-SAFETY WORKFLOW (Read First):
 * 1. START SESSION: Read docs/STATE.md (baseline config + current status)
 * 2. DURING: Read docs/TODO.md (top 10 tasks, definition of done)
 * 3. AFTER MAJOR CHANGE: Run `make checkpoint`
 * 4. BEFORE ENDING: Run `make check-budget`, if LOW run `make handoff`
 * 5. COMMIT GATE: Verify docs/STATE.md + docs/TODO.md updated
 *
 * NO MYSTICAL FIXES. Every change has measurement + rationale.
 * See docs/STATE.md "Architecture Rules Applied" for hard constraints.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Waveshare RGB LCD - Starting...");

    /* Init NVS (Non-Volatile Storage) */
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ESP_LOGI(TAG, "NVS initialized");

    /* TODO: app_config_init() - Load runtime config from NVS, validate */
    /* TODO: board_waveshare_rgb_init() - LCD, Touch, Telemetry */
    /* TODO: eez_ui_init() - EEZ Framework (stub, user provides assets) */

    ESP_LOGI(TAG, "Entering idle loop");
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
        ESP_LOGI(TAG, "Heartbeat");
    }
}
