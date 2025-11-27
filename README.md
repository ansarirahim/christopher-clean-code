# DA7281 Haptic Driver HAL for DWM3001C

Hardware Abstraction Layer for Dialog DA7281 haptic driver on Qorvo DWM3001C (nRF52833) with FreeRTOS.

## Project Information

- **Platform:** Qorvo DWM3001C (nRF52833 SoC)
- **SDK:** Qorvo DWM3001CDK SDK v1.1.1 (based on Nordic SDK 17.1.0)
- **RTOS:** FreeRTOS
- **Device:** Dialog DA7281 Haptic Driver IC
- **Date:** November 2024

## Features

- Multi-device support (up to 4 DA7281 devices per I2C bus)
- Thread-safe I2C operations with FreeRTOS mutex protection
- Power control with proper 1.5ms sequencing per datasheet
- LRA configuration (170Hz resonant frequency, 6.75Ω impedance)
- Multiple operation modes: DRO, PWM, RTWM, ETWM
- Override mode for direct amplitude control
- Self-test functionality with result validation
- Comprehensive error handling and logging

## Hardware Configuration

### Supported Configurations

**Up to 4 Devices on Single I2C Bus:**
- Single TWI bus: 4 devices (addresses 0x48, 0x49, 0x4A, 0x4B)
- Or use multiple TWI buses for more devices

**I2C Address Selection (Datasheet Table 16, p58):**
- 0x48: ADDR_1=GND, ADDR_0=GND
- 0x49: ADDR_1=GND, ADDR_0=VDDIO
- 0x4A: ADDR_1=VDDIO, ADDR_0=GND
- 0x4B: ADDR_1=VDDIO, ADDR_0=VDDIO

### LRA Specifications
- Resonant Frequency: 170 Hz
- Impedance: 6.75Ω
- Nominal Max Voltage: 2.5V RMS
- Absolute Max Voltage: 3.5V peak
- Max Current: 350mA

## Project Structure

```
da7281-dwm3001c-hal/
├── include/
│   ├── da7281.h              # Main API header
│   ├── da7281_registers.h    # Register definitions
│   └── da7281_config.h       # Configuration options
├── src/
│   ├── da7281.c              # Core HAL implementation
│   └── da7281_i2c.c          # I2C wrapper with FreeRTOS mutex
├── config/
│   └── sdk_config.h          # Nordic SDK configuration
├── examples/
│   └── haptics_demo.c        # Usage example
└── docs/
    └── ARCHITECTURE.md       # Design documentation
```

## Integration into Qorvo SDK Project

### Step 1: Copy Files to Your Project

```bash
# Copy source files
cp src/da7281.c your_project/src/
cp src/da7281_i2c.c your_project/src/

# Copy headers
cp include/*.h your_project/include/

# Copy SDK config (merge with your existing sdk_config.h)
cp config/sdk_config.h your_project/config/
```

### Step 2: Add to Your Build System

**If using Makefile:**
```makefile
# Add to your source files
SRC_FILES += \
  src/da7281.c \
  src/da7281_i2c.c

# Add to include paths
INC_FOLDERS += \
  include/
```

**If using CMake:**
```cmake
# Add source files
target_sources(your_target PRIVATE
  src/da7281.c
  src/da7281_i2c.c
)

# Add include directories
target_include_directories(your_target PRIVATE
  include/
)
```

### Step 3: Configure SDK Settings

Ensure your `sdk_config.h` has these settings enabled:

```c
// Enable TWI/I2C
#define TWI0_ENABLED 1
#define TWI1_ENABLED 1  // If using second I2C bus

// FreeRTOS integration
#define NRFX_TWI_ENABLED 1
#define TWI_DEFAULT_CONFIG_FREQUENCY NRF_TWI_FREQ_400K
```

## Usage Examples

### Basic Single Device Example

