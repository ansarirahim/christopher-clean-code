/**
 * @file da7281.h
 * @brief DA7281 Haptic Driver HAL - Main API Header
 * @author A. R. Ansari
 * @date 2024-11-21
 * 
 * Hardware Abstraction Layer for Dialog DA7281 haptic driver IC
 * on Qorvo DWM3001C (nRF52833) with FreeRTOS.
 * 
 * Features:
 * - Multi-device support (up to 4 devices on 2 I2C buses)
 * - Thread-safe I2C operations with FreeRTOS mutex
 * - Power control with proper sequencing
 * - LRA configuration and calibration
 * - Multiple operation modes
 * - Self-test functionality
 * - Comprehensive error handling
 */

#ifndef DA7281_H
#define DA7281_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Includes
 * ======================================================================== */

#include <stdint.h>
#include <stdbool.h>
#include "da7281_registers.h"
#include "da7281_config.h"

/* ========================================================================
 * Type Definitions
 * ======================================================================== */

/**
 * @brief DA7281 error codes
 */
typedef enum {
    DA7281_OK = 0,                      /**< Success */
    DA7281_ERROR_NULL_POINTER,          /**< NULL pointer passed */
    DA7281_ERROR_INVALID_PARAM,         /**< Invalid parameter */
    DA7281_ERROR_I2C_WRITE,             /**< I2C write failed */
    DA7281_ERROR_I2C_READ,              /**< I2C read failed */
    DA7281_ERROR_TIMEOUT,               /**< Operation timeout */
    DA7281_ERROR_NOT_INITIALIZED,       /**< Device not initialized */
    DA7281_ERROR_ALREADY_INITIALIZED,   /**< Device already initialized */
    DA7281_ERROR_CHIP_REV_MISMATCH,     /**< Chip revision verification failed */
    DA7281_ERROR_SELFTEST_FAILED,       /**< Self-test failed */
    DA7281_ERROR_MUTEX_FAILED,          /**< Mutex operation failed */
    DA7281_ERROR_UNKNOWN                /**< Unknown error */
} da7281_error_t;

/**
 * @brief DA7281 operation modes
 */
typedef enum {
    DA7281_MODE_INACTIVE = DA7281_OP_MODE_INACTIVE,  /**< Inactive mode */
    DA7281_MODE_DRO = DA7281_OP_MODE_DRO,            /**< Direct Register Override */
    DA7281_MODE_PWM = DA7281_OP_MODE_PWM,            /**< PWM Input Mode */
    DA7281_MODE_RTWM = DA7281_OP_MODE_RTWM,          /**< Real-Time Waveform Mode */
    DA7281_MODE_ETWM = DA7281_OP_MODE_ETWM,          /**< Embedded Waveform Mode */
    DA7281_MODE_STANDBY = DA7281_OP_MODE_STANDBY     /**< Standby Mode */
} da7281_operation_mode_t;

/**
 * @brief DA7281 motor types
 */
typedef enum {
    DA7281_MOTOR_LRA = DA7281_MOTOR_TYPE_LRA,           /**< Linear Resonant Actuator */
    DA7281_MOTOR_ERM_BAR = DA7281_MOTOR_TYPE_ERM_BAR,   /**< ERM with back-EMF */
    DA7281_MOTOR_ERM_COIN = DA7281_MOTOR_TYPE_ERM_COIN  /**< ERM coin type */
} da7281_motor_type_t;

/**
 * @brief LRA configuration parameters
 */
typedef struct {
    uint16_t resonant_freq_hz;      /**< Resonant frequency in Hz (e.g., 170) */
    float impedance_ohm;            /**< Impedance in ohms (e.g., 6.75) */
    float nom_max_v_rms;            /**< Nominal max voltage in V RMS (e.g., 2.5) */
    float abs_max_v_peak;           /**< Absolute max voltage in V peak (e.g., 3.5) */
    uint16_t max_current_ma;        /**< Max current in mA (e.g., 350) */
} da7281_lra_config_t;

/**
 * @brief DA7281 device handle
 */
typedef struct {
    uint8_t twi_instance;           /**< TWI/I2C instance (0 or 1) */
    uint8_t i2c_address;            /**< I2C address (0x48, 0x49, 0x4A, or 0x4B) */
    bool initialized;               /**< Initialization status */
    da7281_operation_mode_t mode;   /**< Current operation mode */
    void *twi_handle;               /**< Platform-specific TWI handle */
} da7281_device_t;

/* ========================================================================
 * Function Prototypes - Initialization & Control
 * ======================================================================== */

/**
 * @brief Initialize DA7281 device
 * 
 * Initializes the DA7281 device, verifies chip revision, and configures
 * for basic operation.
 * 
 * @param[in,out] device Pointer to device handle
 * @return DA7281_OK on success, error code otherwise
 * 
 * @note This function must be called before any other DA7281 functions
 * @note Device must be powered on before calling this function
 */
da7281_error_t da7281_init(da7281_device_t *device);

/**
 * @brief Deinitialize DA7281 device
 * 
 * @param[in,out] device Pointer to device handle
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_deinit(da7281_device_t *device);

/* ========================================================================
 * Function Prototypes - Configuration
 * ======================================================================== */

