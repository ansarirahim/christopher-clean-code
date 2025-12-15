/**
 * @file da7281.c
 * @brief DA7281 HAL Core Implementation
 * @author A. R. Ansari
 * @date 2024-11-21
 */

#include "da7281.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include <math.h>

/* ========================================================================
 * Initialization & Control Functions
 * ======================================================================== */

/**
 * @brief Initialize DA7281 device
 *
 * Performs complete device initialization sequence:
 * 1. Verify chip ID (must be 0xBA)
 * 2. Read chip revision
 * 3. Configure motor type as LRA
 * 4. Set initial operation mode to INACTIVE
 *
 * Prerequisites:
 * - I2C bus must be functional
 *
 * @param device Pointer to device handle
 * @return DA7281_OK on success
 * @return DA7281_ERROR_NULL_POINTER if device is NULL
 * @return DA7281_ERROR_ALREADY_INITIALIZED if already initialized
 * @return DA7281_ERROR_CHIP_REV_MISMATCH if chip revision != expected
 * @return DA7281_ERROR_I2C_READ/WRITE on communication failure
 */
da7281_error_t da7281_init(da7281_device_t *device)
{
    DA7281_CHECK_NULL(device);

    if (device->initialized) {
        DA7281_LOG_WARNING("Device already initialized");
        return DA7281_ERROR_ALREADY_INITIALIZED;
    }

    da7281_error_t err;
    uint8_t chip_rev = 0;

    DA7281_LOG_INFO("Starting device initialization...");

    /* Read and verify chip revision */
    err = da7281_read_chip_revision(device, &chip_rev);
    DA7281_LOG_DEBUG("Error code: %d", err);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to read chip revision - I2C communication error");
        return err;
    }

    DA7281_LOG_DEBUG("Chip revision: 0x%02X", chip_rev);
    if ((chip_rev != DA7281_CHIP_REV_VALUE) &&
        (chip_rev != DA7281_CHIP_REV_LEGACY_VALUE)) {
        DA7281_LOG_ERROR("Chip revision mismatch: expected 0x%02X or 0x%02X, got 0x%02X",
                         DA7281_CHIP_REV_VALUE,
                         DA7281_CHIP_REV_LEGACY_VALUE,
                         chip_rev);
        DA7281_LOG_ERROR("Possible causes: wrong I2C address, hardware fault, or not a DA7281");
        return DA7281_ERROR_CHIP_REV_MISMATCH;
    }

    DA7281_LOG_INFO("Chip revision verified: 0x%02X (DA7281 detected)", chip_rev);

    /* Clear any pending fault bits by writing 1s to IRQ_EVENT1 */
    DA7281_LOG_DEBUG("Clearing fault bits...");
    err = da7281_write_register(device, DA7281_REG_IRQ_EVENT1, 0xFFU);
    if (err != DA7281_OK) {
        DA7281_LOG_WARNING("Failed to clear fault bits");
    }

    /* Set actuator type to LRA in TOP_CFG1 bit 5 */
    DA7281_LOG_DEBUG("Configuring actuator type as LRA...");
    err = da7281_modify_register(device,
                                   DA7281_REG_TOP_CFG1,
                                   DA7281_TOP_CFG1_ACTUATOR_TYPE,
                                   DA7281_ACTUATOR_TYPE_LRA);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to set actuator type to LRA");
        return err;
    }

    /* Verify actuator type was set correctly */
    uint8_t top_cfg1 = 0;
    err = da7281_read_register(device, DA7281_REG_TOP_CFG1, &top_cfg1);
    if (err == DA7281_OK) {
        uint8_t actuator_type = top_cfg1 & DA7281_TOP_CFG1_ACTUATOR_TYPE;
        if (actuator_type == DA7281_ACTUATOR_TYPE_LRA) {
            DA7281_LOG_INFO("Actuator type verified: LRA");
        } else {
            DA7281_LOG_WARNING("Actuator type verification failed: expected LRA, got 0x%02X", actuator_type);
        }
    }

    /* Mark initialized before calling set_operation_mode so guard passes.
     * Roll back on failure. */
    device->initialized = true;

    /* Set to inactive mode initially */
    DA7281_LOG_DEBUG("Setting initial operation mode to INACTIVE...");
    err = da7281_set_operation_mode(device, DA7281_MODE_INACTIVE);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to set operation mode to INACTIVE");
        device->initialized = false;
        return err;
    }

    device->mode = DA7281_MODE_INACTIVE;

    DA7281_LOG_INFO("Device initialized successfully (TWI%d, addr=0x%02X)",
                    device->twi_instance, device->i2c_address);

    return DA7281_OK;
}

