# Component Architecture & API Reference

## Component Overview

### app_config
**Purpose:** Centralized application configuration (enum render_mode, bool full_refresh, etc.)
**Location:** `components/app_config/`
**API:** `app_config.h`
**Dependencies:** None (stub)
**Status:** [STUB|IN_PROGRESS|COMPLETE|TESTING]

#### Public Functions
```c
// Initialization
int app_config_init(void);

// Getters
render_mode_t app_config_get_render_mode(void);
bool app_config_get_full_refresh(void);
uint32_t app_config_get_pclk_hz(void);
uint8_t app_config_get_num_fbs(void);
uint16_t app_config_get_bounce_buffer_height(void);

// Setters (NVS + runtime)
int app_config_set_render_mode(render_mode_t mode);
int app_config_set_pclk_hz(uint32_t hz);
int app_config_commit_to_nvs(void);

// Telemetry
bool app_config_telemetry_enabled(void);
```

#### Data Structures
```c
typedef enum {
  RENDER_MODE_PARTIAL = 0,  // Partial refresh (bouncing buffer)
  RENDER_MODE_DIRECT = 1,   // Full-screen direct draw (after VSync test)
} render_mode_t;

typedef struct {
  render_mode_t render_mode;
  bool full_refresh;
  uint8_t num_fbs;
  uint32_t pclk_hz;
  uint16_t bounce_buffer_height;
  bool telemetry_enabled;
} app_config_t;
```

---

