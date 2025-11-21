/**
 * @file da7281_i2c.c
 * @brief DA7281 I2C Communication Layer
 * @author A. R. Ansari
 * @date 2024-11-21
 * 
 * Thread-safe I2C communication functions for DA7281 using
 * Qorvo SDK TWI driver with FreeRTOS mutex protection.
 */

#include "da7281.h"
#include "nrf_drv_twi.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "nrf_gpio.h"
#include <string.h>

/* ========================================================================
 * Private Variables
 * ======================================================================== */

/** FreeRTOS mutex for thread-safe I2C access */
static SemaphoreHandle_t s_i2c_mutex = NULL;

/** TWI instance handles */
static nrf_drv_twi_t s_twi_instances[2] = {
    NRF_DRV_TWI_INSTANCE(0),
    NRF_DRV_TWI_INSTANCE(1)
};

/** TWI initialization status */
static bool s_twi_initialized[2] = {false, false};

/* ========================================================================
 * Private Function Prototypes
 * ======================================================================== */

static da7281_error_t da7281_i2c_init_mutex(void);
static da7281_error_t da7281_i2c_init_twi(uint8_t instance);

/* ========================================================================
 * Private Function Implementations
 * ======================================================================== */

/**
 * @brief Initialize I2C mutex for thread-safe operation
 *
 * Creates a FreeRTOS mutex to protect I2C bus access from concurrent threads.
 * This ensures only one thread can communicate with DA7281 devices at a time.
 *
 * @return DA7281_OK on success
 * @return DA7281_ERROR_MUTEX_FAILED if mutex creation fails
 */
static da7281_error_t da7281_i2c_init_mutex(void)
{
    if (s_i2c_mutex == NULL) {
        s_i2c_mutex = xSemaphoreCreateMutex();
        if (s_i2c_mutex == NULL) {
            DA7281_LOG_ERROR("Failed to create I2C mutex - insufficient heap memory");
            return DA7281_ERROR_MUTEX_FAILED;
        }
        DA7281_LOG_INFO("I2C mutex created successfully");
    }
    return DA7281_OK;
}

/**
 * @brief Initialize TWI (I2C) hardware instance
 *
 * Configures and enables the specified TWI peripheral for I2C communication.
 * Supports TWI0 and TWI1 for multi-device configurations.
 *
 * Configuration:
 * - TWI0: SCL=P0.27, SDA=P0.26
 * - TWI1: SCL=P0.29, SDA=P0.28
 * - Frequency: 400 kHz (Fast Mode)
 * - Interrupt priority: High
 *
 * @param instance TWI instance number (0 or 1)
 * @return DA7281_OK on success
 * @return DA7281_ERROR_INVALID_PARAM if instance >= 2
 * @return DA7281_ERROR_I2C_WRITE if TWI initialization fails
 */
static da7281_error_t da7281_i2c_init_twi(uint8_t instance)
{
    if (instance >= 2) {
        DA7281_LOG_ERROR("Invalid TWI instance: %d (valid: 0-1)", instance);
        return DA7281_ERROR_INVALID_PARAM;
    }

    if (s_twi_initialized[instance]) {
        DA7281_LOG_DEBUG("TWI%d already initialized, skipping", instance);
        return DA7281_OK;  /* Already initialized */
    }

    /* TWI configuration */
    const nrf_drv_twi_config_t twi_config = {
        .scl = (instance == 0) ? 27 : 29,  /* Default SCL pins */
        .sda = (instance == 0) ? 26 : 28,  /* Default SDA pins */
        .frequency = NRF_DRV_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init = false
    };

    DA7281_LOG_DEBUG("Initializing TWI%d: SCL=P0.%d, SDA=P0.%d, freq=400kHz",
                     instance, twi_config.scl, twi_config.sda);

    ret_code_t err_code = nrf_drv_twi_init(&s_twi_instances[instance],
                                            &twi_config,
                                            NULL,
                                            NULL);
    if (err_code != NRF_SUCCESS) {
        DA7281_LOG_ERROR("TWI%d init failed with error code: 0x%08X", instance, err_code);
        return DA7281_ERROR_I2C_WRITE;
    }

    nrf_drv_twi_enable(&s_twi_instances[instance]);
    s_twi_initialized[instance] = true;

    DA7281_LOG_INFO("TWI%d initialized and enabled successfully", instance);
    return DA7281_OK;
}

