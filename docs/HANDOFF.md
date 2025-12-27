# Session Handoff - AUTO-GENERATED 2025-12-27T14:12:38Z

**CONTEXT BUDGET LOW / SESSION ENDING**

## Last Session State
- Timestamp: 2025-12-27T14:12:38Z
- Last commit: 17b115c context-safety: checkpoint/handoff system + doc gates + verification
- Git status: M .claude/settings.json
 M Makefile
 M README.md
?? .clangd

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
GENERATED: 2025-12-27T14:12:38Z
READ STATE.md AND TODO.md FIRST, THEN RESUME.
