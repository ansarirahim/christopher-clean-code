# DA7281 HAL Testing Guide

## Overview

This document describes the testing strategy and procedures for the DA7281 HAL.

## Testing Levels

### 1. Unit Tests
- Test individual functions in isolation
- Mock hardware dependencies
- Fast execution
- High code coverage

### 2. Integration Tests
- Test with real hardware
- Verify I2C communication
- Test multi-device scenarios
- Verify thread safety

### 3. System Tests
- End-to-end testing
- Real-world usage scenarios
- Performance validation
- Stress testing

## Unit Testing

### Framework
- **Unity**: Lightweight C unit testing framework
- **CMock**: Automatic mock generation

### Test Structure
```
tests/
├── unit/
│   ├── test_da7281_init.c      # Initialization tests
│   ├── test_da7281_config.c    # Configuration tests
│   ├── test_da7281_i2c.c       # I2C layer tests
│   └── test_da7281_selftest.c  # Self-test tests
└── integration/
    └── test_integration.c       # Hardware integration tests
```

### Running Unit Tests
```bash
# Configure with tests enabled
mkdir build && cd build
cmake .. -DENABLE_TESTS=ON

# Build
make

# Run tests
ctest --output-on-failure

# Run with verbose output
ctest -V
```

### Test Coverage
```bash
# Configure with coverage enabled
cmake .. -DENABLE_TESTS=ON -DENABLE_COVERAGE=ON

# Build and run tests
make
ctest

# Generate coverage report
make coverage

# View report
firefox coverage/index.html
```

### Writing Unit Tests

#### Example Test Case
```c
void test_da7281_init_success(void)
{
    /* Setup */
    da7281_device_t device = {
        .twi_instance = 0,
        .i2c_address = 0x4A,
        .gpio_enable_pin = 12,
        .powered_on = true
    };

    /* Setup expectations */
    nrf_drv_twi_tx_ExpectAndReturn(&mock_twi, 0x4A, 
                                     chip_id_cmd, 1, true, NRF_SUCCESS);
    nrf_drv_twi_rx_ExpectAndReturn(&mock_twi, 0x4A,
                                     chip_id_data, 1, NRF_SUCCESS);

    /* Execute */
    da7281_error_t err = da7281_init(&device);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_OK, err);
    TEST_ASSERT_TRUE(device.initialized);
}
```

### Test Categories

#### 1. Positive Tests
- Valid inputs
- Expected behavior
- Success paths

#### 2. Negative Tests
- NULL pointers
- Invalid parameters
- Out-of-range values
- Error conditions

#### 3. Boundary Tests
- Minimum values
- Maximum values
- Edge cases

#### 4. State Tests
- Uninitialized device
- Already initialized
- Power state transitions

## Integration Testing

### Hardware Setup
```
DWM3001CDK Board
├── DA7281 #1 (TWI0, 0x4A)
│   ├── SCL → Pin 27
│   ├── SDA → Pin 26
│   ├── ADDR → GND
│   └── EN → Pin 12
└── DA7281 #2 (TWI0, 0x4B)
    ├── SCL → Pin 27
    ├── SDA → Pin 26
    ├── ADDR → VDD
    └── EN → Pin 13
```

### Integration Test Cases

#### Test 1: Single Device Initialization
```c
void test_single_device_init(void)
{
    da7281_device_t device = {
        .twi_instance = 0,
        .i2c_address = 0x4A,
        .gpio_enable_pin = 12
    };

    // Power on
    TEST_ASSERT_EQUAL(DA7281_OK, da7281_power_on(&device));

    // Initialize
    TEST_ASSERT_EQUAL(DA7281_OK, da7281_init(&device));

    // Verify chip ID
    uint8_t chip_id;
    TEST_ASSERT_EQUAL(DA7281_OK, da7281_read_chip_revision(&device, &chip_id));
    TEST_ASSERT_EQUAL(0xBA, chip_id);

    // Cleanup
    da7281_deinit(&device);
    da7281_power_off(&device);
}
```

#### Test 2: Multi-Device Initialization
```c
void test_multi_device_init(void)
{
    da7281_device_t devices[2] = {
        {.twi_instance = 0, .i2c_address = 0x4A, .gpio_enable_pin = 12},
        {.twi_instance = 0, .i2c_address = 0x4B, .gpio_enable_pin = 13}
    };

    // Initialize both devices
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_EQUAL(DA7281_OK, da7281_power_on(&devices[i]));
        TEST_ASSERT_EQUAL(DA7281_OK, da7281_init(&devices[i]));
    }

    // Verify both devices respond
    for (int i = 0; i < 2; i++) {
        uint8_t chip_id;
        TEST_ASSERT_EQUAL(DA7281_OK, 
                          da7281_read_chip_revision(&devices[i], &chip_id));
        TEST_ASSERT_EQUAL(0xBA, chip_id);
    }

    // Cleanup
    for (int i = 0; i < 2; i++) {
        da7281_deinit(&devices[i]);
        da7281_power_off(&devices[i]);
    }
}
```

