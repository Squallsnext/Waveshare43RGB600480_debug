# Architecture Decision Records

## ADR-001: Separate render_mode from full_refresh parameter
**Date:** 2025-12-27
**Status:** ACCEPTED

### Context
LVGL rendering configuration traditionally conflates buffer strategy (PARTIAL vs DIRECT mode) with refresh strategy (full screen vs dirty rects). This led to confusion and configuration errors.

### Decision
Split into two independent parameters:
- `render_mode` enum: PARTIAL (0) or DIRECT (1) - controls algorithm
- `full_refresh` bool: true/false - controls refresh strategy

### Consequences
- Clearer API, less configuration confusion
- Validation rules: DIRECT mode requires num_fbs >= 2
- Allows safe upgrade path: start with PARTIAL+full_refresh=true, upgrade to DIRECT after VSync smoke test passes

---

## ADR-002: Stability-First Approach (Phase 2)
**Date:** 2025-12-27
**Status:** ACCEPTED

### Context
Previous sessions experienced WDT crashes due to aggressive DIRECT mode + low framebuffer count. Need proven stable baseline before performance optimization.

### Decision
Phase 2 baseline:
- render_mode = PARTIAL (not DIRECT)
- full_refresh = true (full screen refresh)
- num_fbs = 2 (double-buffer minimum)
- pclk_hz = 16MHz (conservative, not 18MHz)

VSync Smoke Test gate: counter must show ~60Hz delta/sec, STOP immediately if 0

### Consequences
- Slower initial rendering (~60fps, not potential 120fps with DIRECT)
- Proven stability without deadlocks or WDT timeouts
- Foundation for safe upgrade to DIRECT mode after verification

---

## ADR-003: Context-Safety System
**Date:** 2025-12-27
**Status:** ACCEPTED

### Context
Previous sessions lost context when token budget exhausted. Need automated documentation + handoff mechanism.

### Decision
Implement checkpoint system:
- `scripts/checkpoint.py`: Update STATE.md/TODO.md with timestamp + delta, generate HANDOFF.md when budget low
- `Makefile` targets: checkpoint, handoff, check-budget
- Documentation gates: STATE.md + TODO.md MUST be updated before each commit
- HANDOFF.md: Auto-generated recovery file for session restart

### Consequences
- Session context preserved across interruptions
- Clear "restart here" pointer (STATE.md → TODO.md → HANDOFF.md)
- Audit trail of changes (delta blocks with ISO timestamps)

---
Last Updated: 2025-12-27T14:00:00Z
