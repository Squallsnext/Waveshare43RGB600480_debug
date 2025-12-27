#!/bin/bash
# Hook: SessionStart - Resume (RESUME SESSION)
# Triggered when Claude Code resumes an existing session (--resume, --continue, /resume)
# Reloads context: git status, budget, docs state

set -e

REPO_DIR="${CLAUDE_PROJECT_DIR:-.}"
cd "$REPO_DIR"

echo ""
echo "=================================================="
echo "[SESSION-RESUME] Reloading session context..."
echo "=================================================="
echo ""

# 1. Git status
echo "[1/4] Git Status (since last session):"
git status --short
echo ""

# 2. Budget check
echo "[2/4] Context Budget:"
make check-budget 2>/dev/null || true
echo ""

# 3. TODO.md top 10
echo "[3/4] Next 10 Tasks (from TODO.md):"
head -25 docs/TODO.md 2>/dev/null || echo "  [WARN] docs/TODO.md not found"
echo ""

# 4. HANDOFF.md if exists
if [ -f docs/HANDOFF.md ]; then
  echo "[4/4] Previous Handoff (recovery info):"
  head -12 docs/HANDOFF.md
else
  echo "[4/4] No HANDOFF.md - full recovery available in STATE+TODO"
fi
echo ""

echo "=================================================="
echo "[SESSION-RESUME] ✅ Context reloaded. Resuming..."
echo "=================================================="
echo ""

# Return context to Claude
cat <<EOF
{
  "hookSpecificOutput": {
    "hookEventName": "SessionStart",
    "additionalContext": "Session resumed - context reloaded from git, docs, and checkpoint."
  }
}
EOF

exit 0
