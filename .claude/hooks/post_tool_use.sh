#!/bin/bash
# Hook: PostToolUse - Auto-Snapshot
# Triggered after Write|Edit|NotebookEdit|TodoWrite
# Runs auto-snapshot: checkpoint + selective git staging + commit

set -e

REPO_DIR="${CLAUDE_PROJECT_DIR:-.}"
cd "$REPO_DIR" || exit 0

# Run auto-snapshot
bash "$REPO_DIR"/scripts/auto_snapshot.sh 2>/dev/null || true

# Also check budget periodically (via post-tool-budget.sh)
bash "$REPO_DIR"/.claude/hooks/post-tool-budget.sh 2>/dev/null || true

exit 0
