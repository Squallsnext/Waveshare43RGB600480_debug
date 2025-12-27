/**
 * @file board_lcd.c
 * @brief RGB LCD panel initialization and LVGL integration
 *
 * Implements:
 * - esp_lcd_rgb_panel for Waveshare 4.3" 800x480 RGB565 display
 * - LVGL 9.4 display driver with flush callback
 * - VSync/Bounce callback registration with telemetry counters
 * - 1-second telemetry logging task
 * - Test screen for VSync smoke test
 */

#include "board_lcd.h"
#include "board_pins.h"
#include "app_config.h"

#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_rgb.h"
#include "esp_timer.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include <string.h>
#include <assert.h>

static const char *TAG = "LCD";

/*******************************************************************************
 * Static Variables
 ******************************************************************************/

static esp_lcd_panel_handle_t s_panel_handle = NULL;
static lv_display_t *s_lv_display = NULL;

/* Telemetry counters (volatile for ISR access) */
static board_lcd_telemetry_t s_telemetry = {0};
static board_lcd_telemetry_t s_telemetry_last = {0};  /* For delta calculation */

/* Flush timing */
static int64_t s_flush_start_us = 0;

/* Guard: ensure flush_ready called exactly once per flush */
static volatile bool s_flush_pending = false;

/* Telemetry task handle */
static TaskHandle_t s_telemetry_task_handle = NULL;

/* Frame buffers (allocated in PSRAM) */
static void *s_fb1 = NULL;
static void *s_fb2 = NULL;

/*******************************************************************************
 * VSync and Bounce Callbacks (IRAM-safe)
 ******************************************************************************/

static bool IRAM_ATTR on_vsync_event(esp_lcd_panel_handle_t panel,
                                      const esp_lcd_rgb_panel_event_data_t *event_data,
                                      void *user_ctx)
{
    s_telemetry.vsync_count++;
    return false;  /* No high-priority task woken */
}

static bool IRAM_ATTR on_bounce_event(esp_lcd_panel_handle_t panel,
                                       const esp_lcd_rgb_panel_event_data_t *event_data,
                                       void *user_ctx)
{
    s_telemetry.bounce_count++;
    return false;
}

/*******************************************************************************
 * LVGL Flush Callback
 ******************************************************************************/

static void lvgl_flush_cb(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
    /* Guard: Must not be called while previous flush pending */
    assert(!s_flush_pending && "flush_cb called while previous flush pending!");
    s_flush_pending = true;

    s_flush_start_us = esp_timer_get_time();
    s_telemetry.flush_count++;

    /* For RGB panel, the px_map IS the framebuffer - no copy needed.
     * Just need to signal LVGL that flush is complete. */
    esp_lcd_panel_handle_t panel = (esp_lcd_panel_handle_t)lv_display_get_user_data(disp);
    (void)panel;  /* RGB panel doesn't need explicit draw command */

    /* Calculate flush duration */
    int64_t duration_us = esp_timer_get_time() - s_flush_start_us;
    s_telemetry.flush_time_sum_us += (uint32_t)duration_us;
    if ((uint32_t)duration_us > s_telemetry.flush_time_max_us) {
        s_telemetry.flush_time_max_us = (uint32_t)duration_us;
    }

    /* Signal LVGL that flush is complete - EXACTLY ONCE per flush */
    assert(s_flush_pending && "flush_ready called but no flush pending!");
    s_flush_pending = false;
    s_telemetry.flush_ready_calls++;
    lv_display_flush_ready(disp);
}

/*******************************************************************************
 * Telemetry Task (1-second logging)
 ******************************************************************************/

