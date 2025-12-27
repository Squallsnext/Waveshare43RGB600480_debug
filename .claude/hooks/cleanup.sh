#!/bin/bash
# Hook: SessionEnd
# Triggered when Claude Code session ends (exit, clear, logout)
# Ensures STATE.md + TODO.md are updated, generates HANDOFF.md if budget low

set -e

REPO_DIR="${CLAUDE_PROJECT_DIR:-.}"
cd "$REPO_DIR"

echo ""
echo "=================================================="
echo "[SESSION-END] Checkpoint + cleanup..."
echo "=================================================="
echo ""

# 1. Check budget
echo "[1/3] Checking context budget..."
BUDGET_STATUS=$(make check-budget 2>&1 | tail -1)
echo "      $BUDGET_STATUS"

# 2. Update STATE.md + TODO.md (even if budget ok)
echo "[2/3] Updating STATE.md + TODO.md..."
if [ -f scripts/checkpoint.py ]; then
  python3 scripts/checkpoint.py update-state "session-end: checkpoint" 2>/dev/null || true
  python3 scripts/checkpoint.py update-todo 2>/dev/null || true
  echo "      ✓ Docs updated"
else
  echo "      [WARN] scripts/checkpoint.py not found"
fi

# 3. Generate HANDOFF.md if budget low
if echo "$BUDGET_STATUS" | grep -q "LOW"; then
  echo "[3/3] Budget LOW - generating HANDOFF.md..."
  python3 scripts/checkpoint.py generate-handoff 2>/dev/null || true
  echo "      ✓ HANDOFF.md generated"
else
  echo "[3/3] Budget OK - HANDOFF.md optional"
fi

echo ""
echo "=================================================="
echo "[SESSION-END] ✅ Checkpoint complete."
echo "=================================================="
echo ""

exit 0
