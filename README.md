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

🚧 **Work in Progress** - Building debug infrastructure

- [ ] App config & NVS system
- [ ] LVGL port with callbacks
- [ ] Mode 3 validation & VSync gating
- [ ] Debug telemetry infrastructure
- [ ] Mode2 vs Mode3 comparison tests
