# DA7281 Haptic Driver HAL for DWM3001C

Professional Hardware Abstraction Layer for Dialog DA7281 haptic driver on Qorvo DWM3001C (nRF52833) with FreeRTOS.

## Project Information

- **Platform:** Qorvo DWM3001C (nRF52833 SoC)
- **SDK:** Qorvo DWM3001CDK SDK v1.1.1
- **RTOS:** FreeRTOS
- **Device:** Dialog DA7281 Haptic Driver IC
- **Date:** November 2024

## Features

### Core Functionality
- ✅ Multi-device support (up to 4 DA7281 devices on 2 I2C buses)
- ✅ Thread-safe I2C operations with FreeRTOS mutex protection
- ✅ Power control with proper 1.5ms sequencing per datasheet
- ✅ LRA configuration (170Hz resonant frequency, 6.75Ω impedance)
- ✅ Multiple operation modes: DRO, PWM, RTWM, ETWM
- ✅ Override mode for direct amplitude control
- ✅ Self-test functionality with result validation
- ✅ Comprehensive error handling and logging

### Code Quality
- ✅ MISRA-C compliant coding standards
- ✅ Complete unit test coverage
- ✅ CI/CD pipeline with automated testing
- ✅ Doxygen documentation
- ✅ Static analysis integration
- ✅ Code coverage reporting

## Hardware Configuration

### Supported Configurations

**4 Devices on 2 I2C Buses:**
- TWI0: 2 devices (addresses 0x4A, 0x4B)
- TWI1: 2 devices (addresses 0x4A, 0x4B)

**I2C Address Selection:**
- 0x4A: ADDR pin tied to GND
- 0x4B: ADDR pin tied to VDD

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
│   ├── da7281_i2c.c          # I2C wrapper
│   ├── da7281_power.c        # Power control
│   └── da7281_lra.c          # LRA configuration
├── examples/
│   └── haptics_demo.c        # Usage example
├── tests/
│   ├── unit/                 # Unit tests
│   └── integration/          # Integration tests
├── docs/
│   ├── ARCHITECTURE.md       # Design documentation
│   ├── API.md                # API reference
│   └── Doxyfile              # Doxygen configuration
├── .github/
│   └── workflows/
│       └── ci.yml            # CI/CD pipeline
└── CMakeLists.txt            # Build configuration
```

## Quick Start

### Prerequisites

#### Hardware
- Qorvo DWM3001CDK development board (nRF52833)
- DA7281 haptic driver IC(s)
- LRA actuator(s) (170Hz, 6.75Ω recommended)

#### Software
- **Qorvo DWM3001CDK SDK v1.1.1** or later
- **ARM GCC Toolchain** (10.3-2021.10 or later)
- **CMake** 3.20 or later
- **Ninja** build system (optional but recommended)
- **Git** for version control

### Installation

#### 1. Install ARM GCC Toolchain
```bash
# Ubuntu/Debian
sudo apt-get install gcc-arm-none-eabi

# macOS
brew install gcc-arm-embedded

# Windows - Download from:
# https://developer.arm.com/downloads/-/gnu-rm
```

#### 2. Install CMake and Ninja
```bash
# Ubuntu/Debian
sudo apt-get install cmake ninja-build

# macOS
brew install cmake ninja

# Windows - Download from:
# https://cmake.org/download/
```

#### 3. Download Qorvo SDK
Download from Qorvo website and extract to a known location (e.g., `~/qorvo_sdk`)

### Build Instructions

```bash
# Clone repository
git clone https://github.com/yourusername/da7281-dwm3001c-hal.git
cd da7281-dwm3001c-hal

# Create build directory
mkdir build && cd build

# Configure (set SDK path)
cmake .. -G "Ninja" \
  -DCMAKE_BUILD_TYPE=Release \
  -DQORVO_SDK_PATH=/path/to/qorvo_sdk \
  -DENABLE_EXAMPLES=ON

# Build
ninja

# Run tests (if enabled)
ctest --output-on-failure
```

### Integration into Existing Project

```bash
# Copy HAL files to your project
cp -r include/ your_project/components/da7281/
cp -r src/ your_project/components/da7281/

# Add to your Makefile or CMakeLists.txt:
# - Include directories: da7281/include/
# - Source files: da7281/src/*.c
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
    .i2c_address = 0x4A,         // ADDR pin to GND
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
// Configure 4 devices (2 on TWI0, 2 on TWI1)
da7281_device_t haptic_devices[4] = {
    {.twi_instance = 0, .i2c_address = 0x4A, .gpio_enable_pin = 12},
    {.twi_instance = 0, .i2c_address = 0x4B, .gpio_enable_pin = 13},
    {.twi_instance = 1, .i2c_address = 0x4A, .gpio_enable_pin = 14},
    {.twi_instance = 1, .i2c_address = 0x4B, .gpio_enable_pin = 15}
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

## Testing

### Unit Tests
```bash
cd build
ninja test
```

### Code Coverage
```bash
cmake .. -DENABLE_COVERAGE=ON
ninja coverage
```

### Static Analysis
```bash
ninja static-analysis
```

## Documentation

Generate API documentation:
```bash
cd docs
doxygen Doxyfile
```

View documentation: `docs/html/index.html`

## Status

🚧 **In Development**
- Start Date: November 20, 2024
- Milestone: November 26, 2024
- Target Completion: December 1, 2024

