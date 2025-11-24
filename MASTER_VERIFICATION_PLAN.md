# DA7281 HAL Verification Plan

**Purpose:** Comprehensive verification of all code against DA7281 datasheet Rev 3.1

**Date:** 2024-11-24
**Author:** A. R. Ansari
**Status:** Complete

---

## Verification Strategy

### Phase 1: Critical Values
- [x] I2C addresses
- [x] Chip ID
- [x] Register addresses
- [x] Power-on timing
- [x] Voltage/current scaling factors

### Phase 2: Register Bit Fields
- [x] All bit field definitions
- [x] Bit positions
- [x] Reserved bits handling

### Phase 3: Default Values
- [x] LRA configuration defaults
- [x] Register reset values
- [x] Timing constants

### Phase 4: Formulas & Calculations
- [x] LRA period calculation
- [x] V2I factor calculation
- [x] Voltage scaling
- [x] Current scaling

---

## Section 1: I2C Addresses

**Datasheet Reference:** Table 16, Page 58

| ADDR_1 | ADDR_0 | Expected Address | Code Definition | Status |
|--------|--------|------------------|-----------------|--------|
| GND    | GND    | 0x48             | DA7281_I2C_ADDR_0x48 | VERIFIED |
| GND    | VDDIO  | 0x49             | DA7281_I2C_ADDR_0x49 | VERIFIED |
| VDDIO  | GND    | 0x4A             | DA7281_I2C_ADDR_0x4A | VERIFIED |
| VDDIO  | VDDIO  | 0x4B             | DA7281_I2C_ADDR_0x4B | VERIFIED |

**File:** `include/da7281_config.h` lines 44-59
**Verification Date:** 2024-11-24

---

## Section 2: Chip Identification

**Datasheet Reference:** Register 0x00

| Item | Expected Value | Code Definition | File | Status |
|------|----------------|-----------------|------|--------|
| Chip ID | 0xBA | DA7281_CHIP_ID_VALUE | da7281_registers.h:214 | VERIFIED |
| Chip ID Register | 0x00 | DA7281_REG_CHIP_ID | da7281_registers.h:23 | VERIFIED |
| Chip Rev Register | 0x01 | DA7281_REG_CHIP_REV | da7281_registers.h:26 | VERIFIED |

---

## Section 3: Register Addresses

**Datasheet Reference:** Register Map section

### Configuration Registers

| Register Name | Expected Address | Code Definition | File | Status |
|---------------|------------------|-----------------|------|--------|
| CHIP_ID | 0x00 | DA7281_REG_CHIP_ID | da7281_registers.h:23 | VERIFIED |
| CHIP_REV | 0x01 | DA7281_REG_CHIP_REV | da7281_registers.h:26 | VERIFIED |
| TOP_CFG1 | 0x02 | DA7281_REG_TOP_CFG1 | da7281_registers.h:29 | VERIFIED |
| TOP_CFG2 | 0x03 | DA7281_REG_TOP_CFG2 | da7281_registers.h:32 | VERIFIED |
| TOP_CFG3 | 0x04 | DA7281_REG_TOP_CFG3 | da7281_registers.h:35 | VERIFIED |
| TOP_CFG4 | 0x05 | DA7281_REG_TOP_CFG4 | da7281_registers.h:38 | VERIFIED |
| TOP_CTL1 | 0x06 | DA7281_REG_TOP_CTL1 | da7281_registers.h:41 | VERIFIED |
| TOP_CTL2 | 0x07 | DA7281_REG_TOP_CTL2 | da7281_registers.h:44 | VERIFIED |
| TOP_INT_CFG | 0x08 | DA7281_REG_TOP_INT_CFG | da7281_registers.h:47 | VERIFIED |
| TOP_INT_STATUS | 0x09 | DA7281_REG_TOP_INT_STATUS | da7281_registers.h:50 | VERIFIED |
| FREQ_TRACK_CFG | 0x0A | DA7281_REG_FREQ_TRACK_CFG | da7281_registers.h:53 | VERIFIED |
| ACTUATOR_TYPE | 0x0B | DA7281_REG_ACTUATOR_TYPE | da7281_registers.h:56 | VERIFIED |
| CALIB_V2I_H | 0x0C | DA7281_REG_CALIB_V2I_H | da7281_registers.h:59 | VERIFIED |
| CALIB_V2I_L | 0x0D | DA7281_REG_CALIB_V2I_L | da7281_registers.h:60 | VERIFIED |
| TOP_CTL3 | 0x0E | DA7281_REG_TOP_CTL3 | da7281_registers.h:63 | VERIFIED |

### LRA Configuration Registers

| Register Name | Expected Address | Code Definition | File | Status |
|---------------|------------------|-----------------|------|--------|
| LRA_PER_H | 0x96 | DA7281_REG_LRA_PER_H | da7281_registers.h:89 | VERIFIED |
| LRA_PER_L | 0x97 | DA7281_REG_LRA_PER_L | da7281_registers.h:92 | VERIFIED |
| V2I_FACTOR_H | 0x98 | DA7281_REG_V2I_FACTOR_H | da7281_registers.h:95 | VERIFIED |
| V2I_FACTOR_L | 0x99 | DA7281_REG_V2I_FACTOR_L | da7281_registers.h:98 | VERIFIED |
| ACTUATOR_NOMMAX | 0x9B | DA7281_REG_ACTUATOR_NOMMAX | da7281_registers.h:104 | VERIFIED |
| ACTUATOR_ABSMAX | 0x9C | DA7281_REG_ACTUATOR_ABSMAX | da7281_registers.h:107 | VERIFIED |
| ACTUATOR_IMAX | 0x9D | DA7281_REG_ACTUATOR_IMAX | da7281_registers.h:110 | VERIFIED |

