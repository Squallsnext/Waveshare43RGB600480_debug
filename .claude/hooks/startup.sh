#!/bin/bash
# Hook: SessionStart - Startup (NEW SESSION)
# Triggered when Claude Code starts a new session
# Sets up full context: repo path, git status, budget check, docs loaded

set -e

REPO_DIR="${CLAUDE_PROJECT_DIR:-.}"
cd "$REPO_DIR"

echo "=================================================="
echo "[SESSION-START] Initializing session context..."
echo "=================================================="
echo ""

# 1. Verify repo + git
echo "[1/5] Git Status:"
git status --short
echo ""
git log -1 --oneline
echo ""

# 2. Budget check
echo "[2/5] Context Budget:"
make check-budget || true
echo ""

# 3. STATE.md
echo "[3/5] Current State (first 25 lines):"
head -25 docs/STATE.md 2>/dev/null || echo "  [WARN] docs/STATE.md not found"
echo ""

# 4. TODO.md
echo "[4/5] Current Tasks (first 25 lines):"
head -25 docs/TODO.md 2>/dev/null || echo "  [WARN] docs/TODO.md not found"
echo ""

# 5. HANDOFF.md if exists
if [ -f docs/HANDOFF.md ]; then
  echo "[5/5] Session Handoff Info (first 15 lines):"
  head -15 docs/HANDOFF.md
else
  echo "[5/5] No HANDOFF.md (fresh session)"
fi
echo ""

echo "=================================================="
echo "[SESSION-START] ✅ Context loaded. Ready to code."
echo "=================================================="
echo ""

# Return context to Claude
cat <<EOF
{
  "hookSpecificOutput": {
    "hookEventName": "SessionStart",
    "additionalContext": "Session initialized with git context, budget verified, and docs loaded."
  }
}
EOF

exit 0
