/**
 * @file haptics_demo.c
 * @brief DA7281 HAL Usage Example
 * @author A. R. Ansari
 * @date 2024-11-21
 * 
 * Demonstrates basic usage of DA7281 HAL including:
 * - Device initialization
 * - LRA configuration
 * - Self-test
 * - Override mode control
 * - Multiple amplitude levels
 */

#include "da7281.h"
#include "FreeRTOS.h"
#include "task.h"
#include "nrf_log.h"

/* ========================================================================
 * Configuration
 * ======================================================================== */

/** Device configuration for first DA7281 */
static da7281_device_t s_haptic_device_1 = {
    .twi_instance = 0,           /* TWI0 */
    .i2c_address = 0x4A,         /* ADDR_1=VDDIO, ADDR_0=GND */
    .gpio_enable_pin = 12,       /* GPIO pin for power control */
    .initialized = false,
    .powered_on = false,
    .mode = DA7281_MODE_INACTIVE,
    .twi_handle = NULL
};

/** LRA configuration (170Hz, 6.75Ω) */
static const da7281_lra_config_t s_lra_config = {
    .resonant_freq_hz = 170,
    .impedance_ohm = 6.75F,
    .nom_max_v_rms = 2.5F,
    .abs_max_v_peak = 3.5F,
    .max_current_ma = 350
};

/* ========================================================================
 * Private Functions
 * ======================================================================== */

/**
 * @brief Play haptic effect at specified amplitude
 */
static void play_haptic_pulse(da7281_device_t *device, 
                                uint8_t amplitude, 
                                uint32_t duration_ms)
{
    NRF_LOG_INFO("Playing haptic pulse: amplitude=%u, duration=%ums",
                 amplitude, duration_ms);

    /* Set amplitude */
    da7281_set_override_amplitude(device, amplitude);

    /* Wait for duration */
    vTaskDelay(pdMS_TO_TICKS(duration_ms));

    /* Stop */
    da7281_set_override_amplitude(device, 0);
}

/**
 * @brief Run haptic demo sequence
 */
static void run_haptic_demo(da7281_device_t *device)
{
    NRF_LOG_INFO("=== Starting Haptic Demo ===");

    /* Test sequence: increasing amplitude */
    NRF_LOG_INFO("Test 1: Increasing amplitude (25%%, 50%%, 75%%, 100%%)");
    
    play_haptic_pulse(device, 64, 200);   /* 25% amplitude */
    vTaskDelay(pdMS_TO_TICKS(300));
    
    play_haptic_pulse(device, 128, 200);  /* 50% amplitude */
    vTaskDelay(pdMS_TO_TICKS(300));
    
    play_haptic_pulse(device, 192, 200);  /* 75% amplitude */
    vTaskDelay(pdMS_TO_TICKS(300));
    
    play_haptic_pulse(device, 255, 200);  /* 100% amplitude */
    vTaskDelay(pdMS_TO_TICKS(500));

    /* Test sequence: short pulses */
    NRF_LOG_INFO("Test 2: Short pulses (3x 50ms)");
    
    for (int i = 0; i < 3; i++) {
        play_haptic_pulse(device, 200, 50);
        vTaskDelay(pdMS_TO_TICKS(150));
    }
    
    vTaskDelay(pdMS_TO_TICKS(500));

    /* Test sequence: long pulse */
    NRF_LOG_INFO("Test 3: Long pulse (500ms)");
    play_haptic_pulse(device, 180, 500);

    NRF_LOG_INFO("=== Haptic Demo Complete ===");
}

/* ========================================================================
 * FreeRTOS Task
 * ======================================================================== */

/**
 * @brief Haptics demo task
 */
void haptics_demo_task(void *pvParameters)
{
    (void)pvParameters;

    NRF_LOG_INFO("Haptics Demo Task Started");

    da7281_error_t err;

    /* Power on device */
    NRF_LOG_INFO("Powering on DA7281...");
    err = da7281_power_on(&s_haptic_device_1);
    if (err != DA7281_OK) {
        NRF_LOG_ERROR("Failed to power on device: %d", err);
        goto error;
    }

    /* Initialize device */
    NRF_LOG_INFO("Initializing DA7281...");
    err = da7281_init(&s_haptic_device_1);
    if (err != DA7281_OK) {
        NRF_LOG_ERROR("Failed to initialize device: %d", err);
        goto error;
    }

    /* Configure LRA */
    NRF_LOG_INFO("Configuring LRA (170Hz, 6.75Ω)...");
    err = da7281_configure_lra(&s_haptic_device_1, &s_lra_config);
    if (err != DA7281_OK) {
        NRF_LOG_ERROR("Failed to configure LRA: %d", err);
        goto error;
    }

    /* Set to DRO mode */
    NRF_LOG_INFO("Setting operation mode to DRO...");
    err = da7281_set_operation_mode(&s_haptic_device_1, DA7281_MODE_DRO);
    if (err != DA7281_OK) {
        NRF_LOG_ERROR("Failed to set operation mode: %d", err);
        goto error;
    }

    /* Enable amplifier */
    NRF_LOG_INFO("Enabling amplifier...");
    err = da7281_set_amplifier_enable(&s_haptic_device_1, true);
    if (err != DA7281_OK) {
        NRF_LOG_ERROR("Failed to enable amplifier: %d", err);
        goto error;
    }

    /* Run demo sequence */
    run_haptic_demo(&s_haptic_device_1);

    /* Cleanup */
    NRF_LOG_INFO("Cleaning up...");
    da7281_set_amplifier_enable(&s_haptic_device_1, false);
    da7281_deinit(&s_haptic_device_1);
    da7281_power_off(&s_haptic_device_1);

    NRF_LOG_INFO("Demo complete - task exiting");

    /* Task complete */
    vTaskDelete(NULL);
    return;

error:
    NRF_LOG_ERROR("Demo failed - cleaning up");
    da7281_power_off(&s_haptic_device_1);
    vTaskDelete(NULL);
}
