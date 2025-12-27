#!/bin/bash
# Hook: SessionStart - Startup (NEW SESSION)
# Triggered when Claude Code starts a new session
# Sets up full context: repo path, git status, budget check, docs loaded

set -e

# Find repo root robustly; exit silently if not in a git repo
repo_root="$(git rev-parse --show-toplevel 2>/dev/null)" || exit 0
cd "$repo_root" || exit 0

echo "=================================================="
echo "[SESSION-START] Initializing session context..."
echo "=================================================="
echo ""

# 0. Ensure we're on a WIP branch (not main)
current_branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "main")
if [ "$current_branch" = "main" ] || [ "$current_branch" = "master" ]; then
  wip_branch="wip/$(date +%Y%m%d-%H%M)"
  echo "[WIP-BRANCH] Creating: $wip_branch"
  git checkout -b "$wip_branch" 2>/dev/null || echo "[WIP-BRANCH] Branch already exists, checking out..."
  git checkout "$wip_branch" 2>/dev/null || true
  echo ""
fi

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
