# Waveshare 4.3" RGB LCD + ESP32-S3 + LVGL 9.4
## Mode 3 (DIRECT) Debug & Optimization

**Hardware:**
- ESP32-S3 (dual-core, 240 MHz)
- Waveshare 4.3" RGB LCD (600×480, RGB565)
- 2x PSRAM, internal DMA

**Firmware Stack:**
- ESP-IDF 5.5
- LVGL 9.4 (managed component)
- EEZ Framework (UI framework)

**Focus:**
- Mode 3 (DIRECT) rendering optimization
- Tear-free synchronization (VSync gating)
- Real-time debug telemetry (1-second metrics logging)
- Runtime configuration (NVS-backed app_config)

## Project Structure

```
.
├── main/                          # Main app entry point
├── components/
│   ├── app_config/               # Runtime configuration (NVS)
│   ├── board_waveshare_rgb/      # LVGL port + RGB LCD driver
│   ├── eez_ui/                   # EEZ Framework UI
│   └── [third-party libs]/
├── CMakeLists.txt                # ESP-IDF build config
├── sdkconfig*                    # IDF Kconfig
├── partitions.csv                # Flash partition table
└── docs/
    ├── MODE3_DEBUG_GUIDE.md       # Debugging methodology
    └── ARCHITECTURE.md             # System design
```

## Build & Flash

```bash
# Install ESP-IDF 5.5 and dependencies
source $IDF_PATH/export.sh

# Configure
idf.py menuconfig

# Build
idf.py build

# Flash + Monitor
idf.py flash monitor
```

## Debug Output (Serial Log)

Every 1 second:
```
[DEBUG] Mode3 VSync:60 Bounce:60 Flush:50 AvgTime:1.234 ms MaxTime:7.456 ms
```

Metrics:
- **VSync**: True LCD frame-refresh events per second
- **Bounce**: Bounce-buffer DMA completions per second
- **Flush**: LVGL flush_callback invocations per second
- **AvgTime**: Average flush_callback duration (ms)
- **MaxTime**: Peak flush_callback duration (ms)

## Status

🚧 **Work in Progress** - Phase 2: Foundation Build Complete

- [x] Context-Safety System (checkpoint.py, Makefile, documentation gates)
- [x] Foundation Build (CMakeLists.txt, sdkconfig, components stubbed)
- [ ] Phase 3: Component Implementation (board_lcd, board_touch, board_telemetry)
- [ ] VSync Smoke Test (must pass before DIRECT mode upgrade)

---

## Session Restart Checklist

**If context interrupted or session resumed:**

1. **Read Status Files**
   ```bash
   git status
   git log -1 --oneline
   cat docs/STATE.md      # Current baseline config
   cat docs/TODO.md       # Top 10 tasks with DoD
   cat docs/HANDOFF.md    # If budget was low
   ```

2. **Check Budget**
   ```bash
   make check-budget
   ```

3. **Continue Work**
   - Next task listed in docs/TODO.md (Priority 3+)
   - Run `make checkpoint` after major change
   - Run `make test-build` before commit

4. **Commit Gate**
   ```bash
   # Before git commit:
   make checkpoint              # Updates STATE.md + TODO.md
   git add docs/ scripts/ Makefile
   git commit -m "..."
   ```

---

## Context-Safety System

**Goal:** Prevent context loss when token budget exhausted.

**Files:**
- `docs/STATE.md` - Current config + session deltas
- `docs/TODO.md` - Ranked tasks with definition of done
- `docs/HANDOFF.md` - Auto-generated recovery file
- `scripts/checkpoint.py` - Update system
- `Makefile` - Targets: checkpoint, handoff, check-budget

**Usage:**
```bash
make checkpoint              # Interactively update STATE.md with delta
make check-budget            # Check doc files exist, show status
make handoff                 # Generate HANDOFF.md for emergency restart
```

---

## Building Phase 3

After Phase 2 (current) is complete:

1. Create `components/board_waveshare_rgb/board_lcd.c` (RGB panel init, LVGL flush callback, VSync gating)
2. Create `components/board_waveshare_rgb/board_touch.c` (GT911 driver, LVGL input)
3. Create `components/board_waveshare_rgb/board_telemetry.c` (1-second task, atomic counters, logging)
4. **VSync Smoke Test**: Verify VSync counter ~60Hz delta/sec, STOP if 0
5. Flash tests, baseline measurements

See `docs/TODO.md` for detailed task list with DoD.
