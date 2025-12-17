# CRITICAL FINDINGS - DA7281 Register Issues

## Issue Found: Misleading Comments in Register Header

### Problem Location: include/da7281_registers.h lines 110-113

```c
/* ACTUATOR_NOMMAX (0x9B) - Voltage scaling factor */
#define DA7281_ACTUATOR_NOMMAX_SCALE    (23.4F)  /**< mV per LSB */

/* ACTUATOR_ABSMAX (0x9C) - Voltage scaling factor */
#define DA7281_ACTUATOR_ABSMAX_SCALE    (23.4F)    /**< mV per LSB */
```

### The Issue
- **Comments say**: 0x9B and 0x9C
- **Actual register defines use**: 0x0B and 0x0C (lines 63-66)
- This discrepancy is VERY CONFUSING

### What's Correct?
According to the register definitions at the top of the file:
```c
#define DA7281_REG_ACTUATOR_NOMMAX      (0x0BU)  // Line 63
#define DA7281_REG_ACTUATOR_ABSMAX      (0x0CU)  // Line 66  
```

**Question for Christopher**: Which addresses are correct per datasheet Table 20?
- Option A: 0x0B, 0x0C (as defined)
- Option B: 0x9B, 0x9C (as in comments)

## Summary of Code Status

### What's GOOD ✅
1. Implementation logic is correct (writes to proper registers)
2. Formulas match datasheet v3.1
3. Operation mode uses TOP_CTL1 (0x22)
4. Actuator type uses TOP_CFG1 bit 5 (0x13)
5. Override uses TOP_CTL2 (0x23)
6. Fault clearing implemented

### What Needs Christopher's Input ⚠️
1. **LRA register addresses** (0x07-0x0D) - need datasheet Table 20 verification
2. The comment mismatch (0x9B vs 0x0B, 0x9C vs 0x0C)

### What's Already Fixed from Christopher's List ✅
- ✅ Issue 1: Actuator type → TOP_CFG1
- ✅ Issue 2: Operation mode → TOP_CTL1  
- ✅ Issue 3: LRA formulas corrected
- ✅ Issue 4: Override → TOP_CTL2
- ✅ Issue 5: Removed non-existent registers
- ✅ Issue 7: Fault handling added

## Recommendation

**For Christopher to test:**
1. Build the current code
2. Try to read CHIP_REV (0x00) - should return 0xBA or 0xCA
3. Try to read/write TOP_CFG1 (0x13)
4. Try to write LRA_PER_H (currently 0x07) and see if device accepts it
5. Share RTTViewer output showing which registers work/fail

This will definitively identify if register addresses are correct or need adjustment.

## Files That Are Clean ✅
- `src/da7281.c` - Implementation uses correct register symbols
- `src/da7281_i2c.c` - I2C layer is solid
- `include/da7281.h` - API header is clean
- `include/da7281_config.h` - Configuration is correct
- `examples/haptics_demo.c` - Example code is good

## Only File Needing Verification
- `include/da7281_registers.h` - Register addresses need datasheet verification
  - Specifically addresses 0x07-0x0D (LRA configuration registers)
  - Comment discrepancy at lines 110-113