#### Test 3: Thread Safety
```c
void haptic_task_1(void *pvParameters)
{
    da7281_device_t *device = (da7281_device_t *)pvParameters;
    
    for (int i = 0; i < 100; i++) {
        da7281_set_override_amplitude(device, 128);
        vTaskDelay(pdMS_TO_TICKS(10));
        da7281_set_override_amplitude(device, 0);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    
    vTaskDelete(NULL);
}

void test_thread_safety(void)
{
    // Create two tasks accessing different devices on same I2C bus
    xTaskCreate(haptic_task_1, "Haptic1", 2048, &device1, 1, NULL);
    xTaskCreate(haptic_task_1, "Haptic2", 2048, &device2, 1, NULL);
    
    // Wait for tasks to complete
    vTaskDelay(pdMS_TO_TICKS(5000));
    
    // Verify no I2C errors occurred
    TEST_ASSERT_EQUAL(0, i2c_error_count);
}
```

## Performance Testing

### Latency Measurements
```c
void test_haptic_latency(void)
{
    uint32_t start_time, end_time, latency_us;
    
    // Measure amplitude change latency
    start_time = get_microseconds();
    da7281_set_override_amplitude(&device, 200);
    end_time = get_microseconds();
    
    latency_us = end_time - start_time;
    
    // Should be < 100μs (I2C transaction time)
    TEST_ASSERT_LESS_THAN(100, latency_us);
}
```

### Throughput Testing
```c
void test_i2c_throughput(void)
{
    uint32_t start_time, end_time;
    const int num_transactions = 1000;
    
    start_time = get_milliseconds();
    
    for (int i = 0; i < num_transactions; i++) {
        da7281_write_register(&device, DA7281_REG_OVERRIDE_AMP, i % 256);
    }
    
    end_time = get_milliseconds();
    
    uint32_t duration_ms = end_time - start_time;
    float transactions_per_sec = (num_transactions * 1000.0F) / duration_ms;
    
    // Should achieve > 10,000 transactions/sec at 400kHz I2C
    TEST_ASSERT_GREATER_THAN(10000, transactions_per_sec);
}
```

## Stress Testing

### Continuous Operation Test
```c
void test_continuous_operation(void)
{
    const uint32_t test_duration_hours = 24;
    const uint32_t test_duration_ms = test_duration_hours * 3600 * 1000;
    
    uint32_t start_time = get_milliseconds();
    uint32_t error_count = 0;
    
    while ((get_milliseconds() - start_time) < test_duration_ms) {
        // Play haptic pattern
        for (uint8_t amp = 0; amp < 255; amp += 10) {
            if (da7281_set_override_amplitude(&device, amp) != DA7281_OK) {
                error_count++;
            }
            vTaskDelay(pdMS_TO_TICKS(50));
        }
    }
    
    // Should have zero errors
    TEST_ASSERT_EQUAL(0, error_count);
}
```

## Automated Testing (CI/CD)

### GitHub Actions Workflow
```yaml
# Runs on every push and pull request
- Build (Debug and Release)
- Run unit tests
- Generate coverage report
- Run static analysis
- Check coding standards
```

### Test Reports
- JUnit XML format for test results
- Cobertura XML format for coverage
- HTML reports for human review

## Manual Testing Checklist

### Hardware Verification
- [ ] Verify I2C communication (logic analyzer)
- [ ] Measure power consumption
- [ ] Verify GPIO control signals
- [ ] Test with oscilloscope on LRA output

### Functional Testing
- [ ] Single device initialization
- [ ] Multi-device initialization
- [ ] LRA configuration
- [ ] Self-test execution
- [ ] Override mode operation
- [ ] Amplitude control (0-255)
- [ ] Power on/off cycling

### Error Handling
- [ ] NULL pointer handling
- [ ] Invalid parameter rejection
- [ ] I2C error recovery
- [ ] Chip ID mismatch detection
- [ ] Timeout handling

### Thread Safety
- [ ] Concurrent access from multiple tasks
- [ ] No I2C transaction corruption
- [ ] No race conditions

## Test Metrics

### Coverage Targets
- **Line coverage**: >80%
- **Branch coverage**: >75%
- **Function coverage**: 100%

### Quality Metrics
- **Zero compiler warnings**: -Wall -Wextra -Werror
- **Zero static analysis errors**: cppcheck clean
- **MISRA-C compliance**: >95%

## Debugging Failed Tests

### Enable Verbose Logging
```c
#define DA7281_ENABLE_DEBUG_LOG 1
```

### Use Logic Analyzer
- Capture I2C transactions
- Verify timing
- Check ACK/NACK

### Use Debugger
- Set breakpoints in test code
- Inspect device state
- Step through HAL functions

## Document Information

**Date:** November 2024
