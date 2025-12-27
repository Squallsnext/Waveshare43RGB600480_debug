#!/usr/bin/env python3
"""
Context-Safety Checkpoint System
Updates STATE.md, TODO.md, generates HANDOFF.md when budget low.
"""

import sys
import os
import subprocess
import time
from datetime import datetime

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
DOCS_DIR = os.path.join(REPO_ROOT, "docs")
STATE_FILE = os.path.join(DOCS_DIR, "STATE.md")
TODO_FILE = os.path.join(DOCS_DIR, "TODO.md")
HANDOFF_FILE = os.path.join(DOCS_DIR, "HANDOFF.md")


def run_cmd(cmd):
    """Run shell command, return output."""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result.stdout.strip()
    except Exception as e:
        return f"ERROR: {e}"


def get_timestamp():
    """Return ISO 8601 timestamp."""
    return datetime.now().strftime("%Y-%m-%dT%H:%M:%SZ")


def update_state_md(delta_text):
    """Update STATE.md with timestamp + delta block."""
    if not os.path.exists(STATE_FILE):
        print(f"ERROR: {STATE_FILE} not found")
        return False

    with open(STATE_FILE, "r") as f:
        content = f.read()

    # Find "## Critical Context" and insert delta before it
    marker = "## Critical Context"
    if marker not in content:
        print(f"WARNING: {marker} not found in STATE.md")
        return False

    timestamp = get_timestamp()
    delta_block = f"\n## Session Delta [{timestamp}]\n{delta_text}\n"

    new_content = content.replace(marker, delta_block + marker)

    # Update footer timestamp
    new_content = new_content.replace(
        f"Last Updated: {get_timestamp().split('T')[0]}",
        f"Last Updated: {timestamp}"
    )

    with open(STATE_FILE, "w") as f:
        f.write(new_content)

    print(f"[STATE.md] Updated @ {timestamp}")
    return True


def update_todo_md():
    """Update TODO.md - mark top completed, refresh order."""
    if not os.path.exists(TODO_FILE):
        print(f"ERROR: {TODO_FILE} not found")
        return False

    timestamp = get_timestamp()

    with open(TODO_FILE, "r") as f:
        content = f.read()

    # Update footer timestamp
    new_content = content.replace(
        "Last Updated:",
        f"Last Updated: {timestamp}"
    )

    with open(TODO_FILE, "w") as f:
        f.write(new_content)

    print(f"[TODO.md] Timestamp refreshed @ {timestamp}")
    return True


def check_budget():
    """Check context budget (git log size, transcript lines, etc)."""
    log_output = run_cmd("git log --oneline")
    log_lines = len(log_output.split("\n"))
    file_count = len(run_cmd("find . -type f").split("\n"))

    print("[BUDGET CHECK]")
    print(f"  Git log lines: {log_lines}")
    print(f"  Files in repo: {file_count}")
    print(f"  STATE.md exists: {os.path.exists(STATE_FILE)}")
    print(f"  TODO.md exists: {os.path.exists(TODO_FILE)}")
    print(f"  HANDOFF.md exists: {os.path.exists(HANDOFF_FILE)}")

    # Heuristic: if any file is missing, flag LOW
    if not all([os.path.exists(f) for f in [STATE_FILE, TODO_FILE, HANDOFF_FILE]]):
        print("  STATUS: LOW (doc files missing) -> run 'make handoff'")
        return False

    print("  STATUS: OK")
    return True


def generate_handoff():
    """Generate HANDOFF.md with current state."""
    timestamp = get_timestamp()
    last_commit = run_cmd("git log -1 --oneline") or "NO_COMMITS"
    git_status = run_cmd("git status --short") or "clean"

    handoff_content = f"""# Session Handoff - AUTO-GENERATED {timestamp}

**CONTEXT BUDGET LOW / SESSION ENDING**

## Last Session State
- Timestamp: {timestamp}
- Last commit: {last_commit}
- Git status: {git_status}

## Build Status
- Run: `git log -1 --oneline`
- Run: `git status`
- Run: `idf.py build` (if touching hw)

## Restart Checklist
1. Read STATE.md (current baseline config)
2. Read TODO.md (top 10 tasks, DoD)
3. Check git log and status
4. Run: `make check-budget`

## Next Actions
- If build is failing: check stderr in previous session logs
- If git dirty: `git status` to see what changed
- If docs modified: update STATE.md + TODO.md before commit

---
GENERATED: {timestamp}
READ STATE.md AND TODO.md FIRST, THEN RESUME.
"""

    with open(HANDOFF_FILE, "w") as f:
        f.write(handoff_content)

    print(f"[HANDOFF.md] Generated @ {timestamp}")
    return True


def main():
    if len(sys.argv) < 2:
        print("Usage: checkpoint.py [update-state|update-todo|check-budget|generate-handoff]")
        print("  update-state TEXT   - Add delta to STATE.md")
        print("  update-todo         - Refresh TODO.md timestamp")
        print("  check-budget        - Check context budget status")
        print("  generate-handoff    - Create HANDOFF.md")
        sys.exit(1)

    cmd = sys.argv[1]

    if cmd == "update-state" and len(sys.argv) > 2:
        delta = " ".join(sys.argv[2:])
        update_state_md(delta)
    elif cmd == "update-todo":
        update_todo_md()
    elif cmd == "check-budget":
        check_budget()
    elif cmd == "generate-handoff":
        generate_handoff()
    else:
        print(f"Unknown command: {cmd}")
        sys.exit(1)


if __name__ == "__main__":
    main()
