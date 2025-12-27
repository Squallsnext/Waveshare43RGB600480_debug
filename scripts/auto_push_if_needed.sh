#!/bin/bash
# Script: auto_push_if_needed.sh
# Purpose: Push WIP branch if ahead of origin (SessionEnd only)
# Safety: Only pushes wip/* branches, no force push, silent failures

set -e

# 1. Find repo root, exit gracefully if not in repo
repo_root="$(git rev-parse --show-toplevel 2>/dev/null)" || exit 0
cd "$repo_root" || exit 0

# 2. Get current branch
current_branch=$(git rev-parse --abbrev-ref HEAD 2>/dev/null || echo "")

# 3. Only push if on wip/* branch
if [[ ! "$current_branch" =~ ^wip/ ]]; then
  exit 0
fi

# 4. Only push if remote origin exists
if ! git remote get-url origin &>/dev/null; then
  exit 0
fi

# 5. Check if ahead of origin
if git rev-parse "origin/$current_branch" &>/dev/null; then
  if git merge-base --is-ancestor origin/"$current_branch" HEAD 2>/dev/null; then
    # We are ahead or at same point
    if [ "$(git rev-parse HEAD)" != "$(git rev-parse origin/$current_branch)" ]; then
      # We are ahead, push
      git push origin "$current_branch" 2>/dev/null || true
      echo "✓ Pushed $current_branch to origin"
    fi
  fi
else
  # Remote branch doesn't exist yet, create it
  git push -u origin "$current_branch" 2>/dev/null || true
  echo "✓ Created and pushed $current_branch to origin"
fi

exit 0