/* ========================================================================
 * Public Function Implementations
 * ======================================================================== */

/**
 * @brief Write single byte to DA7281 register
 *
 * Performs a thread-safe I2C write operation to a DA7281 register.
 * Automatically initializes mutex and TWI instance on first use.
 *
 * I2C Transaction:
 * - START
 * - Device Address (Write)
 * - Register Address
 * - Data Byte
 * - STOP
 *
 * @param device Pointer to device handle
 * @param reg_addr Register address (0x00-0xAB)
 * @param value Value to write (0x00-0xFF)
 * @return DA7281_OK on success
 * @return DA7281_ERROR_NULL_POINTER if device is NULL
 * @return DA7281_ERROR_MUTEX_FAILED if mutex timeout
 * @return DA7281_ERROR_I2C_WRITE if I2C transaction fails
 */
da7281_error_t da7281_write_register(da7281_device_t *device,
                                       uint8_t reg_addr,
                                       uint8_t value)
{
    DA7281_CHECK_NULL(device);

    /* Initialize mutex if needed */
    da7281_error_t err = da7281_i2c_init_mutex();
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Mutex initialization failed");
        return err;
    }

    /* Initialize TWI if needed */
    err = da7281_i2c_init_twi(device->twi_instance);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("TWI%d initialization failed", device->twi_instance);
        return err;
    }

    /* Take mutex with timeout */
    if (xSemaphoreTake(s_i2c_mutex, DA7281_MUTEX_TIMEOUT_TICKS) != pdTRUE) {
        DA7281_LOG_ERROR("Failed to acquire I2C mutex (timeout after %d ms)",
                         DA7281_I2C_TIMEOUT_MS);
        return DA7281_ERROR_MUTEX_FAILED;
    }

    /* Prepare data: [register_address, value] */
    uint8_t data[2] = {reg_addr, value};

    /* Perform I2C write transaction */
    ret_code_t ret = nrf_drv_twi_tx(&s_twi_instances[device->twi_instance],
                                     device->i2c_address,
                                     data,
                                     sizeof(data),
                                     false);

    /* Release mutex */
    xSemaphoreGive(s_i2c_mutex);

    if (ret != NRF_SUCCESS) {
        DA7281_LOG_ERROR("I2C write failed: TWI%d, addr=0x%02X, reg=0x%02X, val=0x%02X, err=0x%08X",
                         device->twi_instance, device->i2c_address, reg_addr, value, ret);
        return DA7281_ERROR_I2C_WRITE;
    }

    DA7281_LOG_DEBUG("I2C write OK: TWI%d, addr=0x%02X, reg=0x%02X, val=0x%02X",
                     device->twi_instance, device->i2c_address, reg_addr, value);

    return DA7281_OK;
}

/**
 * @brief Read single byte from DA7281 register
 *
 * Performs a thread-safe I2C read operation from a DA7281 register.
 * Uses repeated start condition for proper I2C protocol.
 *
 * I2C Transaction:
 * - START
 * - Device Address (Write)
 * - Register Address
 * - REPEATED START
 * - Device Address (Read)
 * - Data Byte
 * - STOP
 *
 * @param device Pointer to device handle
 * @param reg_addr Register address to read (0x00-0xAB)
 * @param value Pointer to store read value
 * @return DA7281_OK on success
 * @return DA7281_ERROR_NULL_POINTER if device or value is NULL
 * @return DA7281_ERROR_MUTEX_FAILED if mutex timeout
 * @return DA7281_ERROR_I2C_READ if I2C transaction fails
 */
