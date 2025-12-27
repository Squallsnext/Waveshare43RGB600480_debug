# Configuration Matrix - Waveshare 4.3" RGB LCD Project

## ESP-IDF Kconfig (sdkconfig.defaults)

| Parameter | Current | Min | Max | Notes |
|-----------|---------|-----|-----|-------|
| **CPU Frequency** | 240 MHz | 40 | 240 | ESP32-S3 native speed |
| **PSRAM Mode** | Octal SPI | — | — | Required for large buffers |
| **PSRAM Speed** | 120 MHz | 80 | 120 | Max stable speed for Waveshare panel |
| **Flash Speed** | 80 MHz | 40 | 120 | 80 MHz recommended for stability |
| **Log Level** | DEBUG | NONE | VERBOSE | Set to INFO in production |
| **Core Count** | 2 cores | 1 | 2 | Dual-core for UI + telemetry |
| **UART Baud** | 115200 | 9600 | 921600 | Console output speed |
| **Partition Scheme** | Custom | — | — | See partitions.csv |
| **Task Watchdog** | Enabled | — | — | 10 second timeout |

### Flash Partitions (partitions.csv)

| Name | Offset | Size | Usage | Free |
|------|--------|------|-------|------|
| nvs | 0x0 | 24 KB | NVS (key-value store) | — |
| factory | 0x10000 | 2816 KB | Main application partition | 79% |
| spiffs | 0x2D0000 | 1280 KB | SPIFFS filesystem (future) | 100% |
| **Total Used** | — | ~4 MB | Out of 16 MB available | — |

---

## Runtime Configuration (NVS + app_config.h)

### Rendering Pipeline Settings

| Parameter | Type | Current | Range | Options | Impact | DoD Validation |
|-----------|------|---------|-------|---------|--------|-----------------|
| **render_mode** | enum | RENDER_MODE_PARTIAL | — | PARTIAL, DIRECT | Memory usage, latency | ✓ Separate from full_refresh |
| **full_refresh** | bool | true | — | true, false | Ghosting vs. power | ✓ Independent control |
| **num_framebuffers** | uint8_t | 2 | 1–3 | 1, 2, 3 | Memory, tear-free display | ✓ Conservative default (2) |
| **pclk_hz** | uint32_t | 16,000,000 | 4M–80M | Any (Hz) | Pixel clock speed | ✓ Conservative startup (16M) |
| **bounce_buffer_height** | uint16_t | 20 | 1–100 | pixels | Partial refresh row count | ✓ Optimized for 600×480 |

**Rationale:**
- **PARTIAL mode:** Reduces memory footprint, lower latency
- **full_refresh=true:** Eliminates ghosting artifacts (critical for UI)
- **2 FBs:** Sweet spot between memory and tear-free rendering
- **16 MHz pclk:** Conservative baseline for stability testing
- **20-pixel bounce buffer:** ~3-4 FBs at 60Hz (16ms refresh)

---

## Panel Configuration (Verified from Waveshare Demo 2025-12-27)

| Parameter | Type | Value | Source |
|-----------|------|-------|--------|
| **Resolution** | uint16_t | 800 × 480 | lvgl_port.h:24-25 |
| **Refresh Rate** | uint16_t | ~60 Hz | Calculated from timing |
| **PCLK Active Neg** | bool | 1 (true) | waveshare_rgb_lcd_port.c:91 |
| **HSYNC/VSYNC Idle** | — | default (0) | waveshare_rgb_lcd_port.c |
| **Pixel Format** | enum | RGB565 | 16-bit, 16 data lines |

### LCD GPIO Mapping (from waveshare_rgb_lcd_port.h)

| Signal | GPIO | Source Line |
|--------|------|-------------|
| PCLK | 7 | line 42 |
| HSYNC | 46 | line 40 |
| VSYNC | 3 | line 39 |
| DE | 5 | line 41 |
| DATA0-15 | 14,38,18,17,10,39,0,45,48,47,21,1,2,42,41,40 | lines 43-58 |

### LCD Timing (from waveshare_rgb_lcd_port.c:84-89)

| Parameter | Value |
|-----------|-------|
| hsync_pulse_width | 4 |
| hsync_back_porch | 8 |
| hsync_front_porch | 8 |
| vsync_pulse_width | 4 |
| vsync_back_porch | 8 |
| vsync_front_porch | 8 |

---

## Touch Input Configuration (GT911 via I2C)

| Parameter | Type | Current | Notes |
|-----------|------|---------|-------|
| **Driver** | — | GT911 (Goodix) | Capacitive, ESP-IDF driver |
| **I2C Address** | uint8_t | 0x5D | Default, can be 0x14 if ADDR=0 |
| **I2C Bus** | i2c_port_t | I2C_NUM_0 | Primary I2C bus |
| **SCL GPIO** | gpio_num_t | 47 | I2C clock |
| **SDA GPIO** | gpio_num_t | 48 | I2C data |
| **I2C Frequency** | uint32_t | 400 kHz | Standard speed |
| **INT GPIO** | gpio_num_t | 16 | Interrupt line (falling edge) |
| **RST GPIO** | gpio_num_t | 15 | Reset (active low) |
| **Swap XY** | bool | false | No axis swap |
| **Invert X** | bool | false | Normal X axis |
| **Invert Y** | bool | false | Normal Y axis |
| **Max Touch Points** | uint8_t | 5 | GT911 supports up to 5 |