/**
 * @brief Configure LRA parameters
 *
 * Calculates and configures all LRA-specific parameters including
 * resonance period, V2I factor, voltage limits, and current limit.
 *
 * @param[in] device Pointer to device handle
 * @param[in] config Pointer to LRA configuration
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_configure_lra(da7281_device_t *device,
                                     const da7281_lra_config_t *config);

/**
 * @brief Set operation mode
 *
 * @param[in] device Pointer to device handle
 * @param[in] mode Operation mode to set
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_set_operation_mode(da7281_device_t *device,
                                          da7281_operation_mode_t mode);

/**
 * @brief Get current operation mode
 *
 * @param[in] device Pointer to device handle
 * @param[out] mode Pointer to store current mode
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_get_operation_mode(da7281_device_t *device,
                                          da7281_operation_mode_t *mode);

/**
 * @brief Set override amplitude
 *
 * Sets the amplitude for Direct Register Override (DRO) mode.
 *
 * @param[in] device Pointer to device handle
 * @param[in] amplitude Amplitude value (0-255, 0=off, 255=max)
 * @return DA7281_OK on success, error code otherwise
 *
 * @note Device must be in DRO mode for this to take effect
 */
da7281_error_t da7281_set_override_amplitude(da7281_device_t *device,
                                               uint8_t amplitude);

/**
 * @brief Enable/disable amplifier
 *
 * @param[in] device Pointer to device handle
 * @param[in] enable true to enable, false to disable
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_set_amplifier_enable(da7281_device_t *device,
                                             bool enable);

/* ========================================================================
 * Function Prototypes - Self-Test & Diagnostics
 * ======================================================================== */

/**
 * @brief Run self-test
 *
 * Executes the built-in self-test and returns the result.
 *
 * @param[in] device Pointer to device handle
 * @param[out] passed Pointer to store test result (true=pass, false=fail)
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_run_selftest(da7281_device_t *device, bool *passed);

/**
 * @brief Read chip ID
 *
 * @param[in] device Pointer to device handle
 * @param[out] chip_id Pointer to store chip ID
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_read_chip_revision(da7281_device_t *device, uint8_t *chip_rev);

/**
 * @brief Read chip revision
 *
 * @param[in] device Pointer to device handle
 * @param[out] revision Pointer to store chip revision
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_read_chip_revision(da7281_device_t *device,
                                          uint8_t *revision);

/* ========================================================================
 * Function Prototypes - I2C Configuration
 * ======================================================================== */

/**
 * @brief Configure TWI/I2C pins for a specific instance
 *
 * This function MUST be called before da7281_init() to specify the hardware
 * pins for SCL and SDA. Pin assignments are board-specific.
 *
 * Example for custom DWM3001C board:
 *   da7281_i2c_configure_pins(0, 4, 5);  // TWI0: SCL=P0.4, SDA=P0.5
 *
 * Example for Nordic DK:
 *   da7281_i2c_configure_pins(0, 27, 26);  // TWI0: SCL=P0.27, SDA=P0.26
 *
 * @param[in] instance TWI instance number (0 or 1)
 * @param[in] scl_pin GPIO pin number for SCL (0-31)
 * @param[in] sda_pin GPIO pin number for SDA (0-31)
 * @return DA7281_OK on success, error code otherwise
 *
 * @note Must be called BEFORE any I2C operations
 * @note Consult your hardware schematic for correct pin numbers
 */
da7281_error_t da7281_i2c_configure_pins(uint8_t instance,
                                          uint8_t scl_pin,
                                          uint8_t sda_pin);

/* ========================================================================
 * Function Prototypes - Low-Level I2C (Internal Use)
 * ======================================================================== */

/**
 * @brief Write single byte to register
 *
 * @param[in] device Pointer to device handle
 * @param[in] reg_addr Register address
 * @param[in] value Value to write
 * @return DA7281_OK on success, error code otherwise
 *
 * @note This function is thread-safe (uses FreeRTOS mutex)
 */
da7281_error_t da7281_write_register(da7281_device_t *device,
                                       uint8_t reg_addr,
                                       uint8_t value);

/**
 * @brief Read single byte from register
 *
 * @param[in] device Pointer to device handle
 * @param[in] reg_addr Register address
 * @param[out] value Pointer to store read value
 * @return DA7281_OK on success, error code otherwise
 *
 * @note This function is thread-safe (uses FreeRTOS mutex)
 */
da7281_error_t da7281_read_register(da7281_device_t *device,
                                      uint8_t reg_addr,
                                      uint8_t *value);

/**
 * @brief Modify register bits
 *
 * Read-modify-write operation on a register.
 *
 * @param[in] device Pointer to device handle
 * @param[in] reg_addr Register address
 * @param[in] mask Bit mask
 * @param[in] value Value to set (will be masked)
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_modify_register(da7281_device_t *device,
                                        uint8_t reg_addr,
                                        uint8_t mask,
                                        uint8_t value);

#ifdef __cplusplus
}
#endif

#endif /* DA7281_H */