static void telemetry_task(void *arg)
{
    TickType_t last_wake = xTaskGetTickCount();
    const TickType_t period = pdMS_TO_TICKS(1000);

    while (1) {
        vTaskDelayUntil(&last_wake, period);

        /* Calculate deltas */
        uint32_t vsync_delta = s_telemetry.vsync_count - s_telemetry_last.vsync_count;
        uint32_t bounce_delta = s_telemetry.bounce_count - s_telemetry_last.bounce_count;
        uint32_t flush_delta = s_telemetry.flush_count - s_telemetry_last.flush_count;
        uint32_t flush_time_sum_delta = s_telemetry.flush_time_sum_us - s_telemetry_last.flush_time_sum_us;

        /* Calculate average flush time */
        float avg_flush_ms = 0.0f;
        if (flush_delta > 0) {
            avg_flush_ms = (float)flush_time_sum_delta / (float)flush_delta / 1000.0f;
        }

        /* Get max flush time (since last reset) */
        float max_flush_ms = (float)s_telemetry.flush_time_max_us / 1000.0f;

        /* VSync Smoke Test Gate: if delta == 0, something is wrong */
        if (vsync_delta == 0) {
            ESP_LOGE(TAG, "VSYNC SMOKE TEST FAILED! VSync delta=0. Check: callbacks registered? panel started? IRQ enabled?");
        } else if (bounce_delta == 0) {
            ESP_LOGW(TAG, "Bounce delta=0 (may be OK if not using bounce buffer mode)");
        }

        /* Log telemetry */
        ESP_LOGI(TAG, "VSync:%lu Bounce:%lu Flush:%lu AvgTime:%.3f ms MaxTime:%.3f ms",
                 (unsigned long)vsync_delta,
                 (unsigned long)bounce_delta,
                 (unsigned long)flush_delta,
                 avg_flush_ms,
                 max_flush_ms);

        /* Verify flush_ready invariant */
        if (s_telemetry.flush_ready_calls != s_telemetry.flush_count) {
            ESP_LOGE(TAG, "FLUSH INVARIANT VIOLATED! flush_count=%lu flush_ready_calls=%lu",
                     (unsigned long)s_telemetry.flush_count,
                     (unsigned long)s_telemetry.flush_ready_calls);
        }

        /* Update last snapshot */
        memcpy(&s_telemetry_last, &s_telemetry, sizeof(s_telemetry_last));

        /* Log heap info periodically */
        ESP_LOGD(TAG, "Heap: free=%lu largest=%lu PSRAM_free=%lu",
                 (unsigned long)esp_get_free_heap_size(),
                 (unsigned long)heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT),
                 (unsigned long)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    }
}

/*******************************************************************************
 * Public API: Initialization
 ******************************************************************************/

