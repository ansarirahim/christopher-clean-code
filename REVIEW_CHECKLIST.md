# Code Review Checklist for DA7281 HAL

**Purpose:** Ensure all code changes are verified against the DA7281 datasheet and meet quality standards.

**When to use:** Before EVERY commit to this repository.

---

## 📋 Pre-Commit Checklist

### 1. Datasheet Verification ⚠️ **CRITICAL**

**Open DA7281 datasheet Rev 3.1 to relevant section BEFORE reviewing code.**

- [ ] **I2C Addresses** (Table 16, p58)
  - [ ] All 4 addresses present: `0x48`, `0x49`, `0x4A`, `0x4B`
  - [ ] ADDR_0 and ADDR_1 pin combinations documented correctly:
    - `0x48`: ADDR_1=GND, ADDR_0=GND
    - `0x49`: ADDR_1=GND, ADDR_0=VDDIO
    - `0x4A`: ADDR_1=VDDIO, ADDR_0=GND
    - `0x4B`: ADDR_1=VDDIO, ADDR_0=VDDIO
  - [ ] Addresses are 7-bit (NOT left-shifted for R/W bit)

- [ ] **Register Addresses** (Register Map section)
  - [ ] All register addresses match datasheet exactly
  - [ ] Register bit fields match datasheet definitions
  - [ ] Reserved bits are not modified

- [ ] **Timing Values** (Electrical Characteristics section)
  - [ ] Power-on delay ≥ 1.5ms (datasheet minimum)
  - [ ] I2C clock speed ≤ 400kHz (Fast Mode)
  - [ ] Reset pulse width matches datasheet

- [ ] **Default Register Values**
  - [ ] Chip ID = `0xBA` (register 0x00)
  - [ ] Default configuration values match datasheet Table 17

- [ ] **LRA Configuration** (Section 7.2)
  - [ ] Resonant frequency calculation formula matches datasheet
  - [ ] Voltage scaling formulas match datasheet
  - [ ] Overdrive limits match datasheet specifications

---

### 2. Build & Compilation

- [ ] Code compiles without errors in Segger Embedded Studio
- [ ] No new compiler warnings introduced
- [ ] All header files have include guards
- [ ] No missing `#include` statements

---

### 3. Functional Correctness

- [ ] **I2C Communication**
  - [ ] Uses Nordic `nrf_drv_twi` API correctly
  - [ ] I2C addresses are 7-bit (0x48..0x4B)
  - [ ] No manual left-shift of addresses (driver handles R/W bit)
  - [ ] Proper error handling for I2C failures

- [ ] **Hardware Control**
  - [ ] GPIO enable pin usage is correct
  - [ ] Power sequencing follows datasheet recommendations
  - [ ] Reset sequence (if applicable) matches datasheet

- [ ] **Thread Safety**
  - [ ] FreeRTOS mutex used for I2C bus access
  - [ ] No race conditions in multi-device scenarios
  - [ ] No global state without mutex protection

---

### 4. Code Quality

- [ ] **No Magic Numbers**
  - [ ] All numeric constants have named `#define` macros
  - [ ] Magic numbers replaced with descriptive names
  - [ ] Constants reference datasheet section in comments

- [ ] **Comments**
  - [ ] Comments explain WHY, not just WHAT
  - [ ] Complex logic has explanatory comments
  - [ ] Datasheet references included where applicable
  - [ ] No misleading or outdated comments

- [ ] **Code Style**
  - [ ] Follows existing naming conventions
  - [ ] Consistent indentation (spaces, not tabs)
  - [ ] Function names are descriptive
  - [ ] Variable names are clear and meaningful

- [ ] **Error Handling**
  - [ ] All error codes defined in `da7281_error_t`
  - [ ] Functions return appropriate error codes
  - [ ] Errors are logged with `DA7281_LOG_ERROR`

---

### 5. Documentation

- [ ] **README.md**
  - [ ] Updated if public API changed
  - [ ] Examples reflect current code
  - [ ] I2C address table is accurate
  - [ ] No outdated information

- [ ] **Code Examples**
  - [ ] `haptics_demo.c` still compiles and runs
  - [ ] Comments in examples are accurate
  - [ ] Example I2C addresses are valid (0x48..0x4B)

- [ ] **Header Files**
  - [ ] Doxygen comments for public functions
  - [ ] Parameter descriptions are accurate
  - [ ] Return value descriptions are complete

---

### 6. Testing (if applicable)

- [ ] **Manual Testing**
  - [ ] Code tested on actual hardware (if available)
  - [ ] Multi-device scenarios tested (if applicable)
  - [ ] Power-on sequence verified

- [ ] **Unit Tests** (future)
  - [ ] New unit tests added for new functionality
  - [ ] Existing tests still pass
  - [ ] Edge cases covered

---

### 7. Static Analysis

- [ ] **Run cppcheck** (if installed)
  ```powershell
  cppcheck --enable=all --inconclusive --suppress=missingIncludeSystem src/ include/
  ```
  - [ ] No new warnings introduced
  - [ ] Critical warnings addressed

---

### 8. Commit Message Quality

- [ ] **Commit message includes:**
  - [ ] **WHAT** changed (summary of modifications)
  - [ ] **WHY** it changed (reason/motivation)
  - [ ] **Datasheet references** (if applicable)
  - [ ] **Issue/ticket reference** (if applicable)

**Example good commit message:**
```
Fix: Add all 4 I2C addresses (0x48-0x4B) per datasheet Table 16

- Added DA7281_I2C_ADDR_0x48 and DA7281_I2C_ADDR_0x49 to da7281_config.h
- Updated documentation to show all 4 addresses with ADDR_0/ADDR_1 pin combinations
- Corrected multi-device example to use single I2C bus with all 4 addresses

Addresses feedback from hardware engineer - DA7281 supports 4 I2C addresses,
not 2 as previously documented. This allows 4 devices on a single I2C bus.

Datasheet: DA7281 Rev 3.1, Table 16 (p58)
```

---

## 🚨 Critical Failure Modes to Prevent

1. **Wrong I2C addresses** → Device won't respond
2. **Left-shifted addresses** → Communication failure
3. **Wrong power-on delay** → Unreliable initialization
4. **Wrong register values** → Incorrect haptic behavior
5. **Missing mutex protection** → Race conditions in multi-threaded code
6. **Magic numbers** → Unmaintainable code

---

## 📚 Reference Documents

- **DA7281 Datasheet Rev 3.1** (primary source of truth)
- **Nordic nRF52840 Product Specification**
- **Qorvo DWM3001C User Manual**
- **This repository's README.md**

---

## ✅ Final Check

Before committing, ask yourself:

1. **Did I verify EVERY number against the datasheet?**
2. **Would this code work on real hardware?**
3. **Can another developer understand this code in 6 months?**
4. **Is the commit message clear and complete?**

If you answer "NO" to any question, **DO NOT COMMIT** until fixed.

---

**Last updated:** 2024-11-24