/**
 * @brief Deinitialize DA7281 device
 */
da7281_error_t da7281_deinit(da7281_device_t *device)
{
    DA7281_CHECK_NULL(device);

    if (!device->initialized) {
        return DA7281_OK;
    }

    /* Set to inactive mode */
    (void)da7281_set_operation_mode(device, DA7281_MODE_INACTIVE);

    /* Disable amplifier */
    (void)da7281_set_amplifier_enable(device, false);

    device->initialized = false;

    DA7281_LOG_INFO("Device deinitialized");

    return DA7281_OK;
}

/* ========================================================================
 * Configuration Functions
 * ======================================================================== */

/**
 * @brief Configure LRA (Linear Resonant Actuator) parameters
 *
 * Calculates and programs all LRA-specific registers based on motor specifications.
 * This function must be called after initialization and before starting haptic playback.
 *
 * Register Calculations (per DA7281 Datasheet v3.1):
 *
 * 1. LRA_PER (Period Register):
 *    Formula: LRA_PER = T / 1.33332e-9, where T = 1 / f_resonant
 *    Example: For 170Hz LRA:
 *      T = 1/170 = 0.00588s
 *      LRA_PER = 0.00588 / 1.33332e-9 = 4412295
 *
 * 2. V2I_FACTOR (Voltage-to-Current Factor):
 *    Formula: V2I = (Z * (IMAX_reg + 4)) / 1.6104
 *    Example: For Z=6.75 ohm, IMAX=350mA:
 *      IMAX_reg = (350 - 28.6) / 7.2 = 44.6
 *      V2I = (6.75 * (44.6 + 4)) / 1.6104 = 204
 *
 * 3. ACTUATOR_NOMMAX (Nominal Maximum Voltage):
 *    Formula: NOMMAX = (V_rms * 1000) / 23.4
 *    Example: For 2.5V RMS:
 *      NOMMAX = (2.5 * 1000) / 23.4 = 107
 *
 * 4. ACTUATOR_ABSMAX (Absolute Maximum Voltage):
 *    Formula: ABSMAX = (V_peak * 1000) / 23.4
 *    Example: For 3.5V peak:
 *      ABSMAX = (3.5 * 1000) / 23.4 = 150
 *
 * 5. ACTUATOR_IMAX (Maximum Current):
 *    Formula: IMAX = (I_mA - 28.6) / 7.2
 *    Example: For 350mA:
 *      IMAX = (350 - 28.6) / 7.2 = 44.6 ~ 45
 *
 * @param device Pointer to initialized device handle
 * @param config Pointer to LRA configuration structure
 * @return DA7281_OK on success
 * @return DA7281_ERROR_NULL_POINTER if device or config is NULL
 * @return DA7281_ERROR_NOT_INITIALIZED if device not initialized
 * @return DA7281_ERROR_INVALID_PARAM if parameters out of range
 * @return DA7281_ERROR_I2C_WRITE if register write fails
 */
