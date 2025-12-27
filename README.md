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

## How to Restart Session Safely

**Automatic (via Claude Code Hooks):**
- On session start (new or resume): `SessionStart` hook triggers `.claude/hooks/startup.sh` or `.claude/hooks/resume.sh` (loads context automatically)
- On session end: `SessionEnd` hook triggers `.claude/hooks/cleanup.sh` (updates STATE.md + TODO.md, generates HANDOFF.md if budget low)
- On tool execution (every 10 calls): `PostToolUse` hook monitors budget via `.claude/hooks/post-tool-budget.sh` (blocks if budget CRITICAL)

**Manual Fallback (if hooks disabled):**

The 5 Essential Restart Commands:
```bash
1. git status && git log -1 --oneline      # Verify last commit
2. make check-budget                       # Check docs + budget status
3. cat docs/STATE.md | head -30            # Load baseline config
4. cat docs/TODO.md | head -20             # Load task list
5. cat docs/HANDOFF.md (if exists)         # Load recovery context if budget was low
```

Or use the convenience target:
```bash
make session-start    # Runs all 5 checks at once
```

**Workflow:**
1. On arrival: `make session-start` (loads all context)
2. Work on task from `docs/TODO.md`
3. After major changes: `make checkpoint` (updates STATE.md + TODO.md)
4. Before commit: verify `docs/STATE.md` is updated
5. On departure: `make handoff` (generates recovery file if budget low)

---

## Context-Safety System

**Goal:** Prevent context loss when token budget exhausted + ensure safe session restart.

**Configuration:**
- `.claude/settings.json` - Hook definitions + documentation gates
  - `session-start-hook`: Triggers on session init (loads context)
  - `stop-hook`: Triggers before session end (saves deltas)
  - `post-tool-use-hook`: Monitors context budget (generates HANDOFF on budget low)

**Documentation Files:**
- `docs/STATE.md` - Current config + session deltas (updated via checkpoint)
- `docs/TODO.md` - Ranked tasks with definition of done (updated via checkpoint)
- `docs/HANDOFF.md` - Auto-generated recovery file (on budget low or manual handoff)
- `scripts/checkpoint.py` - Update system (executes via Makefile)
- `Makefile` - Targets: session-start, checkpoint, handoff, check-budget

**Make Targets:**
```bash
make session-start           # Load full context (git status + budget + STATE + TODO + HANDOFF)
make checkpoint              # Interactively update STATE.md delta + TODO.md top10 (prompts for description)
make checkpoint-auto         # Non-interactive checkpoint for hooks (uses $AUTO_DESC or "auto snapshot")
make check-budget            # Verify docs exist + show status
make handoff                 # Generate HANDOFF.md for emergency restart
```

**checkpoint vs checkpoint-auto:**
| Target | Interactive | Use Case |
|--------|-------------|----------|
| `checkpoint` | Yes (prompts) | Manual use in terminal |
| `checkpoint-auto` | No | Hooks, scripts, CI/CD |

---

## Auto-Snapshot Workflow (Claude Code Hooks)

**Goal:** Automatically commit changes after every file modification, keep WIP branches safe, push to origin when session ends.

**How It Works:**

1. **SessionStart Hook** (`.claude/hooks/startup.sh`)
   - Triggered when Claude Code starts a session (new or resume)
   - Automatically creates/switches to `wip/YYYYMMDD-HHMM` branch if on `main/master` (prevents direct main commits)
   - Runs `make check-budget` to verify context health
   - Loads STATE.md + TODO.md for context

2. **PostToolUse Hook** (`.claude/hooks/post_tool_use.sh`)
   - Triggered after file-modifying tools: `Write`, `Edit`, `NotebookEdit`, `TodoWrite`
   - Runs `scripts/auto_snapshot.sh`:
     - Executes `make checkpoint-auto` (non-interactive, updates STATE.md + TODO.md)
     - Selectively stages changes (excludes build artifacts: `build/`, `managed_components/`, `*.bin`, `*.elf`)
     - Creates automatic commit: `auto: checkpoint YYYY-MM-DD HH:MM (N files) – <file summary>`
   - Silent no-op if no changes or outside a git repo

3. **SessionEnd Hook** (`.claude/hooks/cleanup.sh`)
   - Triggered when Claude Code session ends (exit, clear, logout)
   - Updates STATE.md + TODO.md (final checkpoint)
   - Generates `docs/HANDOFF.md` if context budget is LOW
   - Runs `scripts/auto_push_if_needed.sh`:
     - Pushes commits to origin if on WIP branch and ahead of remote
     - Never pushes from `main/master` (safe-guard)
     - Silent exit if no commits or network errors

**Benefits:**
- ✅ No manual `git add . && git commit` — fully automatic
- ✅ main branch stays clean — WIP branches for all work
- ✅ Token-sparse — minimal checkpoint delta, fast hooks
- ✅ Rollback-safe — every file change becomes a commit, easy recovery
- ✅ Non-destructive — never deletes, never force-pushes, never cleans

**Examples:**

```bash
# Scenario 1: You edit docs/TODO.md
1. You use Edit tool to update docs/TODO.md
2. PostToolUse hook triggers auto-snapshot
3. Commit created: "auto: checkpoint 2025-12-27 15:24 (2 files) – docs/TODO.md, docs/STATE.md"
4. You keep working, no friction

# Scenario 2: Session ends with uncommitted work
1. Claude Code session ends (exit or timeout)
2. SessionEnd hook runs cleanup.sh
3. Final checkpoint committed
4. If on WIP branch: push to origin/wip/YYYYMMDD-HHMM
5. Next session resumes exactly where it left off

# Scenario 3: You restart on main branch
1. Claude Code session starts
2. SessionStart hook detects branch == main
3. Creates new branch: wip/20251227-1530
4. Checkouts to new branch
5. No commits on main, ever
```

**Disable Auto-Snapshot (if needed):**

Option 1: Temporarily disable PostToolUse hook:
```bash
# Edit .claude/settings.json, comment out PostToolUse section:
#   "PostToolUse": [ ... ]
```

Option 2: Delete the hook script:
```bash
rm .claude/hooks/post_tool_use.sh
```

Option 3: Disable in settings:
```json
{
  "context-safety": {
    "auto-snapshot-enabled": false
  }
}
```

**Manual Equivalent:**
If hooks are disabled, manually replicate auto-snapshot:
```bash
make auto-snapshot  # Same as: checkpoint + git add + git commit
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
