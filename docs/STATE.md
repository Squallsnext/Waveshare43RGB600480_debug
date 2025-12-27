# Project State - 2025-12-27

TODO lives at docs/TODO.md
History: docs/state_history/STATE_20251227.md (archived deltas)

## Current Status
- Build Status: PASSING
- Last Successful Build: 2025-12-27T17:30:00Z (222 KB firmware)
- Runtime Status: NOT_FLASHED (awaiting VSync smoke test)

## Session Goal
Phase 4: Board Component Implementation (LCD/LVGL Foundation)

## Baseline Configuration
```
render_mode:           RENDER_MODE_PARTIAL
full_refresh:          true
num_fbs:               2
pclk_hz:               16,000,000 (16 MHz)
bounce_buffer_height:  20
resolution:            800x480 RGB565
telemetry_enabled:     true
```

## What Works
- GitHub repository initialized
- Basic project structure in place
- Context-safety hooks (SessionStart/End)
- Documentation templates (Phase 3.0)
- Board component skeleton (Phase 4.0)
- LCD panel initialization code (Phase 4.1)
- Build passes (222 KB, 79% partition free)

## What's Blocked
- VSync Smoke Test: Must flash and verify vsync_delta > 0

## Component Status
| Component | Status | Notes |
|-----------|--------|-------|
| app_config | DONE | Baseline config defined |
| board_waveshare_rgb | DONE | LCD init, pins verified from Waveshare demo |
| eez_ui | STUB | Pending Phase 5 |

## Architecture Rules
1. No mystical fixes - Every change has measurement + rationale
2. No deadlock risk - lv_disp_flush_ready() exactly 1x per flush
3. No log spam - Telemetry via counters, logging max 1x/sec
4. Single source of truth - sdkconfig + app_config (NVS)
5. Touch stability - Use ESP-IDF esp_lcd_touch GT911
6. Stability first - PARTIAL + full_refresh=true + 2 FBs
7. VSync gate - Counter must show ~60Hz, if 0 STOP
8. Doc gate - STATE.md + TODO.md before commit


## Session Delta [2025-12-27T17:19:03Z]
hook: auto snapshot

## Session Delta [2025-12-27T17:19:17Z]
hook: auto snapshot

## Session Delta [2025-12-27T17:19:53Z]
hook: auto snapshot
## Critical Context
- Verified pins from Waveshare demo (08_lvgl_Porting)
- CH422G IO expander for backlight (0x24/0x38)
- GT911 touch at 0x5D, polling mode (INT=-1)
- Conservative pclk (16MHz) for stability

---

## Recent Deltas (last 5)

### [2025-12-27T17:45] STATE.md Rotation
- Archived 407 lines to docs/state_history/STATE_20251227.md
- Reduced STATE.md from 446 to ~90 lines

### [2025-12-27T17:30] Phase 4.1 LCD Init Complete
- board_pins.h: All 16 RGB data pins from Waveshare demo
- board_lcd.c: pclk_active_neg=1, correct timing
- Build: 222 KB (PASS)

### [2025-12-27T16:15] Phase 3.0 Documentation
- Created docs/templates/ (7 templates)
- Populated CONFIG_MATRIX.md

### [2025-12-27T14:25] Phase 2.5 Native Hooks
- .claude/hooks/ with startup/resume/cleanup scripts

### [2025-12-27T13:56] Initial Checkpoint
- Project structure established

---

Last Updated: 2025-12-27T17:19:53ZT17:19:17ZT17:19:03ZT17:45:00Z
Branch: wip/20251227-1524
