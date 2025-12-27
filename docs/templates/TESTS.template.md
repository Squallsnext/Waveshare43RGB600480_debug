# Test Plan & Results

## Test Categories

### Build Tests
- **Name:** [Test Name]
  - **Command:** `[build command]`
  - **Expected Result:** [Exit code 0, artifact size < X MB, etc.]
  - **Last Run:** [TIMESTAMP]
  - **Status:** [PASS|FAIL|SKIP]
  - **Notes:** [Any deviations]

### Smoke Tests (Pre-Flash)
- **Name:** [Test Name]
  - **Purpose:** [What does this verify?]
  - **Command:** `[command or manual steps]`
  - **Success Criteria:** [Testable, measurable condition]
  - **Last Run:** [TIMESTAMP]
  - **Status:** [PASS|FAIL|SKIP]

### Runtime Tests (Post-Flash)
- **Name:** [Test Name]
  - **Setup:** [Prerequisites, hardware state]
  - **Steps:**
    1. [Step 1]
    2. [Step 2]
    3. [Step 3]
  - **Expected Result:** [Observable behavior]
  - **Last Run:** [TIMESTAMP]
  - **Status:** [PASS|FAIL|SKIP]
  - **Logs:** [Link to logs if applicable]

### Performance Tests
- **Name:** [Test Name]
  - **Metric:** [What is measured]
  - **Target:** [Expected baseline]
  - **Last Measurement:** [Value ± uncertainty]
  - **Status:** [PASS|FAIL|SKIP]
  - **Date:** [TIMESTAMP]

## Test Matrix

| Component | Build | Smoke | Runtime | Performance | Notes |
|-----------|-------|-------|---------|-------------|-------|
| app_config | [ ] | [ ] | [ ] | [ ] | |
| board_lcd | [ ] | [ ] | [ ] | [ ] | |
| board_touch | [ ] | [ ] | [ ] | [ ] | |
| board_telemetry | [ ] | [ ] | [ ] | [ ] | |
| main (integration) | [ ] | [ ] | [ ] | [ ] | |

## Known Issues
- **Issue [N]:** [Description]
  - **Workaround:** [If any]
  - **Status:** [OPEN|RESOLVED]
  - **Date Reported:** [TIMESTAMP]

## Test Automation
- **CI/CD:** [Github Actions or other]
- **Manual Tests:** [Frequency]
- **Hardware Requirements:** [Board model, connections]

---
Last Updated: [TIMESTAMP]
Coverage: [X% of components]
Next Review: [DATE]
