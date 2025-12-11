# DA7281 Haptic Driver HAL for DWM3001C

Hardware Abstraction Layer for the DA7281 haptic driver (Dialog/Renesas) on Qorvo DWM3001C (nRF52833) using FreeRTOS.

## Project Overview

- **Platform:** Qorvo DWM3001C (nRF52833 SoC)
- **SDK:** Qorvo DWM3001CDK SDK v1.1.1 (based on Nordic SDK 17.1.0)
- **RTOS:** FreeRTOS
- **Device:** Renesas DA7281 haptic driver IC
- **Date:** November 2024

## Features

- Multi-device support (up to 4 DA7281 devices per I2C bus)
- Thread-safe I2C operations with FreeRTOS mutex protection
- Power control with 1.5 ms sequencing per datasheet
- LRA configuration (170 Hz resonant frequency, 6.75 ohm impedance)
- Supported operation modes: DRO, PWM, RTWM, ETWM (and streaming)
- Override mode for direct amplitude control (0-255)
- Comprehensive error handling and runtime logging

## Hardware Configuration

### Device addresses (Datasheet Table 16, p.58)
- `0x48` - ADDR_1 = GND, ADDR_0 = GND
- `0x49` - ADDR_1 = GND, ADDR_0 = VDDIO
- `0x4A` - ADDR_1 = VDDIO, ADDR_0 = GND
- `0x4B` - ADDR_1 = VDDIO, ADDR_0 = VDDIO

> You can place up to four DA7281s on a single I2C bus using the four address combinations above. If you need more devices you can spread them across multiple TWI instances.

### LRA example specs used for configuration
- Resonant frequency: **170 Hz**
- Actuator impedance: **6.75 ohm**
- Nominal max voltage: **2.5 V RMS**
- Absolute max voltage: **3.5 V peak**
- Target max current: **350 mA**

## Repository Layout

```
da7281-dwm3001c-hal/
+-- include/
|   +-- da7281.h
|   +-- da7281_registers.h
|   +-- da7281_config.h
+-- src/
|   +-- da7281.c
|   +-- da7281_i2c.c
+-- config/
|   +-- sdk_config.h
+-- examples/
|   +-- haptics_demo.c
+-- docs/
    +-- ARCHITECTURE.md
```

## Build & Test

This repository includes a standalone CMake build for verification purposes.

### Prerequisites

- **Toolchain:** `arm-none-eabi-gcc` (tested with version 14.2.1)
- **SDK:** Qorvo DWM3001CDK SDK v1.1.1 (or Nordic SDK 17.1.0)
- **Build tools:** CMake 3.20+, Make

### Build Instructions

```bash
# Clone the repository
git clone https://github.com/ansarirahim/da7281-dwm3001c-hal.git
cd da7281-dwm3001c-hal

# Set SDK path (if not using default ~/DW3_QM33_SDK/SDK_BSP/Nordic/SDK_17_1_0)
export QORVO_SDK_PATH=/path/to/your/DW3_QM33_SDK/SDK_BSP/Nordic/SDK_17_1_0

# Build
mkdir build && cd build
cmake ..
make

# Check object file sizes
make size
```

### Build Output

The build produces object files (not a standalone executable):
- `CMakeFiles/da7281_hal.dir/src/da7281.c.obj` (~4.3 KB)
- `CMakeFiles/da7281_hal.dir/src/da7281_i2c.c.obj`

These object files are ready to be linked into your final application.

### Compiler Flags

The build uses the following flags for ARM Cortex-M4:
- `-mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16`
- `-O2 -g` (optimized with debug symbols)
- `-Wall -Werror` (all warnings treated as errors)
- `-ffunction-sections -fdata-sections` (for linker garbage collection)

### Verification

The CMakeLists.txt includes ARM toolchain auto-selection to ensure consistent builds across different host systems (Ubuntu, macOS, Raspberry Pi, etc.).

## Integration (Qorvo / Nordic SDK)

### Step 1 - Copy files
```bash
cp src/da7281.c your_project/src/
cp src/da7281_i2c.c your_project/src/
cp include/*.h your_project/include/
cp config/sdk_config.h your_project/config/   # merge as needed
```

### Step 2 - Build system

**Makefile**

```makefile
SRC_FILES += \
  src/da7281.c \
  src/da7281_i2c.c

INC_FOLDERS += \
  include/
```

**CMake**

```cmake
target_sources(your_target PRIVATE
  src/da7281.c
  src/da7281_i2c.c)

target_include_directories(your_target PRIVATE
  include/)
```

### Step 3 - SDK config

