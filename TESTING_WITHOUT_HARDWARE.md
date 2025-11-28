# Testing DA7281 HAL Without Hardware

This guide explains how to test embedded HAL code without physical hardware.

## Why Test Without Hardware?

1. **Faster Development** - No need to flash firmware for every change
2. **Easier Debugging** - Use PC debuggers (GDB, Visual Studio)
3. **Automated Testing** - Run tests in CI/CD pipelines
4. **Catch Logic Errors Early** - Before hardware integration
5. **No Hardware Required** - Develop anywhere

## Testing Strategies

### Strategy 1: Mock I2C Layer (Recommended)

**Concept:** Replace hardware I2C with software simulation

**How it works:**
```
Your HAL Code
     ↓
Mock I2C Layer (software simulation)
     ↓
Virtual Register Storage (array in memory)
```

**Example:**
```c
// Instead of real I2C:
nrf_drv_twi_tx(&twi, address, data, len);

// Use mock I2C:
mock_i2c_write(address, reg, data);
```

**Benefits:**
- ✅ Tests your logic without hardware
- ✅ Runs on PC (Windows/Linux/Mac)
- ✅ Fast (milliseconds vs seconds)
- ✅ Repeatable (no hardware flakiness)

### Strategy 2: Compilation Testing

**Concept:** Verify code compiles for target platform

**What we did:**
- Compiled with ARM GCC on Raspberry Pi 5
- Used `-Wall -Werror` (all warnings = errors)
- Verified zero errors, zero warnings

**Benefits:**
- ✅ Catches syntax errors
- ✅ Catches type mismatches
- ✅ Catches missing includes
- ✅ Verifies code structure

### Strategy 3: Static Analysis

**Concept:** Analyze code without running it

**Tools:**
- `cppcheck` - Find bugs
- `clang-tidy` - Code quality
- `splint` - Security issues

**Example:**
```bash
cppcheck --enable=all src/da7281.c
```

### Strategy 4: Integration Testing (Requires Hardware)

**Concept:** Test with real DA7281 chip

**When to do this:**
- After mock testing passes
- Before final delivery
- For hardware-specific issues

## Running Unit Tests (No Hardware)

### On Windows PC:

```bash
# Install GCC (MinGW or WSL)
# Navigate to tests directory
cd tests

# Build tests
make

# Run tests
make run
```

### On Linux/Mac:

```bash
cd tests
make run
```

### Expected Output:

```
╔════════════════════════════════════════════╗
║  DA7281 HAL Unit Tests (No Hardware)      ║
╚════════════════════════════════════════════╝

=== Test 1: Initialization Sequence ===
  [MOCK I2C] Write: addr=0x48, reg=0x23, data=0x00
  [MOCK I2C] Write: addr=0x48, reg=0x24, data=0x00
  [MOCK I2C] Write: addr=0x48, reg=0x22, data=0x01
✅ PASS: Initialization sequence correct

=== Test 2: Register Read/Write ===
  [MOCK I2C] Write: addr=0x48, reg=0x10, data=0xAB
  [MOCK I2C] Read:  addr=0x48, reg=0x10, data=0xAB
✅ PASS: Register read/write works

=== Test 3: I2C Address Handling ===
  [MOCK I2C] Write: addr=0x48, reg=0x00, data=0xFF
  [MOCK I2C] Write: addr=0x49, reg=0x00, data=0xFF
  [MOCK I2C] Write: addr=0x4A, reg=0x00, data=0xFF
  [MOCK I2C] Write: addr=0x4B, reg=0x00, data=0xFF
✅ PASS: All 4 I2C addresses handled correctly

=== Test 4: Amplitude Control ===
  [MOCK I2C] Write: addr=0x48, reg=0x4C, data=0x80
  [MOCK I2C] Read:  addr=0x48, reg=0x4C, data=0x80
✅ PASS: Amplitude control works

╔════════════════════════════════════════════╗
║  ✅ ALL TESTS PASSED (10 transactions)    ║
╚════════════════════════════════════════════╝
```

## What These Tests Verify

### ✅ Logic Correctness
- Initialization sequence is correct
- Register addresses are correct
- Data values are correct
- State machine works

### ✅ API Correctness
- Function signatures are correct
- Parameters are validated
- Return values are correct
- Error handling works

### ❌ What Tests DON'T Verify (Need Hardware)
- Actual I2C timing
- Hardware power sequencing
- LRA vibration output
- Electrical characteristics

## Best Practice Workflow

### 1. Write Code
```c
// Write your HAL function
da7281_error_t da7281_set_amplitude(da7281_device_t *dev, uint8_t amplitude)
{
    // Implementation
}
```

### 2. Write Mock Test
```c
void test_set_amplitude(void)
{
    mock_reset();
    da7281_set_amplitude(&device, 128);
    assert(mock_registers[0x4C] == 128);
}
```

### 3. Run Test on PC
```bash
make run
```

### 4. Fix Until Tests Pass
```
❌ FAIL → Fix code → Run test
❌ FAIL → Fix code → Run test
✅ PASS → Move to next feature
```

### 5. Compile for Target
```bash
cd build
make
```

### 6. Test on Hardware (Final Step)
- Flash to DWM3001C
- Test with real DA7281
- Verify vibration output

## Conclusion

**You CAN test embedded code without hardware!**

**What you need:**
1. Mock I2C layer (software simulation)
2. Unit tests (verify logic)
3. Compilation tests (verify syntax)
4. Static analysis (verify quality)

**When you need hardware:**
- Final integration testing
- Performance verification
- Electrical validation
- Customer acceptance

**For Christopher's project:**
- ✅ We verified compilation (Raspberry Pi 5)
- ✅ We can add mock tests (this document)
- ⏳ Christopher will do hardware testing (his DWM3001C + DA7281)

