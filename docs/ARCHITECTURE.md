# DA7281 HAL Architecture

## Overview

The DA7281 Hardware Abstraction Layer (HAL) provides a clean, thread-safe interface for controlling Dialog DA7281 haptic driver ICs on the Qorvo DWM3001C platform (nRF52833 with FreeRTOS).

## Design Principles

### 1. Thread Safety
- All I2C operations protected by FreeRTOS mutex
- Safe for use in multi-threaded FreeRTOS applications
- No race conditions on shared I2C bus

### 2. Datasheet Compliance
- All register definitions verified against DA7281 datasheet v1.1
- Timing requirements strictly followed (e.g., 1.5ms power-up delay)
- Calculation formulas match datasheet specifications

### 3. Error Handling
- Comprehensive error codes for all failure modes
- Parameter validation at API boundaries
- Graceful degradation on errors

### 4. Multi-Device Support
- Supports up to 4 DA7281 devices
- Configurable I2C bus (TWI0 or TWI1)
- Configurable I2C address (0x4A or 0x4B)

### 5. Logging
- Extensive logging for debugging
- Configurable log levels
- Can be disabled for production builds

## Module Structure

```
da7281-dwm3001c-hal/
├── include/
│   ├── da7281.h              # Main API (public)
│   ├── da7281_registers.h    # Register definitions (public)
│   └── da7281_config.h       # Configuration options (public)
├── src/
│   ├── da7281.c              # Core HAL implementation
│   └── da7281_i2c.c          # I2C communication layer
└── examples/
    └── haptics_demo.c        # Usage example
```

## Layer Architecture

```
┌─────────────────────────────────────────┐
│     Application Layer                   │
│  (haptics_demo.c, user code)            │
└─────────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────────┐
│     DA7281 HAL API Layer                │
│  (da7281.h - Public API)                │
│  - da7281_init()                        │
│  - da7281_configure_lra()               │
│  - da7281_set_operation_mode()          │
│  - da7281_set_override_amplitude()      │
└─────────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────────┐
│     Core Implementation Layer           │
│  (da7281.c)                             │
│  - Power control                        │
│  - LRA configuration                    │
│  - Mode control                         │
│  - Self-test                            │
└─────────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────────┐
│     I2C Communication Layer             │
│  (da7281_i2c.c)                         │
│  - Thread-safe I2C read/write           │
│  - FreeRTOS mutex protection            │
│  - Register modify operations           │
└─────────────────────────────────────────┘
                  ↓
┌─────────────────────────────────────────┐
│     Platform Layer                      │
│  (Qorvo SDK - nrf_drv_twi)              │
│  - TWI0/TWI1 hardware drivers           │
│  - GPIO control                         │
│  - FreeRTOS primitives                  │
└─────────────────────────────────────────┘
```

## Initialization Flow

```
1. Application creates device handle
   ↓
2. da7281_power_on()
   - Configure GPIO as output
   - Set GPIO high
   - Wait 1.5ms (datasheet requirement)
   ↓
3. da7281_init()
   - Initialize I2C mutex (if needed)
   - Initialize TWI instance (if needed)
   - Read and verify chip ID (0xBA)
   - Read chip revision
   - Set motor type to LRA
   - Set operation mode to INACTIVE
   ↓
4. da7281_configure_lra()
   - Calculate LRA period from frequency
   - Calculate V2I factor from impedance
   - Write voltage limits
   - Write current limit
   ↓
5. da7281_set_operation_mode(DRO)
   - Set to Direct Register Override mode
   ↓
6. da7281_set_amplifier_enable(true)
   - Enable output amplifier
   ↓
7. Ready for haptic output!
```

## LRA Configuration Calculations

### 1. LRA Period (LRA_PER)
```
Formula: LRA_PER = 1 / (f_res * 1.024e-6)

Example (170 Hz):
  LRA_PER = 1 / (170 * 1.024e-6)
          = 1 / 0.00017408
          = 5747 (0x1673)

Register write:
  LRA_PER_H = 0x16
  LRA_PER_L = 0x73
```

### 2. V2I Factor
```
Formula: V2I_FACTOR = impedance_ohm * 1.5

Example (6.75Ω):
  V2I_FACTOR = 6.75 * 1.5
             = 10.125
             = 10 (rounded)

Register write:
  V2I_FACTOR_H = 0x00
  V2I_FACTOR_L = 0x0A
```

