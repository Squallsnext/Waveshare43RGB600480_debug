# Performance Measurements

## Baseline Configuration
```
render_mode:           RENDER_MODE_PARTIAL
full_refresh:          true
num_fbs:               2
pclk_hz:               16,000,000 (16 MHz)
bounce_buffer_height:  20
resolution:            800x480 RGB565
```

## Build Metrics

| Date | Build | Size | Status |
|------|-------|------|--------|
| 2025-12-27 | Phase 4.1 LCD Init | 222 KB (79% free) | PASS |

## Runtime Metrics (NOT YET FLASHED)

| Metric | Target | Measured | Status |
|--------|--------|----------|--------|
| VSync delta/sec | ~60 | TBD | PENDING |
| Bounce delta/sec | ~60 | TBD | PENDING |
| Flush delta/sec | >0 | TBD | PENDING |
| Avg flush time | <16ms | TBD | PENDING |
| Max flush time | <20ms | TBD | PENDING |
| Free heap | >100KB | TBD | PENDING |
| PSRAM free | >4MB | TBD | PENDING |

## VSync Smoke Test

**Status:** NOT YET RUN

**Gate Criteria:**
- VSync delta/sec must be > 0 (ideally ~60)
- Bounce delta/sec must be > 0 (if bounce buffer enabled)
- If either is 0: STOP and diagnose

**Next Steps:**
1. `idf.py flash monitor`
2. Wait 10 seconds for telemetry output
3. Record VSync/Bounce/Flush values
4. If VSync=0: Check callback registration, panel init, GPIO config

---
Last Updated: 2025-12-27T17:30:00Z