### board_waveshare_rgb
**Purpose:** RGB LCD panel driver (Waveshare 4.3" 600×480)
**Location:** `components/board_waveshare_rgb/`
**API:** `board_lcd.h`
**Dependencies:** esp_lcd, LVGL, app_config
**Status:** [STUB|IN_PROGRESS|COMPLETE|TESTING]

#### Public Functions
```c
// LCD initialization
int board_lcd_init(void);
int board_lcd_start(void);

// Display operations
int board_lcd_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
int board_lcd_set_backlight(uint8_t percent);

// Info
uint16_t board_lcd_get_width(void);
uint16_t board_lcd_get_height(void);
uint32_t board_lcd_get_pclk_hz(void);

// Telemetry
uint32_t board_lcd_get_vsync_count(void);
uint32_t board_lcd_get_vsync_delta_ms(void);
```

#### Pins (ESP32-S3)
| Signal | GPIO | Notes |
|--------|------|-------|
| R[4:0] | 11, 12, 13, 14, 0 | Red channel |
| G[5:0] | 8, 20, 3, 46, 9, 10 | Green channel |
| B[4:0] | 16, 1, 5, 6, 7 | Blue channel |
| HSYNC | 17 | Horizontal sync |
| VSYNC | 18 | Vertical sync |
| ENABLE | 21 | Data enable |
| PCLK | 19 | Pixel clock |
| RESET | 4 | Panel reset |
| BACKLIGHT | [PWM_PIN] | Backlight dimming |

#### Critical Behavior
- **lv_disp_flush_ready()** called exactly once per flush (no deadlock risk)
- **RENDER_MODE_PARTIAL:** Use bouncing buffer (20px high default)
- **RENDER_MODE_DIRECT:** Full-screen flush (only after VSync smoke test)
- **VSync gating:** Wait for VSYNC before starting next frame (prevents tearing)

---

### board_touch (GT911)
**Purpose:** Capacitive touch input (GT911 driver via I2C)
**Location:** `components/board_touch/`
**API:** `board_touch.h`
**Dependencies:** esp_lcd_touch, I2C
**Status:** [STUB|IN_PROGRESS|COMPLETE|TESTING]

#### Public Functions
```c
// Initialization
int board_touch_init(void);

// Input polling
int board_touch_read(lv_indev_t *indev, lv_indev_data_t *data);

// Configuration
int board_touch_set_swap_xy(bool swap);
int board_touch_calibrate(void);

// Telemetry
uint32_t board_touch_get_press_count(void);
uint32_t board_touch_get_latency_ms(void);
```

#### I2C Configuration
| Parameter | Value | Notes |
|-----------|-------|-------|
| Address | 0x5D | GT911 default |
| SCL GPIO | 47 | I2C clock |
| SDA GPIO | 48 | I2C data |
| Frequency | 400 kHz | Standard I2C speed |
| INT GPIO | 16 | Interrupt line (falling edge) |
| RST GPIO | 15 | Reset (active low) |

#### Stability Notes
- Always use ESP-IDF `esp_lcd_touch` abstraction (not raw I2C)
- Debounce interrupt (10 ms typical)
- CRC check on all I2C reads
- Timeout on stuck INT line (2 sec)

---

### board_telemetry
**Purpose:** Performance counters (FPS, latency, memory, CPU)
**Location:** `components/board_telemetry/`
**API:** `board_telemetry.h`
**Dependencies:** FreeRTOS, atomic operations
**Status:** [STUB|IN_PROGRESS|COMPLETE|TESTING]

#### Public Functions
```c
// Initialization
int board_telemetry_init(void);

// Counters (atomic)
void board_telemetry_inc_vsync(void);
void board_telemetry_inc_flush(void);
void board_telemetry_inc_touch_event(void);

// Query
uint32_t board_telemetry_get_vsync_count(void);
uint32_t board_telemetry_get_fps(void);
uint32_t board_telemetry_get_free_heap_kb(void);

// Log (1x/sec max)
void board_telemetry_print_summary(void);
```

#### Output Format
```
[TELEMETRY] FPS: 60.2 Hz | Heap: 180/256 KB | Touch events: 45 | CPU: 32%
```

---

### eez_ui (EEZ Studio)
**Purpose:** UI framework (LVGL integration, screen definitions)
**Location:** `components/eez_ui/`
**API:** `eez_ui.h`
**Dependencies:** LVGL 9.4, app_config
**Status:** [STUB|IN_PROGRESS|COMPLETE|TESTING]

#### Public Functions
```c
// Initialization
int eez_ui_init(lv_display_t *disp, lv_indev_t *indev);

// Screen management
int eez_ui_show_screen(uint8_t screen_id);
int eez_ui_handle_tick(void);

// Data binding
int eez_ui_set_value(const char *var_name, int32_t value);
int32_t eez_ui_get_value(const char *var_name);
```

#### Integration Points
- **Display:** Connects to LVGL display driver (board_waveshare_rgb)
- **Input:** Connects to LVGL input device (board_touch)
- **Tick:** Called by main loop or FreeRTOS task (every 5 ms)

---

## Initialization Sequence

```
main()
  ├─ app_config_init()           [Load NVS config]
  ├─ board_lcd_init()            [Initialize RGB panel, LVGL]
  ├─ board_touch_init()          [Initialize GT911, I2C]
  ├─ board_telemetry_init()      [Start counters]
  ├─ eez_ui_init()               [Initialize UI framework]
  └─ Loop:
      ├─ board_telemetry_print_summary()  [1x/sec]
      ├─ eez_ui_handle_tick()             [5ms]
      ├─ app_config_check_nvm()           [Reload on change]
      └─ vTaskDelay(pdMS_TO_TICKS(5))
```

---

## Error Handling

All component functions return:
- **0** = Success
- **Negative** = Error code (ESP_ERR_*)

Caller must:
1. Check return code
2. Log error (max 1x/sec per component)
3. Fail gracefully or restart component
4. Never proceed with uninitialized dependencies

---

## Testing & Verification

### Smoke Tests (Pre-Flash)
- [ ] `board_lcd_init()` without render loop
- [ ] `board_touch_init()` with dummy I2C
- [ ] `app_config_init()` with NVS mock

### Runtime Tests (Post-Flash)
- [ ] Display pixels change at expected rate (VSync test)
- [ ] Touch input produces events with <100 ms latency
- [ ] Telemetry counters increment at expected rates
- [ ] No heap corruption (SIGSEGV, stack overflow)

---

Last Updated: [TIMESTAMP]
Next Review: [After component implementation]
