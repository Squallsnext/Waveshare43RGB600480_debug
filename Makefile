.PHONY: session-start checkpoint handoff check-budget auto-snapshot build clean fullclean monitor flash test-build

# Context-Safety Session Start (Hook Fallback)
session-start:
	@echo "[SESSION-START] Verifying repo context..."
	@git status && git log -1 --oneline
	@make check-budget
	@echo ""
	@echo "[STATE]"
	@head -20 docs/STATE.md
	@echo ""
	@echo "[TODO]"
	@head -20 docs/TODO.md
	@if [ -f docs/HANDOFF.md ]; then echo "[HANDOFF]"; head -20 docs/HANDOFF.md; fi
	@echo ""
	@echo "[SESSION-START] Ready to code. Context loaded."

# Context-Safety Checkpoints
checkpoint:
	@read -p "Describe change (one line): " desc; \
	python3 scripts/checkpoint.py update-state "$$desc"
	python3 scripts/checkpoint.py update-todo

handoff:
	python3 scripts/checkpoint.py generate-handoff
	@echo "[HANDOFF] Created docs/HANDOFF.md - commit or share for context recovery"

check-budget:
	python3 scripts/checkpoint.py check-budget

auto-snapshot:
	@if [ -f scripts/auto_snapshot.sh ]; then bash scripts/auto_snapshot.sh; else echo "ERROR: scripts/auto_snapshot.sh not found"; exit 1; fi

# ESP-IDF Build Targets
build:
	idf.py build

clean:
	idf.py clean

fullclean:
	idf.py fullclean

monitor:
	idf.py monitor

flash:
	idf.py flash monitor

test-build: fullclean build
	@echo "[TEST-BUILD] Success - firmware at build/waveshare_rgb_lcd.bin"

# Git workflow reminder
pre-commit:
	@echo "[PRE-COMMIT] Checking STATE.md + TODO.md..."
	@test -f docs/STATE.md || (echo "ERROR: docs/STATE.md missing" && exit 1)
	@test -f docs/TODO.md || (echo "ERROR: docs/TODO.md missing" && exit 1)
	@echo "[PRE-COMMIT] All gates passed"

help:
	@echo "Context-Safety + Build Targets:"
	@echo "  make session-start    - Verify repo, show STATUS + TODO + config (session init)"
	@echo "  make checkpoint       - Update STATE.md + TODO.md with delta"
	@echo "  make handoff          - Generate HANDOFF.md for context recovery"
	@echo "  make check-budget     - Check documentation/budget status"
	@echo ""
	@echo "ESP-IDF Targets:"
	@echo "  make build            - idf.py build"
	@echo "  make clean            - idf.py clean"
	@echo "  make fullclean        - idf.py fullclean"
	@echo "  make flash            - idf.py flash monitor"
	@echo "  make test-build       - fullclean && build (verify reproducible)"
	@echo ""
	@echo "Workflow:"
	@echo "  0. (start)  make session-start"
	@echo "  1.          make test-build"
	@echo "  2.          make checkpoint"
	@echo "  3.          git add . && git commit ..."
	@echo "  4. (before) make handoff"
