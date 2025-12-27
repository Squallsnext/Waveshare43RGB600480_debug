/**
 * @file board_lcd.h
 * @brief LCD initialization and LVGL integration for Waveshare 4.3" RGB panel
 */

#pragma once

#include "esp_err.h"
#include "esp_lcd_types.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Telemetry Counters (atomic, updated by callbacks)
 ******************************************************************************/

typedef struct {
    volatile uint32_t vsync_count;       /* VSync interrupts total */
    volatile uint32_t bounce_count;      /* Bounce buffer completions total */
    volatile uint32_t flush_count;       /* LVGL flush_cb invocations total */
    volatile uint32_t flush_time_sum_us; /* Sum of flush durations (for avg) */
    volatile uint32_t flush_time_max_us; /* Max flush duration */
    volatile uint32_t flush_ready_calls; /* lv_disp_flush_ready() calls (must == flush_count) */
} board_lcd_telemetry_t;

/*******************************************************************************
 * Public API
 ******************************************************************************/

/**
 * @brief Initialize the RGB LCD panel and LVGL display driver
 *
 * Creates:
 * - esp_lcd_rgb_panel with configured timing and GPIOs
 * - LVGL display with flush callback
 * - Telemetry counters + 1-second logging task
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t board_lcd_init(void);

/**
 * @brief Get the LVGL display handle
 * @return lv_display_t* or NULL if not initialized
 */
lv_display_t* board_lcd_get_display(void);

/**
 * @brief Get the ESP LCD panel handle
 * @return esp_lcd_panel_handle_t or NULL if not initialized
 */
esp_lcd_panel_handle_t board_lcd_get_panel(void);

/**
 * @brief Get current telemetry snapshot (thread-safe copy)
 * @param[out] out Pointer to telemetry struct to fill
 */
void board_lcd_get_telemetry(board_lcd_telemetry_t *out);

/**
 * @brief Reset telemetry counters (for delta calculation)
 */
void board_lcd_reset_telemetry(void);

/**
 * @brief Create minimal test screen (test pattern + scrollable container + button)
 *
 * Used for VSync smoke test and visual verification.
 */
void board_lcd_create_test_screen(void);

#ifdef __cplusplus
}
#endif
