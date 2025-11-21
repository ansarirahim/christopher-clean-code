# DA7281 HAL Coding Standards

## Overview

This document defines the coding standards for the DA7281 HAL project. These standards are based on MISRA-C:2012 guidelines and embedded systems best practices.

## General Principles

### 1. Safety and Reliability
- No undefined behavior
- Explicit error handling
- Defensive programming
- Parameter validation

### 2. Readability
- Self-documenting code
- Consistent naming
- Clear structure
- Comprehensive comments

### 3. Maintainability
- Modular design
- Low coupling
- High cohesion
- DRY principle (Don't Repeat Yourself)

## Naming Conventions

### Functions
```c
// Format: module_action_object()
da7281_init()
da7281_set_operation_mode()
da7281_configure_lra()
da7281_read_chip_id()
```

### Variables
```c
// snake_case for local variables
uint8_t chip_id;
float impedance_ohm;
bool selftest_passed;

// Prefix 's_' for static/file-scope variables
static SemaphoreHandle_t s_i2c_mutex;
static bool s_twi_initialized[2];
```

### Constants and Macros
```c
// UPPER_SNAKE_CASE
#define DA7281_CHIP_ID_VALUE            (0xBAU)
#define DA7281_POWER_ON_DELAY_MS        (2U)
#define DA7281_I2C_TIMEOUT_MS           (100U)
```

### Types
```c
// module_name_t suffix
typedef struct da7281_device_t;
typedef enum da7281_error_t;
typedef struct da7281_lra_config_t;
```

### Register Definitions
```c
// Format: DA7281_REG_NAME
#define DA7281_REG_CHIP_ID              (0x00U)
#define DA7281_REG_TOP_CFG1             (0x02U)

// Format: DA7281_REGNAME_BITFIELD
#define DA7281_TOP_CFG1_OP_MODE_MASK    (0x07U)
#define DA7281_TOP_CFG1_AMP_EN          (0x08U)
```

## Code Formatting

### Indentation
- 4 spaces (no tabs)
- Consistent indentation level

### Braces
```c
// K&R style - opening brace on same line
if (condition) {
    // code
} else {
    // code
}

// Functions - opening brace on new line
da7281_error_t da7281_init(da7281_device_t *device)
{
    // code
}
```

### Line Length
- Maximum 100 characters per line
- Break long lines at logical points

### Whitespace
```c
// Space after keywords
if (condition)
while (condition)
for (i = 0; i < max; i++)

// Space around operators
x = y + z;
result = (a * b) / c;

// No space before semicolon
statement;

// Space after comma
function(arg1, arg2, arg3);
```

## MISRA-C Compliance

### Required Rules

#### Rule 1.1: No undefined behavior
```c
// ✅ Good
uint8_t value = 0;
if (value < 255) {
    value++;
}

// ❌ Bad
uint8_t value = 255;
value++;  // Overflow - undefined
```

#### Rule 2.1: No unreachable code
```c
// ✅ Good
if (error) {
    return DA7281_ERROR;
}
// More code...

// ❌ Bad
return DA7281_OK;
// More code...  // Unreachable!
```

#### Rule 8.2: Function prototypes
```c
// ✅ Good
da7281_error_t da7281_init(da7281_device_t *device);

// ❌ Bad
da7281_error_t da7281_init();  // No parameter types
```

#### Rule 8.4: Compatible declarations
```c
// ✅ Good - declaration and definition match
// In header:
da7281_error_t da7281_init(da7281_device_t *device);
// In source:
da7281_error_t da7281_init(da7281_device_t *device) { ... }

// ❌ Bad - mismatch
// In header:
da7281_error_t da7281_init(da7281_device_t *device);
// In source:
da7281_error_t da7281_init(void *device) { ... }  // Different type!
```

#### Rule 9.1: No uninitialized variables
```c
// ✅ Good
uint8_t value = 0;
da7281_read_register(device, reg, &value);

// ❌ Bad
uint8_t value;  // Uninitialized
if (value > 10) { ... }  // Using uninitialized value
```

#### Rule 10.1: No implicit conversions
```c
// ✅ Good
uint8_t u8 = 10;
uint16_t u16 = (uint16_t)u8;  // Explicit cast

// ❌ Bad
uint8_t u8 = 10;
uint16_t u16 = u8;  // Implicit conversion
```

#### Rule 11.4: No pointer type conversions
```c
// ✅ Good
uint8_t *ptr = (uint8_t *)buffer;  // Explicit cast with comment
// Cast justified: buffer is byte array

// ❌ Bad
uint8_t *ptr = buffer;  // Implicit conversion
```

#### Rule 14.4: No goto statements
```c
// ✅ Good
if (error) {
    cleanup();
    return DA7281_ERROR;
}

// ❌ Bad
if (error) {
    goto cleanup;  // Avoid goto
}
```

#### Rule 17.7: Check return values
```c
// ✅ Good
da7281_error_t err = da7281_init(&device);
if (err != DA7281_OK) {
    // Handle error
}

// ❌ Bad
da7281_init(&device);  // Ignoring return value
```

#### Rule 21.3: No dynamic memory allocation
```c
// ✅ Good
static uint8_t buffer[256];  // Static allocation

// ❌ Bad
uint8_t *buffer = malloc(256);  // Dynamic allocation - avoid in embedded
```

## Documentation Standards

### File Headers
```c
/**
 * @file da7281.c
 * @brief DA7281 HAL Core Implementation
 * @author A. R. Ansari
 * @date 2024-11-21
 * 
 * Detailed description of file purpose and contents.
 */
```

### Function Documentation
```c
/**
 * @brief Initialize DA7281 device
 * 
 * Initializes the DA7281 device, verifies chip ID, and configures
 * for basic operation.
 * 
 * @param[in,out] device Pointer to device handle
 * @return DA7281_OK on success, error code otherwise
 * 
 * @note This function must be called before any other DA7281 functions
 * @note Device must be powered on before calling this function
 * 
 * @warning This function is not thread-safe during initialization
 */
da7281_error_t da7281_init(da7281_device_t *device);
```

### Inline Comments
```c
/* Calculate LRA period: LRA_PER = 1 / (f_res * 1.024e-6) */
float period_seconds = 1.0F / (float)config->resonant_freq_hz;
uint16_t lra_per = (uint16_t)(period_seconds / DA7281_LRA_PER_TIME_SCALE);
```

## Error Handling

### Always Check Pointers
```c
da7281_error_t da7281_init(da7281_device_t *device)
{
    DA7281_CHECK_NULL(device);  // Macro expands to NULL check
    
    // Rest of function...
}
```

### Always Check Return Values
```c
da7281_error_t err;

err = da7281_write_register(device, reg, value);
if (err != DA7281_OK) {
    DA7281_LOG_ERROR("Failed to write register: %d", err);
    return err;
}
```

### Provide Meaningful Error Codes
```c
// ✅ Good
if (chip_id != DA7281_CHIP_ID_VALUE) {
    return DA7281_ERROR_CHIP_ID_MISMATCH;  // Specific error
}

// ❌ Bad
if (chip_id != DA7281_CHIP_ID_VALUE) {
    return DA7281_ERROR_UNKNOWN;  // Too generic
}
```

## Magic Numbers

### Always Use Named Constants
```c
// ✅ Good
#define DA7281_CHIP_ID_VALUE (0xBAU)
if (chip_id == DA7281_CHIP_ID_VALUE) { ... }

// ❌ Bad
if (chip_id == 0xBA) { ... }  // Magic number
```

### Use Enums for Related Constants
```c
// ✅ Good
typedef enum {
    DA7281_MODE_INACTIVE = 0,
    DA7281_MODE_DRO = 1,
    DA7281_MODE_PWM = 2,
    // ...
} da7281_operation_mode_t;

// ❌ Bad
#define MODE_0 0
#define MODE_1 1
#define MODE_2 2
```

## Type Safety

### Use Specific Integer Types
```c
// ✅ Good
uint8_t register_value;
uint16_t lra_period;
int32_t temperature;

// ❌ Bad
int value;  // Ambiguous size
unsigned x;  // Ambiguous size
```

### Use Enums for State
```c
// ✅ Good
da7281_operation_mode_t mode = DA7281_MODE_DRO;

// ❌ Bad
uint8_t mode = 1;  // What does 1 mean?
```

### Use Bool for Boolean
```c
// ✅ Good
bool initialized = false;
if (initialized) { ... }

// ❌ Bad
int initialized = 0;
if (initialized == 1) { ... }
```

## Static Analysis

### Required Tools
- **cppcheck**: General static analysis
- **MISRA checker**: MISRA-C compliance
- **Compiler warnings**: -Wall -Wextra -Werror

### Running Static Analysis
```bash
# Cppcheck
cppcheck --enable=all --inconclusive --std=c11 \
  -I include/ src/

# MISRA check
cppcheck --addon=misra --std=c11 \
  -I include/ src/
```

## Code Review Checklist

- [ ] All functions documented with Doxygen comments
- [ ] All parameters validated (NULL checks, range checks)
- [ ] All return values checked
- [ ] No magic numbers
- [ ] Consistent naming conventions
- [ ] MISRA-C compliant
- [ ] No compiler warnings
- [ ] Static analysis clean
- [ ] Unit tests written
- [ ] Code coverage >80%

## Document Information

**Date:** November 2024

