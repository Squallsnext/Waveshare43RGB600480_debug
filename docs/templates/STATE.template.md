# Project State - [TIMESTAMP]
TODO lives at docs/TODO.md

## Current Status
- Build Status: [PASSING|FAILING|UNKNOWN]
- Last Successful Build: [TIMESTAMP] ([SIZE] firmware)
- Runtime Status: [NOT_FLASHED|FLASHING|RUNNING|ERROR]

## Session Goal
[One-sentence description of current phase objective]

## Baseline Configuration
```
render_mode:           [RENDER_MODE_PARTIAL|RENDER_MODE_DIRECT]
full_refresh:          [true|false]
num_fbs:               [1|2|3]
pclk_hz:               [frequency in Hz]
bounce_buffer_height:  [pixels]
telemetry_enabled:     [true|false]
```

## What Works
- [Item 1]
- [Item 2]
- [Item 3]

## What's Blocked
- [Blocker 1] - blocked by [reason]
- [Blocker 2] - blocked by [reason]

## Component Status
### app_config
- Status: [STUB|IN_PROGRESS|COMPLETE|TESTING|VERIFIED]
- Last update: [TIMESTAMP]

### board_waveshare_rgb
- Status: [STUB|IN_PROGRESS|COMPLETE|TESTING|VERIFIED]
- Last update: [TIMESTAMP]

### board_touch
- Status: [STUB|IN_PROGRESS|COMPLETE|TESTING|VERIFIED]
- Last update: [TIMESTAMP]

### eez_ui
- Status: [STUB|IN_PROGRESS|COMPLETE|TESTING|VERIFIED]
- Last update: [TIMESTAMP]

## Architecture Rules Applied
1. No mystical fixes - Every change has measurement + rationale
2. No deadlock risk - lv_disp_flush_ready() called exactly once per flush
3. No log spam - Telemetry via counters, logging max 1x/second
4. Single source of truth - sdkconfig + app_config (NVS)
5. Touch stability - Use ESP-IDF esp_lcd_touch GT911
6. Stability before performance - Start PARTIAL + full_refresh=true + 2 FBs
7. VSync Smoke Test - VSync counter must show ~60Hz delta/sec, if 0 STOP
8. Documentation Gate - STATE.md + TODO.md updated before commit

## Critical Context
- [Key constraint 1]
- [Key constraint 2]
- [Measurement or decision 1]

## Session Deltas
[Automatically filled by checkpoint.py]

---
Last Updated: [AUTO-FILLED BY CHECKPOINT]
Branch: [AUTO-FILLED BY CHECKPOINT]
Commit: [AUTO-FILLED BY CHECKPOINT]
