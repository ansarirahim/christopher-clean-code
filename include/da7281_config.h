/**
 * @file da7281_config.h
 * @brief DA7281 HAL Configuration Options
 * @author A. R. Ansari
 * @date 2024-11-21
 */

#ifndef DA7281_CONFIG_H
#define DA7281_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Configuration Options
 * ======================================================================== */

/** Maximum number of DA7281 devices supported */
#ifndef DA7281_MAX_DEVICES
#define DA7281_MAX_DEVICES              (4U)
#endif

/** Enable debug logging (0=disabled, 1=enabled) */
#ifndef DA7281_ENABLE_DEBUG_LOG
#define DA7281_ENABLE_DEBUG_LOG         (1U)
#endif

/** Enable parameter validation (0=disabled, 1=enabled) */
#ifndef DA7281_ENABLE_PARAM_CHECK
#define DA7281_ENABLE_PARAM_CHECK       (1U)
#endif

/** I2C timeout in milliseconds */
#ifndef DA7281_I2C_TIMEOUT_MS
#define DA7281_I2C_TIMEOUT_MS           (100U)
#endif

/** Power-on delay in milliseconds (datasheet minimum: 1.5ms) */
#ifndef DA7281_POWER_ON_DELAY_MS
#define DA7281_POWER_ON_DELAY_MS        (2U)
#endif

/** Default I2C address (ADDR pin to GND) */
#ifndef DA7281_DEFAULT_I2C_ADDR
#define DA7281_DEFAULT_I2C_ADDR         (0x4AU)
#endif

/** Alternate I2C address (ADDR pin to VDD) */
#ifndef DA7281_ALTERNATE_I2C_ADDR
#define DA7281_ALTERNATE_I2C_ADDR       (0x4BU)
#endif

/** Enable FreeRTOS mutex protection (0=disabled, 1=enabled) */
#ifndef DA7281_ENABLE_FREERTOS_MUTEX
#define DA7281_ENABLE_FREERTOS_MUTEX    (1U)
#endif

/** FreeRTOS mutex timeout in ticks */
#ifndef DA7281_MUTEX_TIMEOUT_TICKS
#define DA7281_MUTEX_TIMEOUT_TICKS      (pdMS_TO_TICKS(100))
#endif

/* ========================================================================
 * Default LRA Configuration
 * ======================================================================== */

/** Default LRA resonant frequency in Hz */
#ifndef DA7281_DEFAULT_LRA_FREQ_HZ
#define DA7281_DEFAULT_LRA_FREQ_HZ      (170U)
#endif

/** Default LRA impedance in ohms */
#ifndef DA7281_DEFAULT_LRA_IMPEDANCE_OHM
#define DA7281_DEFAULT_LRA_IMPEDANCE_OHM (6.75F)
#endif

/** Default nominal max voltage in V RMS */
#ifndef DA7281_DEFAULT_NOMMAX_V_RMS
#define DA7281_DEFAULT_NOMMAX_V_RMS     (2.5F)
#endif

/** Default absolute max voltage in V peak */
#ifndef DA7281_DEFAULT_ABSMAX_V_PEAK
#define DA7281_DEFAULT_ABSMAX_V_PEAK    (3.5F)
#endif

/** Default max current in mA */
#ifndef DA7281_DEFAULT_IMAX_MA
#define DA7281_DEFAULT_IMAX_MA          (350U)
#endif

/* ========================================================================
 * Validation Macros
 * ======================================================================== */

#if DA7281_ENABLE_PARAM_CHECK

/** Validate pointer is not NULL */
#define DA7281_CHECK_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            return DA7281_ERROR_NULL_POINTER; \
        } \
    } while(0)

/** Validate device handle */
#define DA7281_CHECK_DEVICE(dev) \
    do { \
        if ((dev) == NULL) { \
            return DA7281_ERROR_NULL_POINTER; \
        } \
        if (!(dev)->initialized) { \
            return DA7281_ERROR_NOT_INITIALIZED; \
        } \
    } while(0)

/** Validate parameter range */
#define DA7281_CHECK_RANGE(val, min, max) \
    do { \
        if (((val) < (min)) || ((val) > (max))) { \
            return DA7281_ERROR_INVALID_PARAM; \
        } \
    } while(0)

#else

#define DA7281_CHECK_NULL(ptr)          ((void)0)
#define DA7281_CHECK_DEVICE(dev)        ((void)0)
#define DA7281_CHECK_RANGE(val, min, max) ((void)0)

#endif /* DA7281_ENABLE_PARAM_CHECK */

/* ========================================================================
 * Logging Macros
 * ======================================================================== */

#if DA7281_ENABLE_DEBUG_LOG

#include "nrf_log.h"

#define DA7281_LOG_ERROR(...)   NRF_LOG_ERROR(__VA_ARGS__)
#define DA7281_LOG_WARNING(...) NRF_LOG_WARNING(__VA_ARGS__)
#define DA7281_LOG_INFO(...)    NRF_LOG_INFO(__VA_ARGS__)
#define DA7281_LOG_DEBUG(...)   NRF_LOG_DEBUG(__VA_ARGS__)

#else

#define DA7281_LOG_ERROR(...)   ((void)0)
#define DA7281_LOG_WARNING(...) ((void)0)
#define DA7281_LOG_INFO(...)    ((void)0)
#define DA7281_LOG_DEBUG(...)   ((void)0)

#endif /* DA7281_ENABLE_DEBUG_LOG */

#ifdef __cplusplus
}
#endif

#endif /* DA7281_CONFIG_H */