### 3. Nominal Max Voltage
```
Formula: NOMMAX = (V_rms * 1000) / 23.4375

Example (2.5V RMS):
  NOMMAX = (2.5 * 1000) / 23.4375
         = 2500 / 23.4375
         = 106.67
         = 107 (0x6B)
```

### 4. Absolute Max Voltage
```
Formula: ABSMAX = (V_peak * 1000) / 48.75

Example (3.5V peak):
  ABSMAX = (3.5 * 1000) / 48.75
         = 3500 / 48.75
         = 71.79
         = 72 (0x48)
```

### 5. Max Current
```
Formula: IMAX = current_ma / 7.8125

Example (350mA):
  IMAX = 350 / 7.8125
       = 44.8
       = 45 (0x2D)
```

## Thread Safety Implementation

### FreeRTOS Mutex Protection

```c
// Global mutex (created once)
static SemaphoreHandle_t s_i2c_mutex = NULL;

// In I2C write function:
xSemaphoreTake(s_i2c_mutex, DA7281_MUTEX_TIMEOUT_TICKS);
// ... perform I2C transaction ...
xSemaphoreGive(s_i2c_mutex);
```

### Why Mutex is Needed

1. **Multiple devices on same I2C bus**
   - Device 1 at 0x4A
   - Device 2 at 0x4B
   - Both use TWI0

2. **Multiple FreeRTOS tasks**
   - Task A controls Device 1
   - Task B controls Device 2
   - Without mutex: I2C transactions can interleave → corruption

3. **Read-Modify-Write operations**
   - Read register
   - Modify bits
   - Write back
   - Without mutex: another task could modify between read and write

## Error Handling Strategy

### Error Codes
```c
typedef enum {
    DA7281_OK = 0,                    // Success
    DA7281_ERROR_NULL_POINTER,        // NULL pointer passed
    DA7281_ERROR_INVALID_PARAM,       // Invalid parameter
    DA7281_ERROR_I2C_WRITE,           // I2C write failed
    DA7281_ERROR_I2C_READ,            // I2C read failed
    DA7281_ERROR_TIMEOUT,             // Operation timeout
    DA7281_ERROR_NOT_INITIALIZED,     // Device not initialized
    DA7281_ERROR_ALREADY_INITIALIZED, // Device already initialized
    DA7281_ERROR_CHIP_REV_MISMATCH,   // Chip revision verification failed
    DA7281_ERROR_MUTEX_FAILED,        // Mutex operation failed
    DA7281_ERROR_UNKNOWN              // Unknown error
} da7281_error_t;
```

### Error Handling Pattern
```c
da7281_error_t err;

err = da7281_init(&device);
if (err != DA7281_OK) {
    // Log error
    NRF_LOG_ERROR("Init failed: %d", err);
    // Clean up
    da7281_power_off(&device);
    // Return error to caller
    return err;
}
```

## Memory Usage

### Static Memory
- I2C mutex: ~32 bytes (FreeRTOS semaphore)
- TWI instances: ~200 bytes (2 instances)
- **Total: ~250 bytes**

### Per-Device Memory
- Device handle: 16 bytes
- **Total per device: 16 bytes**

### Stack Usage
- Typical function call: ~100 bytes
- I2C transaction: ~200 bytes
- **Recommended task stack: 2KB minimum**

## Performance Characteristics

### I2C Transaction Times (400kHz)
- Single register write: ~50μs
- Single register read: ~70μs
- LRA configuration (7 writes): ~350μs

### Initialization Time
- Power-on delay: 2ms (configurable)
- Chip ID read: ~70μs
- Full initialization: ~3ms total

### Haptic Response Time
- Override amplitude change: ~50μs (I2C write)
- Actuator response: ~6ms (170Hz period)
- **Total latency: ~6ms**

## Testing Strategy

### Unit Tests
- Test each function in isolation
- Mock hardware dependencies (I2C, GPIO)
- Verify error handling
- Test boundary conditions

### Integration Tests
- Test with real hardware
- Verify I2C communication
- Test multi-device scenarios
- Verify thread safety

### Compliance Tests
- MISRA-C compliance
- Static analysis (cppcheck)
- Code coverage (target: >80%)

## Future Enhancements

1. **Waveform Memory Support**
   - SNP memory programming
   - Embedded waveform mode (ETWM)

2. **Auto-Resonance Tracking**
   - Frequency tracking enable
   - Dynamic resonance adjustment

3. **Interrupt Support**
   - IRQ pin configuration
   - Event-driven operation

4. **Power Management**
   - Standby mode optimization
   - Dynamic power scaling

## Document Information

**Date:** November 2024
