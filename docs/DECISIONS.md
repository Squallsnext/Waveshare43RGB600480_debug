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

## ADR-004: Auto-Snapshot via Claude Code Hooks
**Date:** 2025-12-27
**Status:** ACCEPTED

### Context
Manual `git add . && git commit` after each change is friction-heavy and error-prone. Need automatic commit system that:
- Commits after every file modification (via Claude Code hooks)
- Prevents accidental commits to main branch
- Avoids large build artifacts in commits
- Pushes to WIP branch only when session ends
- Works seamlessly without token overhead

### Decision
Implement auto-snapshot system with three Claude Code hooks:

1. **SessionStart Hook** (`startup.sh`)
   - Detects if on main/master, creates WIP branch if needed
   - Prevents direct main commits (each session gets own wip/YYYYMMDD-HHMM branch)

2. **PostToolUse Hook** (`post_tool_use.sh` + `auto_snapshot.sh`)
   - Triggers after Write/Edit/NotebookEdit/TodoWrite
   - Runs checkpoint (minimal STATE.md + TODO.md delta)
   - Selectively stages changes (excludes build/, *.bin, *.elf, etc)
   - Creates auto-commit with format: `auto: checkpoint YYYY-MM-DD HH:MM (N files) – <summary>`

3. **SessionEnd Hook** (`cleanup.sh` + `auto_push_if_needed.sh`)
   - Final checkpoint before session ends
   - Pushes to origin/wip/* if ahead of remote
   - Generates HANDOFF.md if budget low
   - Never pushes from main/master

### Consequences

**Pros:**
- ✅ Zero manual commits required — all changes captured automatically
- ✅ main branch stays clean — impossible to commit directly to main
- ✅ Token-sparse — hooks are lightweight, minimal I/O
- ✅ Rollback-safe — every edit becomes a commit, easy recovery
- ✅ Non-destructive — never uses `git clean -fd`, never force-pushes
- ✅ Graceful degradation — silent no-op if outside repo or no origin

**Cons:**
- ⚠️ More commits in history (one per file change)
- ⚠️ Requires Claude Code hook support (not portable to other editors)
- ⚠️ Build artifacts must be manually excluded (git ignore can help)

**Mitigations:**
- Exclude common artifacts in auto_snapshot.sh: build/, managed_components/, sdkconfig*, *.bin, *.elf, *.map
- Can disable auto-snapshot by commenting PostToolUse hook in .claude/settings.json
- Manual equivalent: `make auto-snapshot` if hooks fail

### Trade-offs Considered

| Alternative | Pros | Cons |
|---|---|---|
| **Auto-Snapshot (chosen)** | Automatic, safe, minimal overhead | More commits in history |
| Manual checkpoint after task | Cleaner history | Requires discipline, loses work if forgotten |
| Pre-commit hook (git hook) | Works in any editor | Can't distinguish "should commit" from "still editing" |
| Scheduled background commits | Batches work | Loses real-time recovery |

### Implementation Details

**Files:**
- `scripts/auto_snapshot.sh` — Main logic: checkpoint + git add + git commit
- `scripts/auto_push_if_needed.sh` — Safe push to WIP branch only
- `.claude/hooks/post_tool_use.sh` — Hook entry point for PostToolUse
- `.claude/settings.json` — Hook configuration

**Safety Checks:**
```bash
# All scripts implement:
repo_root="$(git rev-parse --show-toplevel 2>/dev/null)" || exit 0
cd "$repo_root" || exit 0
```

This ensures hooks are:
- Repo-location-agnostic (work from any directory)
- Silent no-op if not in a git repo
- Non-destructive on network errors

---

## ADR-005: Non-Interactive checkpoint-auto for Hooks
**Date:** 2025-12-27
**Status:** ACCEPTED

### Context
The PostToolUse hook runs `scripts/auto_snapshot.sh`, which originally called `make checkpoint`. The `checkpoint` target uses `read -p "Describe change..."` to prompt for user input. This caused hooks to hang indefinitely waiting for input that never comes (hooks run non-interactively).

### Problem
```
make checkpoint
# Hangs on: "Describe change (one line): "
# Hook timeout → session blocked
```

### Decision
Add a separate `checkpoint-auto` Makefile target:
- **`checkpoint`** (manual): Interactive, prompts for description via `read -p`
- **`checkpoint-auto`** (hooks): Non-interactive, uses `$AUTO_DESC` env var or default "auto snapshot"

Update `auto_snapshot.sh` to call `make checkpoint-auto` instead of `make checkpoint`.

### Implementation
```makefile
# Makefile
checkpoint-auto:
	@python3 scripts/checkpoint.py update-state "$${AUTO_DESC:-auto snapshot}" 2>/dev/null || true
	@python3 scripts/checkpoint.py update-todo 2>/dev/null || true
```

```bash
# auto_snapshot.sh
AUTO_DESC="hook: auto snapshot" make checkpoint-auto 2>/dev/null || true
```

### Consequences
- Hooks run without blocking (~0.03s execution time)
- Manual `make checkpoint` remains interactive for human use
- Commit messages from hooks have prefix "hook:" for easy identification
- No changes to checkpoint.py required

---

## ADR-006: STATE.md Rotation Policy
**Date:** 2025-12-27
**Status:** ACCEPTED

### Context
STATE.md grew to 446 lines / 258 KB due to auto-snapshot hooks appending deltas. This exceeded Claude's Read tool token limit (25K tokens), making the file unreadable and defeating its purpose as quick context reference.

### Decision
Implement STATE rotation policy:

**Limits:**
- STATE.md max 200 lines OR 12 KB (whichever first)
- Keep only last 5 meaningful deltas

**Rotation:**
- When limit exceeded: move older deltas to `docs/state_history/STATE_YYYYMMDD.md`
- History files are append-only archives
- STATE.md header references history file location

**Token-Safe Audit:**
- Use Bash tools (wc, head, tail, grep) instead of Read for large files
- Never attempt full Read on files >25K tokens

### Consequences
- STATE.md remains readable in single Read call
- Full history preserved in state_history/
- Hooks continue appending; rotation happens when needed
- Rule of thumb: rotate after ~50 hook deltas

---
Last Updated: 2025-12-27T17:45:00Z
