/**
 * @file test_da7281_init.c
 * @brief Unit tests for DA7281 initialization functions
 * @author A. R. Ansari
 * @date 2024-11-21
 */

#include "unity.h"
#include "da7281.h"
#include "mock_nrf_drv_twi.h"
#include "mock_nrf_gpio.h"
#include "mock_FreeRTOS.h"

/* ========================================================================
 * Test Fixtures
 * ======================================================================== */

static da7281_device_t test_device;

void setUp(void)
{
    /* Initialize test device */
    test_device.twi_instance = 0;
    test_device.i2c_address = 0x4A;
    test_device.gpio_enable_pin = 12;
    test_device.initialized = false;
    test_device.powered_on = false;
    test_device.mode = DA7281_MODE_INACTIVE;
    test_device.twi_handle = NULL;
}

void tearDown(void)
{
    /* Cleanup after each test */
}

static void run_init_success_with_revision(uint8_t chip_rev_value)
{
    /* Reset device state because we test multiple revisions in one Unity test */
    test_device.twi_instance = 0;
    test_device.i2c_address = 0x4A;
    test_device.gpio_enable_pin = 12;
    test_device.initialized = false;
    test_device.powered_on = true;
    test_device.mode = DA7281_MODE_INACTIVE;
    test_device.twi_handle = NULL;

    /* Setup expectations for chip revision read */
    uint8_t chip_id_data[2] = {DA7281_REG_CHIP_REV, chip_rev_value};
    nrf_drv_twi_tx_ExpectAndReturn(&mock_twi_instance, 0x4A,
                                     &chip_id_data[0], 1, true, NRF_SUCCESS);
    nrf_drv_twi_rx_ExpectAndReturn(&mock_twi_instance, 0x4A,
                                     &chip_id_data[1], 1, NRF_SUCCESS);

    /* Execute */
    da7281_error_t err = da7281_init(&test_device);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_OK, err);
    TEST_ASSERT_TRUE(test_device.initialized);
    TEST_ASSERT_EQUAL(DA7281_MODE_INACTIVE, test_device.mode);
}

/* ========================================================================
 * Power Control Tests
 * ======================================================================== */

void test_da7281_power_on_success(void)
{
    /* Setup expectations */
    nrf_gpio_cfg_output_Expect(12);
    nrf_gpio_pin_set_Expect(12);
    vTaskDelay_Expect(pdMS_TO_TICKS(2));

    /* Execute */
    da7281_error_t err = da7281_power_on(&test_device);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_OK, err);
    TEST_ASSERT_TRUE(test_device.powered_on);
}

void test_da7281_power_on_null_pointer(void)
{
    /* Execute */
    da7281_error_t err = da7281_power_on(NULL);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_NULL_POINTER, err);
}

void test_da7281_power_on_already_powered(void)
{
    /* Setup */
    test_device.powered_on = true;

    /* Execute */
    da7281_error_t err = da7281_power_on(&test_device);

    /* Verify - should succeed but not reconfigure GPIO */
    TEST_ASSERT_EQUAL(DA7281_OK, err);
}

void test_da7281_power_off_success(void)
{
    /* Setup */
    test_device.powered_on = true;
    test_device.initialized = true;

    /* Setup expectations */
    nrf_gpio_pin_clear_Expect(12);

    /* Execute */
    da7281_error_t err = da7281_power_off(&test_device);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_OK, err);
    TEST_ASSERT_FALSE(test_device.powered_on);
    TEST_ASSERT_FALSE(test_device.initialized);
}

/* ========================================================================
 * Initialization Tests
 * ======================================================================== */

void test_da7281_init_success(void)
{
    /* Validate both current and legacy revisions succeed */
    run_init_success_with_revision(DA7281_CHIP_REV_VALUE);
    run_init_success_with_revision(DA7281_CHIP_REV_LEGACY_VALUE);
}

void test_da7281_init_null_pointer(void)
{
    /* Execute */
    da7281_error_t err = da7281_init(NULL);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_NULL_POINTER, err);
}

void test_da7281_init_not_powered(void)
{
    /* Setup - device not powered */
    test_device.powered_on = false;

    /* Execute */
    da7281_error_t err = da7281_init(&test_device);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_NOT_INITIALIZED, err);
}

void test_da7281_init_already_initialized(void)
{
    /* Setup */
    test_device.initialized = true;

    /* Execute */
    da7281_error_t err = da7281_init(&test_device);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_ALREADY_INITIALIZED, err);
}

void test_da7281_init_chip_rev_mismatch(void)
{
    /* Setup */
    test_device.powered_on = true;

    /* Setup expectations - wrong chip revision */
    uint8_t chip_id_data[2] = {DA7281_REG_CHIP_REV, 0xFF};  /* Wrong revision */
    nrf_drv_twi_tx_ExpectAndReturn(&mock_twi_instance, 0x4A,
                                     &chip_id_data[0], 1, true, NRF_SUCCESS);
    nrf_drv_twi_rx_ExpectAndReturn(&mock_twi_instance, 0x4A,
                                     &chip_id_data[1], 1, NRF_SUCCESS);

    /* Execute */
    da7281_error_t err = da7281_init(&test_device);

    /* Verify */
    TEST_ASSERT_EQUAL(DA7281_ERROR_CHIP_REV_MISMATCH, err);
    TEST_ASSERT_FALSE(test_device.initialized);
}

/* ========================================================================
 * Test Runner
 * ======================================================================== */

int main(void)
{
    UNITY_BEGIN();

    /* Power control tests */
    RUN_TEST(test_da7281_power_on_success);
    RUN_TEST(test_da7281_power_on_null_pointer);
    RUN_TEST(test_da7281_power_on_already_powered);
    RUN_TEST(test_da7281_power_off_success);

    /* Initialization tests */
    RUN_TEST(test_da7281_init_success);
    RUN_TEST(test_da7281_init_null_pointer);
    RUN_TEST(test_da7281_init_not_powered);
    RUN_TEST(test_da7281_init_already_initialized);
    RUN_TEST(test_da7281_init_chip_rev_mismatch);

    return UNITY_END();
}
