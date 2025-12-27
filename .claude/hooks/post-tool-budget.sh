#!/bin/bash
# Hook: PostToolUse - Budget Monitoring
# Triggered after every Bash, Edit, Write, Read tool use
# Tracks tool usage count and flags budget concerns early
# Prevents context overflow by proactive checkpoint signaling

set -e

REPO_DIR="${CLAUDE_PROJECT_DIR:-.}"
cd "$REPO_DIR"

# Only log every 10th call to avoid spam
COUNTER_FILE=".claude/.tool_counter"
mkdir -p "$(dirname "$COUNTER_FILE")"

COUNTER=0
if [ -f "$COUNTER_FILE" ]; then
  COUNTER=$(cat "$COUNTER_FILE" 2>/dev/null || echo 0)
fi
COUNTER=$((COUNTER + 1))
echo "$COUNTER" > "$COUNTER_FILE"

# Every 10 tool uses, check budget
if [ $((COUNTER % 10)) -eq 0 ]; then
  BUDGET_STATUS=$(make check-budget 2>&1 | grep "STATUS:" | awk '{print $NF}')

  if [ "$BUDGET_STATUS" = "LOW" ] || [ "$BUDGET_STATUS" = "CRITICAL" ]; then
    # Budget is low - signal to Claude via context
    cat <<EOF
{
  "decision": "block",
  "reason": "Context budget is $BUDGET_STATUS. Please run 'make handoff' and commit before continuing.",
  "hookSpecificOutput": {
    "hookEventName": "PostToolUse",
    "additionalContext": "Budget status: $BUDGET_STATUS. Call 'make handoff' to generate recovery docs before next session."
  }
}
EOF
    exit 1
  fi
fi

# Budget ok, allow tool to continue
exit 0
