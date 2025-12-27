# Test Plan

## Phase 2 Build Tests (CURRENT)

### TEST-BUILD-001: Clean Build Success
- **Command:** `idf.py fullclean && idf.py build`
- **Expected:** Build succeeds, firmware < 1.5 MB, exit code 0
- **Last Result:** PASS (2025-12-27T13:50:00Z, 217 KB firmware)
- **Status:** PASS

### TEST-BUILD-002: Reproducible Build
- **Command:** `make test-build` (fullclean + build)
- **Expected:** Same binary size, no warnings
- **Status:** NOT_RUN

### TEST-BUDGET-001: Context Safety Check
- **Command:** `make check-budget`
- **Expected:** All doc files exist, STATUS: OK
- **Last Result:** PASS (2025-12-27T14:00:00Z)
- **Status:** PASS

### TEST-CHECKPOINT-001: Checkpoint System
- **Command:** `make checkpoint` (input: "test delta")
- **Expected:** STATE.md + TODO.md updated with timestamp
- **Last Result:** PASS (2025-12-27T14:00:00Z, delta added)
- **Status:** PASS

---

## Phase 3 Runtime Tests (PLANNED)

### TEST-RUNTIME-001: Boot Sequence
- **Steps:** `idf.py flash monitor`
- **Expected:**
  ```
  I (XXX) MAIN: Waveshare RGB LCD - Starting...
  I (XXX) MAIN: NVS initialized
  I (XXX) APP_CONFIG: Init - mode=0, full_refresh=1, num_fbs=2
  I (XXX) BOARD: Init - mode=0, full_refresh=1
  I (XXX) EEZ_UI: Init (stub - user provides EEZ Studio generated assets)
  I (XXX) MAIN: Entering idle loop
  I (XXX) MAIN: Heartbeat
  ```
- **Pass:** No crashes, serial output appears
- **Status:** NOT_RUN

### TEST-TELEMETRY-001: VSync Smoke Test
- **Steps:** Monitor serial for 10+ seconds
- **Expected:** `[TELEMETRY] VSync:60 Bounce:60 Flush:50 AvgTime:X ms MaxTime:Y ms` (or similar)
- **Pass Criteria:**
  - VSync counter != 0 (STOP if 0, fix root cause)
  - VSync ~60 Hz (within 50-70 Hz range)
  - No crashes, logs consistent
- **Status:** NOT_RUN
- **GATE:** Must PASS before upgrading to DIRECT mode

### TEST-CONFIG-001: NVS Load/Save
- **Steps:** (Phase 3) Load config from NVS, modify, save, reload
- **Expected:** Values persist across reset
- **Status:** NOT_RUN

### TEST-TOUCH-001: GT911 Responsiveness
- **Steps:** (Phase 3) Touch screen, watch for log output
- **Expected:** Touch events logged, CPU not stuck at 100%
- **Status:** NOT_RUN

---

## Performance Baseline (PLANNED)

### Metric Collection (Phase 3)
Record when VSync smoke test passes:
- VSync Rate (Hz)
- Bounce Rate (Hz)
- Flush Rate (Hz)
- Avg Flush Time (ms)
- Max Flush Time (ms)
- Heap Free (bytes)
- CPU Load (%)

---
Last Updated: 2025-12-27T14:00:00Z