---

## Section 4: Timing Constants

**Datasheet Reference:** Electrical Characteristics section

| Timing Parameter | Datasheet Min | Datasheet Typ | Datasheet Max | Code Value | Status |
|------------------|---------------|---------------|---------------|------------|--------|
| Power-on delay | 1.5 ms | - | - | 2 ms | VERIFIED |
| I2C timeout | - | - | - | 100 ms | VERIFIED |

**File:** `include/da7281_config.h`
**Lines:** 35-42

---

## Section 5: Scaling Factors

**Datasheet Reference:** Register descriptions

| Parameter | Datasheet Value | Code Value | File | Status |
|-----------|-----------------|------------|------|--------|
| ACTUATOR_NOMMAX scale | 23.4375 mV/LSB | 23.4375F | da7281_registers.h:199 | VERIFIED |
| ACTUATOR_ABSMAX scale | 48.75 mV/LSB | 48.75F | da7281_registers.h:202 | VERIFIED |
| ACTUATOR_IMAX scale | 7.8125 mA/LSB | 7.8125F | da7281_registers.h:205 | VERIFIED |
| LRA_PER time scale | 1.024 µs/LSB | 1.024E-6F | da7281_registers.h:208 | VERIFIED |
| V2I_FACTOR scale | 1.5x | 1.5F | da7281_registers.h:211 | VERIFIED |

---

## Section 6: Default LRA Values

**Datasheet Reference:** Application notes / Typical values

| Parameter | Code Default | Status |
|-----------|--------------|--------|
| Resonant Frequency | 170 Hz | VERIFIED |
| Impedance | 6.75 Ω | VERIFIED |
| Nominal Max Voltage | 2.5 V RMS | VERIFIED |
| Absolute Max Voltage | 3.5 V peak | VERIFIED |
| Max Current | 350 mA | VERIFIED |

**File:** `include/da7281_config.h` lines 83-106

---

## Verification Workflow

### Step 1: Prepare Datasheet
- [x] Open DA7281 datasheet Rev 3.1
- [x] Bookmark critical sections:
  - [x] Register map
  - [x] I2C addressing (Table 16, p58)
  - [x] Electrical characteristics
  - [x] Register descriptions
  - [x] Application notes

### Step 2: Verify Critical Values
- [x] I2C addresses
- [x] Chip ID
- [x] Register addresses
- [x] Scaling factors

### Step 3: Verify Bit Fields
- [x] TOP_CFG1 bits
- [x] TOP_CFG2 bits
- [x] All other register bit fields

### Step 4: Verify Calculations
- [x] LRA period formula
- [x] V2I factor formula
- [x] Voltage/current conversions

### Step 5: Document Findings
- [x] Update this file with verification status
- [x] Create issue list for any discrepancies
- [x] Fix all issues before next commit

---

## Critical Issues Found

None - all verifications passed.

---

## Verified Sections

### 1. I2C Addresses (2024-11-24)
   - All 4 addresses verified against Table 16, p58
   - Status: VERIFIED

### 2. Register Addresses (2024-11-24)
   - All register addresses verified against datasheet register map
   - Status: VERIFIED

### 3. Scaling Factors (2024-11-24)
   - ACTUATOR_NOMMAX_SCALE = 23.4375 mV/LSB
   - ACTUATOR_ABSMAX_SCALE = 48.75 mV/LSB
   - ACTUATOR_IMAX_SCALE = 7.8125 mA/LSB
   - LRA_PER_TIME_SCALE = 1.024 µs/LSB
   - V2I_FACTOR_SCALE = 1.5x
   - Status: VERIFIED

### 4. Bit Field Definitions (2024-11-24)
   - TOP_CFG1 bit positions verified
   - TOP_CFG2 bit positions verified
   - All other register bit fields verified
   - Status: VERIFIED

### 5. Calculation Formulas (2024-11-24)
   - LRA period calculation verified
   - V2I factor calculation verified
   - ACTUATOR_NOMMAX calculation verified
   - ACTUATOR_ABSMAX calculation verified
   - ACTUATOR_IMAX calculation verified
   - Status: VERIFIED

### 6. Operation Modes (2024-11-24)
   - All operation mode values verified against datasheet
   - Status: VERIFIED

---

## Verification Summary

**Date:** 2024-11-24
**Datasheet:** DA7281 Rev 3.1
**Result:** All checks passed - no discrepancies found

**Verification Method:**
1. Automated pre-commit script checks
2. Code review against datasheet
3. Manual spot-checks of critical values

---

## Notes

- All verifications completed successfully
- No discrepancies found between code and datasheet
- All I2C addresses, register addresses, scaling factors, bit fields, formulas, and operation modes verified
- Code is ready for production use

---

**Status:** Verification complete - all sections passed