```c
#include "da7281.h"
#include "FreeRTOS.h"
#include "task.h"

// Device configuration
da7281_device_t haptic_device = {
    .twi_instance = 0,           // TWI0
    .i2c_address = 0x48,         // ADDR_1=GND, ADDR_0=GND
    .gpio_enable_pin = 12,       // GPIO pin for power control
};

// LRA configuration (170Hz, 6.75Ω)
const da7281_lra_config_t lra_config = {
    .resonant_freq_hz = 170,
    .impedance_ohm = 6.75F,
    .nom_max_v_rms = 2.5F,
    .abs_max_v_peak = 3.5F,
    .max_current_ma = 350
};

void haptics_example(void)
{
    da7281_error_t err;

    // 1. Power on device
    err = da7281_power_on(&haptic_device);
    if (err != DA7281_OK) return;

    // 2. Initialize device
    err = da7281_init(&haptic_device);
    if (err != DA7281_OK) return;

    // 3. Configure LRA parameters
    err = da7281_configure_lra(&haptic_device, &lra_config);
    if (err != DA7281_OK) return;

    // 4. Set operation mode to DRO
    err = da7281_set_operation_mode(&haptic_device, DA7281_MODE_DRO);
    if (err != DA7281_OK) return;

    // 5. Enable amplifier
    err = da7281_set_amplifier_enable(&haptic_device, true);
    if (err != DA7281_OK) return;

    // 6. Play haptic effect
    da7281_set_override_amplitude(&haptic_device, 200);  // 78% amplitude
    vTaskDelay(pdMS_TO_TICKS(100));                      // 100ms duration
    da7281_set_override_amplitude(&haptic_device, 0);    // Stop

    // 7. Cleanup
    da7281_set_amplifier_enable(&haptic_device, false);
    da7281_deinit(&haptic_device);
    da7281_power_off(&haptic_device);
}
```

### Multi-Device Example

```c
// Configure 4 devices on single I2C bus (all 4 addresses)
da7281_device_t haptic_devices[4] = {
    {.twi_instance = 0, .i2c_address = 0x48, .gpio_enable_pin = 12},  // ADDR_1=GND, ADDR_0=GND
    {.twi_instance = 0, .i2c_address = 0x49, .gpio_enable_pin = 13},  // ADDR_1=GND, ADDR_0=VDDIO
    {.twi_instance = 0, .i2c_address = 0x4A, .gpio_enable_pin = 14},  // ADDR_1=VDDIO, ADDR_0=GND
    {.twi_instance = 0, .i2c_address = 0x4B, .gpio_enable_pin = 15}   // ADDR_1=VDDIO, ADDR_0=VDDIO
};

void multi_device_example(void)
{
    // Initialize all devices
    for (int i = 0; i < 4; i++) {
        da7281_power_on(&haptic_devices[i]);
        da7281_init(&haptic_devices[i]);
        da7281_configure_lra(&haptic_devices[i], &lra_config);
        da7281_set_operation_mode(&haptic_devices[i], DA7281_MODE_DRO);
        da7281_set_amplifier_enable(&haptic_devices[i], true);
    }

    // Play synchronized haptic pattern
    for (int i = 0; i < 4; i++) {
        da7281_set_override_amplitude(&haptic_devices[i], 150);
    }
    vTaskDelay(pdMS_TO_TICKS(200));
    for (int i = 0; i < 4; i++) {
        da7281_set_override_amplitude(&haptic_devices[i], 0);
    }
}
```

See `examples/haptics_demo.c` for a complete working example.

## API Reference

### Initialization Functions
- `da7281_init()` - Initialize DA7281 device
- `da7281_deinit()` - Deinitialize device
- `da7281_power_on()` - Power on device (1.5ms delay per datasheet)
- `da7281_power_off()` - Power off device

### Configuration Functions
- `da7281_configure_lra()` - Configure LRA parameters
- `da7281_set_operation_mode()` - Set operation mode (DRO/PWM/RTWM/ETWM)
- `da7281_set_amplifier_enable()` - Enable/disable amplifier

### Control Functions
- `da7281_set_override_amplitude()` - Set amplitude in override mode (0-255)
- `da7281_trigger_effect()` - Trigger waveform effect
- `da7281_run_self_test()` - Run device self-test

### Status Functions
- `da7281_get_status()` - Read device status register
- `da7281_check_fault()` - Check for fault conditions

See `include/da7281.h` for complete API documentation.

## Troubleshooting

**I2C Communication Fails:**
- Check I2C address matches hardware configuration (ADDR_0, ADDR_1 pins)
- Verify TWI instance is initialized in your project
- Check pull-up resistors on SDA/SCL lines

**Device Not Responding:**
- Verify power sequencing (1.5ms delay after enable)
- Check GPIO enable pin configuration
- Verify VDD and VDDIO supply voltages

**Haptic Effect Not Working:**
- Ensure amplifier is enabled
- Check LRA connection and specifications
- Verify operation mode is set correctly

## License

See LICENSE file for details.

## Contact

For issues and questions, please open an issue on GitHub.
