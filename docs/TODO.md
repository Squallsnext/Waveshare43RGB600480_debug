# Task List - 2025-12-27

## Testing: Auto-Snapshot System
- Testing Phase 3: Auto-Commit + Auto-Push via Claude Code Hooks

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

## Priority 2.5: Session Restart Safety (✅ COMPLETE - VERIFIED)

- [x] **Implement Hooks + Fallbacks System** - ✅ VERIFIED [2025-12-27]
  - DoD: 3 Hooks + 4 Scripts + Makefile targets + README documented + all verified working
  - Completed: 2025-12-27T14:25:38Z
  - Implementation:
    - **SessionStart (startup)**: `.claude/hooks/startup.sh` loads full git+budget+STATE+TODO+HANDOFF on new session
    - **SessionStart (resume)**: `.claude/hooks/resume.sh` reloads git+budget+TODO on resume
    - **SessionEnd**: `.claude/hooks/cleanup.sh` updates STATE.md+TODO.md, generates HANDOFF.md if budget low
    - **PostToolUse**: `.claude/hooks/post-tool-budget.sh` monitors budget every 10 tool calls
    - **Fallback**: `make session-start` runs all 5 restart commands, `make checkpoint` runs cleanup
  - Verification: All 4 hooks tested + working (100% pass rate)
  - Status: HOOKS NOW LIVE - next session will auto-initialize with full context

## Priority 3: Documentation Infrastructure (✅ COMPLETE)

- [x] **Initialize docs templates (7 files)** - ✅ DONE [2025-12-27T16:07]
  - DoD: docs/templates/{STATE,TODO,DECISIONS,TESTS,CONFIG_MATRIX,MEASUREMENTS,COMPONENTS}.template.md exist
  - Completed: 2025-12-27T16:07:00Z
  - Templates auto-committed via PostToolUse hook (7 separate snapshots)

- [x] **Populate docs/CONFIG_MATRIX.md** - ✅ DONE [2025-12-27T16:13]
  - DoD: ESP-IDF Kconfig table filled ✓, NVS runtime config with render_mode/full_refresh separation ✓, validation rules documented ✓
  - Completed: 2025-12-27T16:13:00Z
  - Includes: Tuning history, dependency graph, change control process, constraints

## Backlog (Phase 3+)

- [ ] board_lcd.c (RGB panel init, LVGL flush callback, VSync gating)
- [ ] board_touch.c (GT911 driver, LVGL input device)
- [ ] board_telemetry.c (1-second task, atomic counters)
- [ ] VSync Smoke Test (verify ~60Hz delta/sec, STOP if 0)
- [ ] Optimize pclk_hz, bounce_buffer_height, num_fbs

---
Last Updated: 2025-12-27T16:50:49Z 2025-12-27T16:49:41Z 2025-12-27T16:49:23Z 2025-12-27T16:44:28Z 2025-12-27T16:44:16Z 2025-12-27T16:43:58Z 2025-12-27T16:43:03Z 2025-12-27T16:42:43Z 2025-12-27T16:42:43Z 2025-12-27T16:42:32Z 2025-12-27T16:42:32Z 2025-12-27T16:38:46Z 2025-12-27T16:38:46Z 2025-12-27T16:27:48Z 2025-12-27T16:14:40Z 2025-12-27T16:14:40Z 2025-12-27T16:13:27Z 2025-12-27T16:13:27Z 2025-12-27T16:13:08Z 2025-12-27T16:13:08Z 2025-12-27T16:07:49Z 2025-12-27T16:07:49Z 2025-12-27T16:07:26Z 2025-12-27T16:07:26Z 2025-12-27T16:07:10Z 2025-12-27T16:07:10Z 2025-12-27T16:06:57Z 2025-12-27T16:06:57Z 2025-12-27T16:06:50Z 2025-12-27T16:06:50Z 2025-12-27T16:06:45Z 2025-12-27T16:06:45Z 2025-12-27T16:06:40Z 2025-12-27T16:06:40Z 2025-12-27T15:17:45Z 2025-12-27T15:03:28Z 2025-12-27T14:25:38Z 2025-12-27T14:12:38Z
Status: Phase 2.5 COMPLETE - Ready for Phase 3 (documentation templates)
