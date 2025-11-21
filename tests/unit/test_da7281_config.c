/**
 * @file test_da7281_config.c
 * @brief Unit tests for DA7281 configuration functions
 * @author A. R. Ansari
 * @date 2024-11-21
 */

#include "unity.h"
#include "da7281.h"
#include "mock_nrf_drv_twi.h"

/* ========================================================================
 * Test Fixtures
 * ======================================================================== */

static da7281_device_t test_device;
static da7281_lra_config_t test_lra_config;

void setUp(void)
{
    /* Initialize test device */
    test_device.twi_instance = 0;
    test_device.i2c_address = 0x4A;
    test_device.gpio_enable_pin = 12;
    test_device.initialized = true;  /* Device must be initialized */
    test_device.powered_on = true;
    test_device.mode = DA7281_MODE_INACTIVE;
    test_device.twi_handle = NULL;

    /* Initialize test LRA config */
    test_lra_config.resonant_freq_hz = 170;
    test_lra_config.impedance_ohm = 6.75F;
    test_lra_config.nom_max_v_rms = 2.5F;
    test_lra_config.abs_max_v_peak = 3.5F;
    test_lra_config.max_current_ma = 350;
}

void tearDown(void)
{
    /* Cleanup */
}

/* ========================================================================
 * LRA Configuration Tests
 * ======================================================================== */

void test_da7281_configure_lra_success(void)
{
    /* Setup expectations for register writes */
    /* LRA_PER_H, LRA_PER_L, V2I_FACTOR_H, V2I_FACTOR_L, 
       ACTUATOR_NOMMAX, ACTUATOR_ABSMAX, ACTUATOR_IMAX */

    /* Execute */
    da7281_error_t err = da7281_configure_lra(&test_device, &test_lra_config);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_OK, err);
}

void test_da7281_configure_lra_null_device(void)
{
    /* Execute */
    da7281_error_t err = da7281_configure_lra(NULL, &test_lra_config);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_NULL_POINTER, err);
}

void test_da7281_configure_lra_null_config(void)
{
    /* Execute */
    da7281_error_t err = da7281_configure_lra(&test_device, NULL);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_NULL_POINTER, err);
}

void test_da7281_configure_lra_invalid_frequency(void)
{
    /* Setup - invalid frequency */
    test_lra_config.resonant_freq_hz = 500;  /* Out of range */

    /* Execute */
    da7281_error_t err = da7281_configure_lra(&test_device, &test_lra_config);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_INVALID_PARAM, err);
}

void test_da7281_configure_lra_invalid_impedance(void)
{
    /* Setup - invalid impedance */
    test_lra_config.impedance_ohm = 100.0F;  /* Out of range */

    /* Execute */
    da7281_error_t err = da7281_configure_lra(&test_device, &test_lra_config);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_INVALID_PARAM, err);
}

/* ========================================================================
 * Operation Mode Tests
 * ======================================================================== */

void test_da7281_set_operation_mode_success(void)
{
    /* Setup expectations for register modify */
    /* ... */

    /* Execute */
    da7281_error_t err = da7281_set_operation_mode(&test_device, 
                                                     DA7281_MODE_DRO);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_OK, err);
    TEST_ASSERT_EQUAL(DA7281_MODE_DRO, test_device.mode);
}

void test_da7281_set_operation_mode_null_device(void)
{
    /* Execute */
    da7281_error_t err = da7281_set_operation_mode(NULL, DA7281_MODE_DRO);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_NULL_POINTER, err);
}

void test_da7281_set_operation_mode_invalid_mode(void)
{
    /* Execute - invalid mode value */
    da7281_error_t err = da7281_set_operation_mode(&test_device, 0xFF);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_INVALID_PARAM, err);
}

void test_da7281_get_operation_mode_success(void)
{
    /* Setup expectations for register read */
    /* ... */

    da7281_operation_mode_t mode;

    /* Execute */
    da7281_error_t err = da7281_get_operation_mode(&test_device, &mode);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_OK, err);
}

/* ========================================================================
 * Override Amplitude Tests
 * ======================================================================== */

void test_da7281_set_override_amplitude_success(void)
{
    /* Setup expectations */
    /* ... */

    /* Execute */
    da7281_error_t err = da7281_set_override_amplitude(&test_device, 128);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_OK, err);
}

void test_da7281_set_override_amplitude_null_device(void)
{
    /* Execute */
    da7281_error_t err = da7281_set_override_amplitude(NULL, 128);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_NULL_POINTER, err);
}

void test_da7281_set_override_amplitude_boundary_values(void)
{
    /* Test minimum value (0) */
    da7281_error_t err = da7281_set_override_amplitude(&test_device, 0);
    TEST_ASSERT_EQUAL(DA7281_OK, err);

    /* Test maximum value (255) */
    err = da7281_set_override_amplitude(&test_device, 255);
    TEST_ASSERT_EQUAL(DA7281_OK, err);
}

/* ========================================================================
 * Test Runner
 * ======================================================================== */

int main(void)
{
    UNITY_BEGIN();

    /* LRA configuration tests */
    RUN_TEST(test_da7281_configure_lra_success);
    RUN_TEST(test_da7281_configure_lra_null_device);
    RUN_TEST(test_da7281_configure_lra_null_config);
    RUN_TEST(test_da7281_configure_lra_invalid_frequency);
    RUN_TEST(test_da7281_configure_lra_invalid_impedance);

    /* Operation mode tests */
    RUN_TEST(test_da7281_set_operation_mode_success);
    RUN_TEST(test_da7281_set_operation_mode_null_device);
    RUN_TEST(test_da7281_set_operation_mode_invalid_mode);
    RUN_TEST(test_da7281_get_operation_mode_success);

    /* Override amplitude tests */
    RUN_TEST(test_da7281_set_override_amplitude_success);
    RUN_TEST(test_da7281_set_override_amplitude_null_device);
    RUN_TEST(test_da7281_set_override_amplitude_boundary_values);

    return UNITY_END();
}

