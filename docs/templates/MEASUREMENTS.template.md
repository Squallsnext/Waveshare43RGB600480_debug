# Performance & Telemetry Measurements

## Frame Rate & VSync

### VSync Delta Measurements
| Date | pclk_hz | fps_target | vsync_delta_ms | fps_actual | drift | Status |
|------|---------|-----------|----------------|-----------|-------|--------|
| [Date] | 16M | 60 | 16 ± ? | ? | ? | [PASS/FAIL] |
| [Date] | 20M | 60 | 16 ± ? | ? | ? | [PASS/FAIL] |
| [Date] | 24M | 60 | 16 ± ? | ? | ? | [PASS/FAIL] |

**How to measure:**
```bash
# Monitor telemetry output (1 sec interval)
idf.py monitor | grep -i "vsync"
# Expected: "VSync: NNN Hz" where NNN ≈ 60
```

---

## Memory Usage

### Heap Analysis
| Component | Allocated (KB) | Peak (KB) | Type | Status |
|-----------|----------------|-----------|------|--------|
| app_config | ? | ? | Static | [MEASURE] |
| LVGL | ? | ? | Dynamic | [MEASURE] |
| FreeRTOS | ? | ? | Static | [MEASURE] |
| Display Driver | ? | ? | Static | [MEASURE] |
| Touch Driver | ? | ? | Static | [MEASURE] |
| **Total** | **?** | **?** | | |

**Measurement command:**
```c
printf("Free heap: %u KB\n", esp_get_free_heap_size() / 1024);
printf("Largest free block: %u KB\n", heap_caps_get_largest_free_block(MALLOC_CAP_DEFAULT) / 1024);
```

---

## Latency Measurements

### Input → Output Latency
| Scenario | Measurement | Expected | Delta | Status |
|----------|-------------|----------|-------|--------|
| Touch press → screen response | ? ms | <100 ms | ? | [MEASURE] |
| Frame submission → display flush | ? ms | <16 ms (@ 60Hz) | ? | [MEASURE] |
| Button press → ISR → callback | ? ms | <1 ms | ? | [MEASURE] |

**How to measure:**
```c
// Timestamp touch event in GT911 ISR
// Timestamp LVGL callback
// Delta = (callback_time - isr_time)
```

---

## CPU Utilization

### Task CPU Time
| Task | Core | Load (%) | Peak (%) | Status |
|------|------|----------|----------|--------|
| display_refresh | Core 0 | ? | ? | [MEASURE] |
| touch_input | Core 1 | ? | ? | [MEASURE] |
| telemetry | Core 1 | ? | ? | [MEASURE] |
| **Total CPU** | | **?** | **?** | |

**How to measure:**
```c
// Use FreeRTOS task statistics
vTaskGetRunTimeStats(buffer);
printf(buffer);
```

---

## Power Consumption (if measured)

| State | Current (mA) | Voltage (V) | Power (W) | Notes |
|-------|--------------|-------------|-----------|-------|
| Idle (no backlight) | ? | 5.0 | ? | [MEASURE] |
| Display 50% brightness | ? | 5.0 | ? | [MEASURE] |
| Display 100% brightness | ? | 5.0 | ? | [MEASURE] |
| Touch active | +? | 5.0 | +? | [MEASURE] |

---

## Thermal (if monitored)

| Temperature Sensor | Idle (°C) | Active (°C) | Limit (°C) | Status |
|-------------------|-----------|------------|-----------|--------|
| ESP32-S3 internal | ? | ? | 120 | [NOT_MONITORED] |
| Display panel | ? | ? | 80 | [NOT_MONITORED] |

---

## Display Quality

### Ghosting / Tearing
| render_mode | full_refresh | Ghosting | Tearing | Notes |
|-----------|--------------|----------|--------|-------|
| PARTIAL | true | None observed | None | Baseline |
| PARTIAL | false | Slight | None | [TEST] |
| DIRECT | true | None | Possible | [TEST] |
| DIRECT | false | None | Likely | [TEST] |

---

## Stability Metrics

### Uptime & Crashes
| Test Duration | Crashes | Hangs | Watchdog Resets | Status |
|---------------|---------|-------|-----------------|--------|
| [Duration] | ? | ? | ? | [IN_PROGRESS] |

**Test procedure:**
```bash
# Flash and monitor for N hours
# Log any restarts or anomalies
idf.py flash monitor
```

---

## Measurement Log

### Session [Date/Time]
```
pclk_hz: 16M
render_mode: PARTIAL
full_refresh: true
num_fbs: 2

Measurements:
- VSync: 60.2 Hz (±0.5)
- Free heap: 180 KB
- Peak heap: 245 KB
- Touch latency: 45 ms
- CPU load: 32%

Notes:
[Any observations, anomalies, or next steps]
```

---

Last Updated: [TIMESTAMP]
Next Measurement Session: [DATE]
Target Metrics for Optimization: [pclk tuning, memory reduction, etc.]