Ensure `sdk_config.h` enables TWI/FreeRTOS hooks required:

```c
#define NRFX_TWI_ENABLED 1
#define TWI0_ENABLED 1
#define TWI1_ENABLED 1   // if using second bus
#define TWI_DEFAULT_CONFIG_FREQUENCY NRF_TWI_FREQ_400K
```

### Step 4 - Logging Configuration

The HAL supports multiple logging backends. Configure `DA7281_LOG_BACKEND` before including the HAL headers:

| Value | Backend | Description |
|-------|---------|-------------|
| 0 | Disabled | No logging output |
| 1 | NRF_LOG | Nordic SDK (default) - RTT or UART via `sdk_config.h` |
| 2 | QORVO_SDK | Uses `QLOGE`/`QLOGW`/`QLOGI`/`QLOGD` (JLinkRTTViewer) |
| 3 | SEGGER_RTT | Direct SEGGER RTT (bypasses Nordic wrapper) |
| 4 | UART_PRINTF | Direct `printf` to UART |
| 5 | CUSTOM | User-defined macros |

**For Qorvo DWM3001CDK with JLinkRTTViewer** (recommended for HelloWorld integration):

```c
// In your project's config or before including da7281.h
#define DA7281_LOG_BACKEND 2   // Use QLOGE/QLOGW/QLOGI/QLOGD
#include "da7281.h"
```

**For UART via minicom:**

```c
#define DA7281_LOG_BACKEND 4   // Direct printf to UART
#include "da7281.h"
```

**Custom logging backend:**

```c
// Define your macros BEFORE including da7281_config.h
#define DA7281_LOG_BACKEND 5
#define DA7281_LOG_ERROR(...)   my_log_error(__VA_ARGS__)
#define DA7281_LOG_WARNING(...) my_log_warning(__VA_ARGS__)
#define DA7281_LOG_INFO(...)    my_log_info(__VA_ARGS__)
#define DA7281_LOG_DEBUG(...)   my_log_debug(__VA_ARGS__)
#include "da7281.h"
```

## Usage Example (single device)

```c
#include "da7281.h"
#include "FreeRTOS.h"
#include "task.h"

da7281_device_t haptic_device = {
    .twi_instance = 0,
    .i2c_address = 0x48,
    .gpio_enable_pin = 12,
};

const da7281_lra_config_t lra_config = {
    .resonant_freq_hz = 170,
    .impedance_ohm    = 6.75f,
    .nom_max_v_rms    = 2.5f,
    .abs_max_v_peak   = 3.5f,
    .max_current_ma   = 350
};

void haptics_example(void)
{
    if (da7281_power_on(&haptic_device) != DA7281_OK) return;
    if (da7281_init(&haptic_device) != DA7281_OK) return;
    if (da7281_configure_lra(&haptic_device, &lra_config) != DA7281_OK) return;

    da7281_set_operation_mode(&haptic_device, DA7281_MODE_DRO);
    da7281_set_amplifier_enable(&haptic_device, true);
    da7281_set_override_amplitude(&haptic_device, 200);
    vTaskDelay(pdMS_TO_TICKS(100));
    da7281_set_override_amplitude(&haptic_device, 0);

    da7281_set_amplifier_enable(&haptic_device, false);
    da7281_deinit(&haptic_device);
    da7281_power_off(&haptic_device);
}
```

## Multi-device example

(See `examples/haptics_demo.c` for complete sample.)
## API Summary

* `da7281_power_on()`, `da7281_power_off()`
* `da7281_init()`, `da7281_deinit()`
* `da7281_configure_lra()`
* `da7281_set_operation_mode()`
* `da7281_set_amplifier_enable()`
* `da7281_set_override_amplitude()`
* `da7281_run_self_test()`
* `da7281_get_status()`, `da7281_check_fault()`

See `include/da7281.h` for full prototypes and doxygen comments.

## Troubleshooting

**I2C fails / no ACK**

* Confirm ADDR_0/ADDR_1 wiring & correct device address.
* Confirm TWI instance is initialized.
* Confirm SDA/SCL pull-ups and electrical wiring.

**Device not responding**

* Confirm power rail sequencing (1.5 ms delay after enabling VDDIO/VDD).
* Confirm enable GPIO is configured correctly.
* Check supply voltages (VDD / VDDIO).

**Haptics silent or weak**

* Ensure amplifier is enabled.
* Verify LRA is connected and matches configuration (impedance & resonance).
* Verify OP_MODE = DRO (or correct mode) before override drive.

## License

See LICENSE.

## Contact

Open an issue on GitHub for bugs, logs, or questions.
