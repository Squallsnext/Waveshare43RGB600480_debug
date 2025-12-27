#include "app_config.h"
#include "esp_log.h"
#include <string.h>

static const char *TAG = "APP_CONFIG";

/**
 * Default Configuration - STABILITY FIRST
 *
 * Phase 2 Baseline:
 *   render_mode = PARTIAL (not DIRECT)
 *   full_refresh = true (full screen refresh)
 *   num_fbs = 2 (double-buffer minimum)
 *   pclk_hz = 16MHz (conservative, not 18MHz)
 *
 * After VSync Smoke Test passes (VSync counter ~60Hz), can switch to DIRECT.
 */
static app_config_t s_config = {
    .render_mode = RENDER_MODE_PARTIAL,  /* START STABLE, NOT DIRECT */
    .full_refresh = true,                /* Full screen refresh */
    .num_fbs = 2,                        /* Double-buffer */
    .bounce_buffer_height = 20,
    .pclk_hz = 16000000,                 /* Conservative 16MHz */
    .hsync_front_porch = 40,
    .hsync_back_porch = 40,
    .vsync_front_porch = 13,
    .vsync_back_porch = 13,
    .touch_threshold = 30,
    .telemetry_enabled = true,
};

esp_err_t app_config_init(void)
{
    ESP_LOGI(TAG, "Init - mode=%d, full_refresh=%d, num_fbs=%d, pclk=%lu Hz",
             s_config.render_mode, s_config.full_refresh, s_config.num_fbs,
             s_config.pclk_hz);
    /* TODO: Load from NVS, validate on load */
    return ESP_OK;
}

const app_config_t* app_config_get(void)
{
    return &s_config;
}

esp_err_t app_config_set_u8(const char *key, uint8_t value)
{
    ESP_LOGW(TAG, "set_u8 not implemented: %s=%d", key, value);
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t app_config_set_u16(const char *key, uint16_t value)
{
    ESP_LOGW(TAG, "set_u16 not implemented: %s=%d", key, value);
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t app_config_set_u32(const char *key, uint32_t value)
{
    ESP_LOGW(TAG, "set_u32 not implemented: %s=%lu", key, (unsigned long)value);
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t app_config_set_bool(const char *key, bool value)
{
    ESP_LOGW(TAG, "set_bool not implemented: %s=%d", key, value);
    return ESP_ERR_NOT_SUPPORTED;
}

esp_err_t app_config_reset_defaults(void)
{
    ESP_LOGI(TAG, "Reset defaults");
    return ESP_OK;
}

/**
 * Validation Rules (Hard Constraints)
 *
 * Rule 1: DIRECT mode requires num_fbs >= 2
 * Rule 2: full_refresh=true with num_fbs<2 may cause tearing (warn)
 */
bool app_config_validate(char *error_msg, size_t msg_size)
{
    /* Rule 1: DIRECT mode requires num_fbs >= 2 */
    if (s_config.render_mode == RENDER_MODE_DIRECT && s_config.num_fbs < 2) {
        if (error_msg) {
            snprintf(error_msg, msg_size, "DIRECT mode requires num_fbs >= 2");
        }
        return false;
    }

    /* Rule 2: full_refresh + num_fbs < 2 may tear */
    if (s_config.full_refresh && s_config.num_fbs < 2) {
        ESP_LOGW(TAG, "full_refresh=true with num_fbs<2 may cause tearing, recommend num_fbs>=2");
        /* Not fatal, just warning */
    }

    return true;
}
