#!/bin/bash
# Hook: SessionEnd
# Triggered when Claude Code session ends (exit, clear, logout)
# Ensures STATE.md + TODO.md are updated, generates HANDOFF.md if budget low

set -e

# Find repo root robustly; exit silently if not in a git repo
repo_root="$(git rev-parse --show-toplevel 2>/dev/null)" || exit 0
cd "$repo_root" || exit 0

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
  # Commit HANDOFF.md
  git add docs/HANDOFF.md 2>/dev/null || true
  git commit -m "docs: HANDOFF.md – context recovery (budget low)" 2>/dev/null || true
  echo "      ✓ HANDOFF.md generated and committed"
else
  echo "[3/3] Budget OK - HANDOFF.md optional"
fi

# 4. Auto-push if on WIP branch and ahead of origin
echo "[4/4] Auto-pushing commits..."
if [ -f scripts/auto_push_if_needed.sh ]; then
  bash scripts/auto_push_if_needed.sh 2>/dev/null || true
  echo "      ✓ Push check complete"
fi

echo ""
echo "=================================================="
echo "[SESSION-END] ✅ Checkpoint complete."
echo "=================================================="
echo ""

exit 0