da7281_error_t da7281_configure_lra(da7281_device_t *device,
                                     const da7281_lra_config_t *config)
{
    DA7281_CHECK_DEVICE(device);
    DA7281_CHECK_NULL(config);

    /* Validate parameters against datasheet limits */
    DA7281_CHECK_RANGE(config->resonant_freq_hz, 50, 300);
    DA7281_CHECK_RANGE(config->impedance_ohm, 1.0F, 50.0F);
    DA7281_CHECK_RANGE(config->nom_max_v_rms, 0.5F, 6.0F);
    DA7281_CHECK_RANGE(config->abs_max_v_peak, 1.0F, 12.0F);
    DA7281_CHECK_RANGE(config->max_current_ma, 50, 500);

    da7281_error_t err;

    /* ===== 1. Configure LRA Period ===== */
    /* Calculate period in seconds, then convert to register value */
    /* DA7281 Datasheet: LRA_PER = T / 1.33332e-9 */
    float period_seconds = 1.0F / (float)config->resonant_freq_hz;
    float lra_per_float = period_seconds / DA7281_LRA_PER_TIME_SCALE;

    /* Round to nearest integer and clamp to valid 16-bit range (1-65535) */
    uint16_t lra_per = (uint16_t)roundf(lra_per_float);
    if (lra_per == 0) {
        lra_per = 1;  /* Minimum valid value */
        DA7281_LOG_WARNING("LRA_PER calculated as 0, clamped to 1");
    }

    DA7281_LOG_DEBUG("LRA period calculation: f=%uHz, T=%.6fs, LRA_PER=0x%04X (rounded from %.2f)",
                     config->resonant_freq_hz, period_seconds, lra_per, lra_per_float);

    /* Write LRA period (16-bit register, high byte first) */
    err = da7281_write_register(device, DA7281_REG_LRA_PER_H,
                                  (uint8_t)(lra_per >> 8));
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to write LRA_PER_H register");
        return err;
    }

    err = da7281_write_register(device, DA7281_REG_LRA_PER_L,
                                  (uint8_t)(lra_per & 0xFF));
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to write LRA_PER_L register");
        return err;
    }

    DA7281_LOG_INFO("LRA period configured: %u Hz -> LRA_PER=0x%04X",
                    config->resonant_freq_hz, lra_per);

    /* ===== 2. Configure V2I Factor ===== */
    /* V2I factor converts voltage to current based on actuator impedance */
    /* DA7281 Datasheet: V2I_FACTOR = (Z * (IMAX_reg + 4)) / 1.6104 */
    /* First calculate IMAX register value for the formula */
    float imax_reg = ((float)config->max_current_ma - DA7281_ACTUATOR_IMAX_OFFSET) / DA7281_ACTUATOR_IMAX_SCALE;
    float v2i_float = (config->impedance_ohm * (imax_reg + DA7281_V2I_FACTOR_IMAX_OFFSET)) / DA7281_V2I_FACTOR_DIVISOR;

    /* Round to nearest integer and clamp to valid 16-bit range (1-65535) */
    uint16_t v2i_factor = (uint16_t)roundf(v2i_float);
    if (v2i_factor == 0) {
        v2i_factor = 1;  /* Minimum valid value */
        DA7281_LOG_WARNING("V2I_FACTOR calculated as 0, clamped to 1");
    }

    DA7281_LOG_DEBUG("V2I calculation: Z=%.2f ohm, IMAX_reg=%.2f, V2I=0x%04X (rounded from %.2f)",
                     config->impedance_ohm, imax_reg, v2i_factor, v2i_float);

    /* Write V2I factor (16-bit register, high byte first) */
    err = da7281_write_register(device, DA7281_REG_V2I_FACTOR_H,
                                  (uint8_t)(v2i_factor >> 8));
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to write V2I_FACTOR_H register");
        return err;
    }

    err = da7281_write_register(device, DA7281_REG_V2I_FACTOR_L,
                                  (uint8_t)(v2i_factor & 0xFF));
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to write V2I_FACTOR_L register");
        return err;
    }

    DA7281_LOG_INFO("V2I factor configured: %.2f ohm -> V2I=0x%04X",
                    config->impedance_ohm, v2i_factor);

    /* ===== 3. Configure Nominal Maximum Voltage ===== */
    /* This is the normal operating voltage (RMS) */
    uint8_t nommax = (uint8_t)((config->nom_max_v_rms * 1000.0F) /
                                DA7281_ACTUATOR_NOMMAX_SCALE);

    DA7281_LOG_DEBUG("NOMMAX calculation: V_rms=%.2fV, NOMMAX=0x%02X",
                     config->nom_max_v_rms, nommax);

    err = da7281_write_register(device, DA7281_REG_ACTUATOR_NOMMAX, nommax);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to write ACTUATOR_NOMMAX register");
        return err;
    }

    DA7281_LOG_INFO("Nominal max voltage: %.2f V RMS -> NOMMAX=0x%02X",
                    config->nom_max_v_rms, nommax);

    /* ===== 4. Configure Absolute Maximum Voltage ===== */
    /* This is the peak voltage limit for protection */
    uint8_t absmax = (uint8_t)((config->abs_max_v_peak * 1000.0F) /
                                DA7281_ACTUATOR_ABSMAX_SCALE);

    DA7281_LOG_DEBUG("ABSMAX calculation: V_peak=%.2fV, ABSMAX=0x%02X",
                     config->abs_max_v_peak, absmax);

    err = da7281_write_register(device, DA7281_REG_ACTUATOR_ABSMAX, absmax);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to write ACTUATOR_ABSMAX register");
        return err;
    }

    DA7281_LOG_INFO("Absolute max voltage: %.2f V peak -> ABSMAX=0x%02X",
                    config->abs_max_v_peak, absmax);

    /* ===== 5. Configure Maximum Current ===== */
    /* Current limit for actuator protection */
    /* DA7281 Datasheet: IMAX = (I_mA - 28.6) / 7.2 */
    float imax_float = ((float)config->max_current_ma - DA7281_ACTUATOR_IMAX_OFFSET) /
                       DA7281_ACTUATOR_IMAX_SCALE;
    uint8_t imax = (uint8_t)roundf(imax_float);
    if (imax_float < 0) {
        imax = 0;
        DA7281_LOG_WARNING("IMAX calculated as negative, clamped to 0");
    }

    DA7281_LOG_DEBUG("IMAX calculation: I=%umA, IMAX=0x%02X (rounded from %.2f)",
                     config->max_current_ma, imax, imax_float);

    err = da7281_write_register(device, DA7281_REG_ACTUATOR_IMAX, imax);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to write ACTUATOR_IMAX register");
        return err;
    }

    DA7281_LOG_INFO("Max current: %u mA -> IMAX=0x%02X",
                    config->max_current_ma, imax);

    DA7281_LOG_INFO("LRA configuration complete - all parameters programmed successfully");

    return DA7281_OK;
}

