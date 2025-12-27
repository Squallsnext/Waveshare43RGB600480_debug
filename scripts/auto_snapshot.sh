#!/bin/bash
# Script: auto_snapshot.sh
# Purpose: Checkpoint + selective git staging + commit (NO PUSH)
# Called by: PostToolUse hook after Write/Edit/NotebookEdit/TodoWrite
# Safety: Only stages whitelisted files, works from any directory

set -e

# 1. Find repo root, exit gracefully if not in repo
repo_root="$(git rev-parse --show-toplevel 2>/dev/null)" || exit 0
cd "$repo_root" || exit 0

# 2. Check if there are any changes at all
if git diff --quiet HEAD 2>/dev/null && [ -z "$(git status --porcelain)" ]; then
  echo "NO-OP"
  exit 0
fi

# 3. Run checkpoint-auto (non-interactive, for hooks)
if [ -f Makefile ]; then
  AUTO_DESC="hook: auto snapshot" make checkpoint-auto 2>/dev/null || true
else
  python3 scripts/checkpoint.py update-state "hook: auto snapshot" 2>/dev/null || true
  python3 scripts/checkpoint.py update-todo 2>/dev/null || true
fi

# 4. Stage only whitelisted files
for path in docs/ .claude/ main/ components/ CMakeLists.txt sdkconfig.defaults partitions.csv idf_component.yml Makefile README.md; do
  if [ -e "$path" ]; then
    git add "$path" 2>/dev/null || true
  fi
done

# Also add any .sh and .py scripts
for script in scripts/*.sh scripts/*.py; do
  if [ -f "$script" ]; then
    git add "$script" 2>/dev/null || true
  fi
done

# 5. Check if anything was staged
if ! git diff --cached --quiet 2>/dev/null; then
  # Get number of staged files
  NUM_FILES=$(git diff --cached --name-only | wc -l)

  # Get first up to 5 filenames
  FILES_LIST=$(git diff --cached --name-only | head -5 | tr '\n' ', ' | sed 's/,$//')

  # Build commit message with timestamp
  TIMESTAMP=$(date '+%Y-%m-%d %H:%M')
  COMMIT_MSG="auto: checkpoint $TIMESTAMP ($NUM_FILES files) – $FILES_LIST"

  # Commit
  git commit -m "$COMMIT_MSG" 2>/dev/null || true
  echo "✓ Committed: $COMMIT_MSG"
else
  echo "NO-OP"
fi

exit 0
