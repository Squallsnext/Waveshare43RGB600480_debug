# Configuration Matrix

## ESP-IDF Kconfig (sdkconfig.defaults)

| Parameter | Current | Min | Max | Notes |
|-----------|---------|-----|-----|-------|
| **CPU Frequency** | 240 MHz | 40 | 240 | ESP32-S3 native speed |
| **PSRAM** | Octal SPI @ 120MHz | N/A | 120 | Required for large buffers |
| **Flash Speed** | 80 MHz | 40 | 120 | Performance vs. stability trade-off |
| **Log Level** | DEBUG | NONE | VERBOSE | Set to INFO in production |
| **Core Count** | 2 cores | 1 | 2 | Dual-core for UI + telemetry |
| **UART Baud** | 115200 | 9600 | 921600 | Console output speed |

### Flash Partitions (partitions.csv)

| Name | Offset | Size | Usage |
|------|--------|------|-------|
| nvs | 0x0 | 24 KB | NVS (key-value store) |
| factory | 0x10000 | 2816 KB | Main application partition |
| spiffs | 0x2D0000 | 1280 KB | SPIFFS filesystem |
| **Total Used** | - | ~4 MB | Out of 16 MB available |

---

## Runtime Configuration (NVS + app_config.h)

### Rendering Pipeline

| Parameter | Type | Current | Options | Impact |
|-----------|------|---------|---------|--------|
| **render_mode** | enum | RENDER_MODE_PARTIAL | PARTIAL, DIRECT | Memory usage, latency |
| **full_refresh** | bool | true | true, false | Ghosting vs. power |
| **num_framebuffers** | int | 2 | 1, 2, 3 | Memory, tear-free display |
| **pclk_hz** | uint32_t | 16,000,000 | 4M–80M | Pixel clock speed |
| **bounce_buffer_height** | int | 20 | 1–100 | Partial refresh row count |

### Panel Configuration

| Parameter | Type | Current | Notes |
|-----------|------|---------|-------|
| **width** | uint16_t | 600 | Waveshare 4.3" RGB panel |
| **height** | uint16_t | 480 | Resolution fixed in hardware |
| **hsync_polarity** | uint8_t | 0 | Low active |
| **vsync_polarity** | uint8_t | 0 | Low active |
| **refresh_rate** | uint16_t | 60 | Target Hz |

### Touch Input (GT911)

| Parameter | Type | Current | Notes |
|-----------|------|---------|-------|
| **i2c_addr** | uint8_t | 0x5D | I2C address of GT911 |
| **int_gpio** | gpio_num_t | 16 | Interrupt line from GT911 |
| **rst_gpio** | gpio_num_t | 15 | Reset line to GT911 |
| **swap_xy** | bool | false | Axis mapping |
| **invert_x** | bool | false | X-axis inversion |
| **invert_y** | bool | false | Y-axis inversion |

### Telemetry

| Parameter | Type | Current | Notes |
|-----------|------|---------|-------|
| **enabled** | bool | true | Enable performance counters |
| **vsync_delta_ms** | uint32_t | 16 | ~60Hz (1000/60) |
| **task_watchdog_timeout_sec** | uint32_t | 10 | Task monitor threshold |

---

## Validation Rules

### Build-time Checks
- [ ] `pclk_hz` must be ≥ 4,000,000 Hz
- [ ] `pclk_hz` must be ≤ 80,000,000 Hz
- [ ] `num_framebuffers` ∈ {1, 2, 3}
- [ ] Total partition size must not exceed 16 MB
- [ ] NVS partition size ≥ 24 KB

### Runtime Checks
- [ ] VSync delta ≤ 50% of nominal (> 30Hz for 60Hz display)
- [ ] LVGL tick period ≤ 5 ms
- [ ] Touch input latency ≤ 100 ms
- [ ] Memory usage < 80% of PSRAM

### Performance Targets
| Metric | Target | Measurement Method |
|--------|--------|-------------------|
| Frame Rate | ~60 Hz | VSync counter delta/sec |
| Input Latency | <100 ms | Touch → screen response |
| Memory (heap) | <80% | `esp_get_free_heap_size()` |
| CPU Usage | <70% | Task runtime profiling |

---

## Tuning History

| Date | Parameter | Old → New | Rationale |
|------|-----------|-----------|-----------|
| 2025-12-27 | pclk_hz | — → 16M | Conservative startup speed |
| 2025-12-27 | num_fbs | — → 2 | Tear-free rendering |
| 2025-12-27 | full_refresh | — → true | Eliminate ghosting |

---

Last Updated: [TIMESTAMP]
Next Review Trigger: [After VSync smoke test, performance tuning phase]