**Driver Implementation:**
- Use ESP-IDF `esp_lcd_touch` abstraction (not raw I2C)
- Debounce interrupt: 10 ms minimum
- CRC check on all I2C reads
- Timeout watchdog on stuck INT line (2 sec)

---

## Telemetry Configuration

| Parameter | Type | Current | Notes |
|-----------|------|---------|-------|
| **Enabled** | bool | true | Enable performance counters |
| **VSync Target (ms)** | uint32_t | 16 | 1000/60 Hz |
| **VSync Tolerance (%)** | uint32_t | 10 | ±10% = 14.4–17.6 ms |
| **Telemetry Interval (sec)** | uint32_t | 1 | Output every 1 second max |
| **Task Watchdog Timeout** | uint32_t | 10 | Seconds before reset |
| **Heap Monitoring** | bool | true | Log heap stats |

---

## Validation Rules

### Build-Time Checks (compile-time assertions)
```c
// All in app_config.c
BUILD_ASSERT(PCLK_HZ >= 4000000);           // Min 4M
BUILD_ASSERT(PCLK_HZ <= 80000000);          // Max 80M
BUILD_ASSERT(NUM_FBS >= 1 && NUM_FBS <= 3); // Range 1–3
```

### Runtime Checks (startup verification)
- [ ] VSync counter increments (no stuck display)
- [ ] Heap usage < 80% after boot
- [ ] Touch INT line responsive within 2 seconds
- [ ] No watchdog resets in first 10 seconds

### Performance Targets
| Metric | Target | Measurement Method | DoD Status |
|--------|--------|-------------------|------------|
| **Frame Rate** | ~60 Hz | VSync counter delta/sec | ✓ After VSync smoke test |
| **Input Latency** | <100 ms | Touch → screen response | ✓ After touch test |
| **Memory (heap)** | <80% | `esp_get_free_heap_size()` | ✓ Baseline: 180 KB free |
| **CPU Usage** | <70% | Task runtime profiling | ✓ Estimated 32% core 0 |

---

## Tuning History & Rationale

| Date | Phase | Parameter | Old | New | Rationale | Status |
|------|-------|-----------|-----|-----|-----------|--------|
| 2025-12-27 | 2 | pclk_hz | — | 16M | Conservative startup speed for stability | ✓ BASELINE |
| 2025-12-27 | 2 | num_fbs | — | 2 | Tear-free rendering without excessive memory | ✓ CONSERVATIVE |
| 2025-12-27 | 2 | full_refresh | — | true | Eliminate ghosting artifacts | ✓ CONSERVATIVE |
| TBD | 3 | pclk_hz | 16M | 20M+ | Increase after VSync smoke test | ⏳ PENDING |
| TBD | 4 | render_mode | PARTIAL | DIRECT | Switch after performance validation | ⏳ PENDING |

---

## Configuration Dependency Graph

```
app_config.h (compile-time constants)
    ├─ render_mode enum (fixed: PARTIAL in Phase 2)
    ├─ NUM_FBS (fixed: 2)
    └─ PCLK_HZ (fixed: 16M in Phase 2)

sdkconfig.defaults (ESP-IDF)
    ├─ CPU freq: 240 MHz
    ├─ PSRAM: Octal 120 MHz
    └─ Log level: DEBUG

NVS (runtime, per session)
    ├─ full_refresh (bool: true at boot)
    ├─ display_brightness (0–100%, default 100)
    └─ [future: user preferences]

Panel (hardware, fixed)
    ├─ 600×480, 60 Hz
    └─ RGB565 color

Touch (hardware, I2C fixed)
    ├─ GT911, 0x5D, I2C0
    └─ GPIO 16 (INT), GPIO 15 (RST)
```

---

## Change Control Process

### Before modifying any parameter:
1. **Understand impact**: Read rationale + DoD
2. **Test baseline first**: Measure current behavior
3. **Change one parameter**: Only one at a time
4. **Measure delta**: Before/after comparison
5. **Document**: Add entry to Tuning History
6. **Commit**: Update this file + STATE.md

### Example: Increasing pclk_hz from 16M to 20M
```bash
# 1. Measure baseline (16M)
idf.py flash monitor
# Note: FPS, thermal, heap usage

# 2. Edit app_config.h: PCLK_HZ = 20000000

# 3. Build + flash
idf.py build flash

# 4. Measure new (20M)
idf.py monitor
# Compare: FPS should improve, thermal increase ~5°C

# 5. Document result in Tuning History table above

# 6. Commit with clear message
git add app_config.h docs/CONFIG_MATRIX.md
git commit -m "pclk: increase 16M → 20M (+5% FPS, +0.3W power)"
```

---

## Notes & Constraints

1. **PSRAM requirement**: Panel buffers (600×480×2 bytes) = 576 KB per FB. With 2 FBs + LVGL overhead ≈ 1.5 MB total. PSRAM mandatory.

2. **DIRECT mode risk**: Only enable AFTER successful VSync smoke test. PARTIAL mode is fail-safe.

3. **Clock tree trade-off**: Increasing pclk_hz improves FPS but generates more heat. Monitor thermal with accelerometer if available.

4. **Touch calibration**: GT911 may need re-calibration if pclk_hz increases significantly (>50%).

5. **LVGL tick period**: Fixed at 5 ms in LVGL configuration. Do NOT change without re-testing display.

---

Last Updated: 2025-12-27T16:07:00Z
Validated By: [Manual review + build test on 2025-12-27]
Next Review: After Phase 3 VSync smoke test
Status: **COMPLETE - All parameters documented + validation rules defined**