/**
 * @brief Set operation mode
 *
 * Changes the DA7281 operation mode. Available modes:
 * - INACTIVE (0): Chip powered but not generating haptics
 * - DRO (1): Direct Register Override - manual amplitude control
 * - PWM (2): External PWM input controls amplitude
 * - RTWM (3): Real-Time Waveform Memory playback
 * - ETWM (4): Embedded Waveform Memory playback
 * - STANDBY (5): Low power mode
 *
 * Note: Always return to INACTIVE mode before changing to a different mode.
 *
 * @param device Pointer to initialized device handle
 * @param mode Desired operation mode (0-5)
 * @return DA7281_OK on success
 * @return DA7281_ERROR_NULL_POINTER if device is NULL
 * @return DA7281_ERROR_NOT_INITIALIZED if device not initialized
 * @return DA7281_ERROR_INVALID_PARAM if mode out of range
 * @return DA7281_ERROR_I2C_WRITE on communication failure
 */
da7281_error_t da7281_set_operation_mode(da7281_device_t *device,
                                          da7281_operation_mode_t mode)
{
    DA7281_CHECK_DEVICE(device);
    DA7281_CHECK_RANGE(mode, DA7281_MODE_INACTIVE, DA7281_MODE_STANDBY);

    const char *mode_names[] = {
        "INACTIVE", "DRO", "PWM", "RTWM", "ETWM", "STANDBY"
    };

    DA7281_LOG_DEBUG("Changing operation mode from %s to %s",
                     mode_names[device->mode], mode_names[mode]);

    /* OP_MODE is bits [2:0] of TOP_CTL1 */
    uint8_t mode_value = ((uint8_t)mode << DA7281_TOP_CTL1_OP_MODE_SHIFT) & DA7281_TOP_CTL1_OP_MODE_MASK;

    da7281_error_t err = da7281_modify_register(device,
                                                  DA7281_REG_TOP_CTL1,
                                                  DA7281_TOP_CTL1_OP_MODE_MASK,
                                                  mode_value);
    if (err != DA7281_OK) {
        DA7281_LOG_ERROR("Failed to set operation mode to %s", mode_names[mode]);
        return err;
    }

    /* Verify mode was set correctly */
    uint8_t top_ctl1 = 0;
    err = da7281_read_register(device, DA7281_REG_TOP_CTL1, &top_ctl1);
    if (err == DA7281_OK) {
        uint8_t actual_mode = (top_ctl1 & DA7281_TOP_CTL1_OP_MODE_MASK) >> DA7281_TOP_CTL1_OP_MODE_SHIFT;
        if (actual_mode != (uint8_t)mode) {
            DA7281_LOG_WARNING("Operation mode verification failed: expected %d, got %d",
                               mode, actual_mode);
        }
    }

    device->mode = mode;

    DA7281_LOG_INFO("Operation mode set to: %s (%d)", mode_names[mode], mode);

    return DA7281_OK;
}