esp_err_t board_lcd_init(void)
{
    ESP_LOGI(TAG, "Initializing RGB LCD panel (%dx%d)", LCD_H_RES, LCD_V_RES);

    const app_config_t *config = app_config_get();
    ESP_LOGI(TAG, "Config: render_mode=%d, full_refresh=%d, num_fbs=%d, pclk=%lu Hz",
             config->render_mode, config->full_refresh, config->num_fbs,
             (unsigned long)config->pclk_hz);

    /* Allocate frame buffers in PSRAM */
    size_t fb_size = LCD_H_RES * LCD_V_RES * sizeof(uint16_t);  /* RGB565 = 2 bytes/pixel */
    ESP_LOGI(TAG, "Allocating %d frame buffers (%zu bytes each) in PSRAM", config->num_fbs, fb_size);

    s_fb1 = heap_caps_malloc(fb_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (!s_fb1) {
        ESP_LOGE(TAG, "Failed to allocate FB1 in PSRAM");
        return ESP_ERR_NO_MEM;
    }

    if (config->num_fbs >= 2) {
        s_fb2 = heap_caps_malloc(fb_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        if (!s_fb2) {
            ESP_LOGE(TAG, "Failed to allocate FB2 in PSRAM");
            heap_caps_free(s_fb1);
            s_fb1 = NULL;
            return ESP_ERR_NO_MEM;
        }
    }

    /* Configure RGB panel */
    esp_lcd_rgb_panel_config_t panel_config = {
        .clk_src = LCD_CLK_SRC_DEFAULT,
        .timings = {
            .pclk_hz = config->pclk_hz,
            .h_res = LCD_H_RES,
            .v_res = LCD_V_RES,
            .hsync_pulse_width = LCD_HSYNC_PULSE_WIDTH,
            .hsync_back_porch = LCD_HSYNC_BACK_PORCH,
            .hsync_front_porch = LCD_HSYNC_FRONT_PORCH,
            .vsync_pulse_width = LCD_VSYNC_PULSE_WIDTH,
            .vsync_back_porch = LCD_VSYNC_BACK_PORCH,
            .vsync_front_porch = LCD_VSYNC_FRONT_PORCH,
            .flags = {
                .hsync_idle_low = 0,
                .vsync_idle_low = 0,
                .de_idle_high = 0,
                .pclk_active_neg = LCD_PCLK_ACTIVE_NEG,  /* 1 = active low (from Waveshare demo) */
                .pclk_idle_high = 0,
            },
        },
        .data_width = 16,  /* RGB565 */
        .num_fbs = config->num_fbs,
        .bounce_buffer_size_px = config->bounce_buffer_height * LCD_H_RES,
        .sram_trans_align = 4,
        .psram_trans_align = 64,
        .hsync_gpio_num = LCD_GPIO_HSYNC,
        .vsync_gpio_num = LCD_GPIO_VSYNC,
        .de_gpio_num = LCD_GPIO_DE,
        .pclk_gpio_num = LCD_GPIO_PCLK,
        .disp_gpio_num = LCD_GPIO_BL,  /* -1 if via IO expander */
        .data_gpio_nums = {
            LCD_DATA_GPIO_NUMS
        },
        .flags = {
            .fb_in_psram = 1,
            .double_fb = (config->num_fbs >= 2) ? 1 : 0,
            .no_fb = 0,
            .refresh_on_demand = 0,
        },
    };

    ESP_LOGI(TAG, "Creating RGB panel...");
    esp_err_t ret = esp_lcd_new_rgb_panel(&panel_config, &s_panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create RGB panel: %s", esp_err_to_name(ret));
        return ret;
    }

    /* Register VSync and Bounce callbacks */
    esp_lcd_rgb_panel_event_callbacks_t callbacks = {
        .on_vsync = on_vsync_event,
        .on_bounce_frame_finish = on_bounce_event,
    };
    ESP_LOGI(TAG, "Registering VSync/Bounce callbacks...");
    ret = esp_lcd_rgb_panel_register_event_callbacks(s_panel_handle, &callbacks, NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register callbacks: %s", esp_err_to_name(ret));
        return ret;
    }

    /* Initialize panel */
    ESP_LOGI(TAG, "Resetting and initializing panel...");
    ret = esp_lcd_panel_reset(s_panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Panel reset failed: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = esp_lcd_panel_init(s_panel_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Panel init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    /* Initialize LVGL display */
    ESP_LOGI(TAG, "Creating LVGL display...");
    s_lv_display = lv_display_create(LCD_H_RES, LCD_V_RES);
    if (!s_lv_display) {
        ESP_LOGE(TAG, "Failed to create LVGL display");
        return ESP_FAIL;
    }

    /* Configure LVGL buffers */
    lv_display_set_buffers(s_lv_display, s_fb1, s_fb2, fb_size,
                           config->full_refresh ? LV_DISPLAY_RENDER_MODE_FULL
                                                 : LV_DISPLAY_RENDER_MODE_PARTIAL);

    /* Set flush callback */
    lv_display_set_flush_cb(s_lv_display, lvgl_flush_cb);
    lv_display_set_user_data(s_lv_display, s_panel_handle);

    /* Start telemetry task */
    ESP_LOGI(TAG, "Starting telemetry task...");
    BaseType_t task_ret = xTaskCreate(telemetry_task, "lcd_telem", 4096, NULL, 5, &s_telemetry_task_handle);
    if (task_ret != pdPASS) {
        ESP_LOGE(TAG, "Failed to create telemetry task");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "LCD initialization complete");
    return ESP_OK;
}

/*******************************************************************************
 * Public API: Getters
 ******************************************************************************/

lv_display_t* board_lcd_get_display(void)
{
    return s_lv_display;
}

esp_lcd_panel_handle_t board_lcd_get_panel(void)
{
    return s_panel_handle;
}

void board_lcd_get_telemetry(board_lcd_telemetry_t *out)
{
    if (out) {
        /* Atomic copy not strictly necessary for 32-bit reads on ESP32 */
        memcpy(out, &s_telemetry, sizeof(*out));
    }
}

void board_lcd_reset_telemetry(void)
{
    memset(&s_telemetry, 0, sizeof(s_telemetry));
    memset(&s_telemetry_last, 0, sizeof(s_telemetry_last));
}

/*******************************************************************************
 * Test Screen (for VSync smoke test)
 ******************************************************************************/

void board_lcd_create_test_screen(void)
{
    if (!s_lv_display) {
        ESP_LOGE(TAG, "Display not initialized");
        return;
    }

    lv_obj_t *scr = lv_display_get_screen_active(s_lv_display);

    /* Background: gradient test pattern */
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x003366), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_color(scr, lv_color_hex(0x660033), LV_PART_MAIN);
    lv_obj_set_style_bg_grad_dir(scr, LV_GRAD_DIR_VER, LV_PART_MAIN);

    /* Title label */
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "VSync Smoke Test");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_24, LV_PART_MAIN);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    /* Scrollable container */
    lv_obj_t *cont = lv_obj_create(scr);
    lv_obj_set_size(cont, LCD_H_RES - 40, LCD_V_RES - 150);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x222222), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(cont, LV_OPA_80, LV_PART_MAIN);
    lv_obj_set_style_radius(cont, 10, LV_PART_MAIN);

    /* Add items to scrollable container */
    for (int i = 0; i < 20; i++) {
        lv_obj_t *item = lv_label_create(cont);
        lv_label_set_text_fmt(item, "Scroll Item %d - Testing LVGL rendering", i + 1);
        lv_obj_set_style_text_color(item, lv_color_hex(0xCCCCCC), LV_PART_MAIN);
        lv_obj_align(item, LV_ALIGN_TOP_LEFT, 10, 10 + i * 30);
    }

    /* Test button */
    lv_obj_t *btn = lv_button_create(scr);
    lv_obj_set_size(btn, 200, 50);
    lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(btn, lv_color_hex(0x00AA00), LV_PART_MAIN);
    lv_obj_set_style_radius(btn, 10, LV_PART_MAIN);

    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Test Button");
    lv_obj_set_style_text_color(btn_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_center(btn_label);

    ESP_LOGI(TAG, "Test screen created");
}
