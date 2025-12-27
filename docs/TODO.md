# Task List - 2025-12-27

## Priority 1: Context-Safety (Gate System)

- [x] **Create scripts/checkpoint.py** - DONE
  - DoD: Script updates STATE.md with timestamp + delta, updates TODO.md top 10, generates HANDOFF.md when budget low
  - Completed: 2025-12-27T13:50:00Z

- [x] **Create Makefile with targets** - DONE
  - DoD: make checkpoint, make handoff, make check-budget all work
  - Completed: 2025-12-27T13:50:00Z

- [x] **Create docs/HANDOFF.md template** - DONE
  - DoD: Template file exists, can be filled manually or auto-generated
  - Completed: 2025-12-27T13:50:00Z

## Priority 2: Foundation Build (15 files)

- [x] **Create CMakeLists.txt + idf_component.yml** - DONE
  - DoD: Root build config defines component dirs, LVGL 9.4 dependency set
  - Completed: 2025-12-27T13:50:00Z (removed idf_component.yml, LVGL for Phase 3)

- [x] **Create sdkconfig.defaults** - DONE
  - DoD: ESP32-S3, PSRAM octal 120MHz, LOG_MAXIMUM_LEVEL_DEBUG, partitions configured
  - Completed: 2025-12-27T13:50:00Z

- [x] **Create partitions.csv** - DONE
  - DoD: NVS 24KB, factory 2816K, SPIFFS 1280K (total ~4MB used of 16MB)
  - Completed: 2025-12-27T13:50:00Z

- [x] **Create main/ + components structure** - DONE
  - DoD: main/CMakeLists.txt, main/main.c (NVS init stub), app_config/, board_waveshare_rgb/, eez_ui/ all with basic CMakeLists + header files
  - Completed: 2025-12-27T13:50:00Z

- [x] **Create app_config.h + app_config.c** - DONE
  - DoD: render_mode enum (PARTIAL/DIRECT) SEPARATE from full_refresh bool, defaults to PARTIAL+true+2FBs+16MHz
  - Completed: 2025-12-27T13:50:00Z

- [x] **Build test: idf.py build** - PASSED
  - DoD: Exit code 0, build/waveshare_rgb_lcd.bin created, < 1.5 MB
  - Result: 217 KB firmware, 79% space free in partition
  - Completed: 2025-12-27T13:50:00Z

## Priority 2.5: Session Restart Safety (COMPLETE)

- [x] **Implement Hooks + Fallbacks System** - DONE [2025-12-27]
  - DoD: SessionStart/SessionEnd hooks configured (Claude Code native), Makefile targets work, README documented
  - Completed: 2025-12-27T14:15:00Z
  - Result:
    - SessionStart (startup): auto-loads context on new session
    - SessionStart (resume): auto-loads context on resume
    - SessionEnd: auto-updates STATE.md+TODO.md on exit
    - Fallback: `make session-start` runs all 5 restart commands

## Priority 3: Documentation Infrastructure

- [ ] **Initialize docs templates (7 files)**
  - DoD: docs/templates/{STATE,TODO,DECISIONS,TESTS,CONFIG_MATRIX,MEASUREMENTS}.template.md exist
  - Dependencies: None
  - Complexity: LOW

- [ ] **Populate docs/CONFIG_MATRIX.md**
  - DoD: ESP-IDF Kconfig table filled, NVS runtime config table with render_mode/full_refresh separation, validation rules documented
  - Dependencies: docs/templates/CONFIG_MATRIX.template.md
  - Complexity: LOW

## Backlog (Phase 3+)

- [ ] board_lcd.c (RGB panel init, LVGL flush callback, VSync gating)
- [ ] board_touch.c (GT911 driver, LVGL input device)
- [ ] board_telemetry.c (1-second task, atomic counters)
- [ ] VSync Smoke Test (verify ~60Hz delta/sec, STOP if 0)
- [ ] Optimize pclk_hz, bounce_buffer_height, num_fbs

---
Last Updated: 2025-12-27T14:12:38Z
Status: Phase 2.5 COMPLETE - Ready for Phase 3 (documentation templates)