/**
 * @brief Get current operation mode
 */
da7281_error_t da7281_get_operation_mode(da7281_device_t *device,
                                          da7281_operation_mode_t *mode)
{
    DA7281_CHECK_DEVICE(device);
    DA7281_CHECK_NULL(mode);

    uint8_t reg_value = 0;
    da7281_error_t err = da7281_read_register(device, DA7281_REG_TOP_CTL1,
                                                &reg_value);
    if (err != DA7281_OK) {
        return err;
    }

    *mode = (da7281_operation_mode_t)((reg_value & DA7281_TOP_CTL1_OP_MODE_MASK) >> DA7281_TOP_CTL1_OP_MODE_SHIFT);

    return DA7281_OK;
}

/**
 * @brief Set override amplitude
 *
 * Sets the amplitude for Direct Register Override (DRO) mode.
 * The device should be in DRO mode (set via da7281_set_operation_mode)
 * before calling this function.
 *
 * @param device Pointer to device handle
 * @param amplitude Amplitude value (0-255, 0=off, 255=max)
 * @return DA7281_OK on success, error code otherwise
 */
da7281_error_t da7281_set_override_amplitude(da7281_device_t *device,
                                               uint8_t amplitude)
{
    DA7281_CHECK_DEVICE(device);

    /* Write override value to TOP_CTL2 */
    da7281_error_t err = da7281_write_register(device, DA7281_REG_TOP_CTL2, amplitude);
    if (err != DA7281_OK) {
        return err;
    }

    DA7281_LOG_DEBUG("Override amplitude set to: %u", amplitude);

    return DA7281_OK;
}

/**
 * @brief Enable/disable amplifier
 */
da7281_error_t da7281_set_amplifier_enable(da7281_device_t *device,
                                             bool enable)
{
    DA7281_CHECK_DEVICE(device);

    uint8_t value = enable ? DA7281_TOP_CFG1_AMP_EN : 0;

    da7281_error_t err = da7281_modify_register(device,
                                                  DA7281_REG_TOP_CFG1,
                                                  DA7281_TOP_CFG1_AMP_EN,
                                                  value);
    if (err != DA7281_OK) {
        return err;
    }

    DA7281_LOG_INFO("Amplifier %s", enable ? "enabled" : "disabled");

    return DA7281_OK;
}

/**
 * @brief Read chip revision
 */
da7281_error_t da7281_read_chip_revision(da7281_device_t *device, uint8_t *chip_rev)
{
    DA7281_CHECK_NULL(device);
    DA7281_CHECK_NULL(chip_rev);

    return da7281_read_register(device, DA7281_REG_CHIP_REV, chip_rev);
}