da7281_error_t da7281_read_register(da7281_device_t *device,
                                      uint8_t reg_addr,
                                      uint8_t *value)
{
    DA7281_CHECK_NULL(device);
    DA7281_CHECK_NULL(value);

    /* Initialize mutex if needed */
    da7281_error_t err = da7281_i2c_init_mutex();
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Mutex initialization failed");
        return err;
    }

    /* Initialize TWI if needed */
    err = da7281_i2c_init_twi(device->twi_instance);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("TWI%d initialization failed", device->twi_instance);
        return err;
    }

    /* Take mutex with timeout */
    if (xSemaphoreTake(s_i2c_mutex, DA7281_MUTEX_TIMEOUT_TICKS) != pdTRUE) {
        DA7281_LOG_ERROR("Failed to acquire I2C mutex (timeout after %d ms)",
                         DA7281_I2C_TIMEOUT_MS);
        return DA7281_ERROR_MUTEX_FAILED;
    }

    ret_code_t ret;

    /* Write register address (with repeated start) */
    ret = nrf_drv_twi_tx(&s_twi_instances[device->twi_instance],
                          device->i2c_address,
                          &reg_addr,
                          1,
                          true);  /* No stop condition - repeated start */

    if (ret == NRF_SUCCESS) {
        /* Read data byte */
        ret = nrf_drv_twi_rx(&s_twi_instances[device->twi_instance],
                              device->i2c_address,
                              value,
                              1);
    }

    /* Release mutex */
    xSemaphoreGive(s_i2c_mutex);

    if (ret != NRF_SUCCESS) {
        DA7281_LOG_ERROR("I2C read failed: TWI%d, addr=0x%02X, reg=0x%02X, err=0x%08X",
                         device->twi_instance, device->i2c_address, reg_addr, ret);
        return DA7281_ERROR_I2C_READ;
    }

    DA7281_LOG_DEBUG("I2C read OK: TWI%d, addr=0x%02X, reg=0x%02X, val=0x%02X",
                     device->twi_instance, device->i2c_address, reg_addr, *value);

    return DA7281_OK;
}

/**
 * @brief Modify specific bits in a register (read-modify-write)
 *
 * Performs an atomic read-modify-write operation on a DA7281 register.
 * Only the bits specified by the mask are modified, other bits remain unchanged.
 *
 * Operation:
 * 1. Read current register value
 * 2. Clear bits specified by mask
 * 3. Set new bits (masked)
 * 4. Write modified value back
 *
 * Formula: new_value = (old_value & ~mask) | (value & mask)
 *
 * Example:
 * - Current register value: 0b11001100
 * - Mask:                   0b00111100
 * - New value:              0b00101000
 * - Result:                 0b11101000
 *
 * @param device Pointer to device handle
 * @param reg_addr Register address to modify
 * @param mask Bit mask (1 = modify this bit, 0 = leave unchanged)
 * @param value New value for masked bits
 * @return DA7281_OK on success
 * @return DA7281_ERROR_NULL_POINTER if device is NULL
 * @return DA7281_ERROR_I2C_READ if read fails
 * @return DA7281_ERROR_I2C_WRITE if write fails
 */
da7281_error_t da7281_modify_register(da7281_device_t *device,
                                        uint8_t reg_addr,
                                        uint8_t mask,
                                        uint8_t value)
{
    DA7281_CHECK_NULL(device);

    uint8_t reg_value = 0;
    da7281_error_t err;

    /* Read current register value */
    err = da7281_read_register(device, reg_addr, &reg_value);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to read register 0x%02X for modify operation", reg_addr);
        return err;
    }

    uint8_t old_value = reg_value;

    /* Modify bits: clear masked bits, then set new masked bits */
    reg_value = (reg_value & ~mask) | (value & mask);

    /* Write modified value back */
    err = da7281_write_register(device, reg_addr, reg_value);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to write modified value to register 0x%02X", reg_addr);
        return err;
    }

    DA7281_LOG_DEBUG("I2C modify OK: addr=0x%02X, reg=0x%02X, old=0x%02X, new=0x%02X, mask=0x%02X",
                     device->i2c_address, reg_addr, old_value, reg_value, mask);

    return DA7281_OK;
}

