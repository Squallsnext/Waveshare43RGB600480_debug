#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * LVGL Rendering Mode (buffer strategy)
 * IMPORTANT: This is SEPARATE from full_refresh (see app_config_t)
 */
typedef enum {
    RENDER_MODE_PARTIAL = 0,  /* Partial updates (dirty rects) */
    RENDER_MODE_DIRECT = 1,   /* Direct mode (write to framebuffer) */
} app_render_mode_t;

/**
 * Runtime Configuration - STABILITY FIRST
 *
 * Initial stage (Phase 2):
 *   render_mode = PARTIAL
 *   full_refresh = true
 *   num_fbs = 2
 *   pclk_hz = 16MHz (conservative)
 *
 * After VSync Smoke Test passes (~60Hz):
 *   Can upgrade to render_mode = DIRECT
 *
 * HARD RULES:
 * - render_mode and full_refresh are INDEPENDENT
 * - DIRECT mode requires num_fbs >= 2
 * - All waits have timeout + fallback (no deadlocks)
 * - Telemetry via atomic counters, logging max 1x/second
 */
typedef struct {
    /* Rendering (separate parameters, not combined) */
    app_render_mode_t render_mode;  /* PARTIAL or DIRECT */
    bool full_refresh;              /* true = full screen, false = dirty rects */
    uint8_t num_fbs;                /* 2 or 3 framebuffers */
    uint16_t bounce_buffer_height;  /* Rows per bounce buffer (10-100) */

    /* LCD Timing */
    uint32_t pclk_hz;               /* Pixel clock frequency (Hz) */
    uint16_t hsync_front_porch;
    uint16_t hsync_back_porch;
    uint16_t vsync_front_porch;
    uint16_t vsync_back_porch;

    /* Touch */
    uint8_t touch_threshold;        /* Touch sensitivity */

    /* Debug */
    bool telemetry_enabled;         /* Enable 1-second telemetry logging */
} app_config_t;

/* API */
esp_err_t app_config_init(void);
const app_config_t* app_config_get(void);
esp_err_t app_config_set_u8(const char *key, uint8_t value);
esp_err_t app_config_set_u16(const char *key, uint16_t value);
esp_err_t app_config_set_u32(const char *key, uint32_t value);
esp_err_t app_config_set_bool(const char *key, bool value);
esp_err_t app_config_reset_defaults(void);
bool app_config_validate(char *error_msg, size_t msg_size);

#ifdef __cplusplus
}
#endif
