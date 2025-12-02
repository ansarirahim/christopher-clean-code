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

/**
 * @brief Enable GPIO-controlled power/enable
 *
 * Set to 0 if the device is always powered and no GPIO enable is present.
 */
#ifndef DA7281_ENABLE_GPIO_POWER
#define DA7281_ENABLE_GPIO_POWER        (1U)
#endif

/** I2C address: ADDR_1=GND, ADDR_0=GND (datasheet Table 16, p58) */
#ifndef DA7281_I2C_ADDR_0x48
#define DA7281_I2C_ADDR_0x48            (0x48U)
#endif

/** I2C address: ADDR_1=GND, ADDR_0=VDDIO (datasheet Table 16, p58) */
#ifndef DA7281_I2C_ADDR_0x49
#define DA7281_I2C_ADDR_0x49            (0x49U)
#endif

/** I2C address: ADDR_1=VDDIO, ADDR_0=GND (datasheet Table 16, p58) */
#ifndef DA7281_I2C_ADDR_0x4A
#define DA7281_I2C_ADDR_0x4A            (0x4AU)
#endif

/** I2C address: ADDR_1=VDDIO, ADDR_0=VDDIO (datasheet Table 16, p58) */
#ifndef DA7281_I2C_ADDR_0x4B
#define DA7281_I2C_ADDR_0x4B            (0x4BU)
#endif

/** Default I2C address (for backward compatibility) */
#ifndef DA7281_DEFAULT_I2C_ADDR
#define DA7281_DEFAULT_I2C_ADDR         DA7281_I2C_ADDR_0x4A
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
 * Logging Backend Configuration
 * ========================================================================
 *
 * DA7281_LOG_BACKEND options:
 *   0 = Disabled (no logging)
 *   1 = NRF_LOG (Nordic SDK - RTT or UART via sdk_config.h)
 *   2 = QORVO_SDK (QLOGE/QLOGW/QLOGI/QLOGD macros)
 *   3 = SEGGER_RTT (Direct SEGGER RTT, no Nordic wrapper)
 *   4 = UART_PRINTF (Direct printf to UART)
 *   5 = CUSTOM (User provides DA7281_LOG_* macros before including this header)
 *
 * For Qorvo DWM3001CDK with JLinkRTTViewer:
 *   - Use DA7281_LOG_BACKEND=2 to integrate with QLOGE (recommended)
 *   - Or use DA7281_LOG_BACKEND=3 for direct SEGGER RTT
 *
 * For UART over minicom:
 *   - Use DA7281_LOG_BACKEND=1 with NRF_LOG_BACKEND_UART_ENABLED=1
 *   - Or use DA7281_LOG_BACKEND=4 for direct printf
 * ======================================================================== */

#ifndef DA7281_LOG_BACKEND
#define DA7281_LOG_BACKEND              (1U)  /* Default: NRF_LOG */
#endif

/* ========================================================================
 * Logging Macros
 * ======================================================================== */

#if (DA7281_LOG_BACKEND == 0) || !DA7281_ENABLE_DEBUG_LOG

/* Logging disabled */
#define DA7281_LOG_ERROR(...)   ((void)0)
#define DA7281_LOG_WARNING(...) ((void)0)
#define DA7281_LOG_INFO(...)    ((void)0)
#define DA7281_LOG_DEBUG(...)   ((void)0)

#elif (DA7281_LOG_BACKEND == 1)

/* Nordic SDK NRF_LOG (configurable via sdk_config.h for RTT or UART) */
#include "nrf_log.h"

#define DA7281_LOG_ERROR(...)   NRF_LOG_ERROR(__VA_ARGS__)
#define DA7281_LOG_WARNING(...) NRF_LOG_WARNING(__VA_ARGS__)
#define DA7281_LOG_INFO(...)    NRF_LOG_INFO(__VA_ARGS__)
#define DA7281_LOG_DEBUG(...)   NRF_LOG_DEBUG(__VA_ARGS__)

#elif (DA7281_LOG_BACKEND == 2)

/* Qorvo SDK QLOG macros (for use with JLinkRTTViewer) */
#include "qlog.h"

#define DA7281_LOG_ERROR(...)   QLOGE(__VA_ARGS__)
#define DA7281_LOG_WARNING(...) QLOGW(__VA_ARGS__)
#define DA7281_LOG_INFO(...)    QLOGI(__VA_ARGS__)
#define DA7281_LOG_DEBUG(...)   QLOGD(__VA_ARGS__)

#elif (DA7281_LOG_BACKEND == 3)

/* Direct SEGGER RTT (bypasses Nordic SDK wrapper) */
#include "SEGGER_RTT.h"

#define DA7281_LOG_ERROR(...)   SEGGER_RTT_printf(0, "[ERR] DA7281: " __VA_ARGS__); SEGGER_RTT_printf(0, "\n")
#define DA7281_LOG_WARNING(...) SEGGER_RTT_printf(0, "[WRN] DA7281: " __VA_ARGS__); SEGGER_RTT_printf(0, "\n")
#define DA7281_LOG_INFO(...)    SEGGER_RTT_printf(0, "[INF] DA7281: " __VA_ARGS__); SEGGER_RTT_printf(0, "\n")
#define DA7281_LOG_DEBUG(...)   SEGGER_RTT_printf(0, "[DBG] DA7281: " __VA_ARGS__); SEGGER_RTT_printf(0, "\n")

#elif (DA7281_LOG_BACKEND == 4)

/* Direct UART printf (requires stdio configured for UART) */
#include <stdio.h>

#define DA7281_LOG_ERROR(...)   do { printf("[ERR] DA7281: "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define DA7281_LOG_WARNING(...) do { printf("[WRN] DA7281: "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define DA7281_LOG_INFO(...)    do { printf("[INF] DA7281: "); printf(__VA_ARGS__); printf("\n"); } while(0)
#define DA7281_LOG_DEBUG(...)   do { printf("[DBG] DA7281: "); printf(__VA_ARGS__); printf("\n"); } while(0)

#elif (DA7281_LOG_BACKEND == 5)

/* Custom logging - user must define DA7281_LOG_* macros before including this header */
#ifndef DA7281_LOG_ERROR
#error "DA7281_LOG_BACKEND=5 (CUSTOM) requires DA7281_LOG_ERROR to be defined"
#endif
#ifndef DA7281_LOG_WARNING
#error "DA7281_LOG_BACKEND=5 (CUSTOM) requires DA7281_LOG_WARNING to be defined"
#endif
#ifndef DA7281_LOG_INFO
#error "DA7281_LOG_BACKEND=5 (CUSTOM) requires DA7281_LOG_INFO to be defined"
#endif
#ifndef DA7281_LOG_DEBUG
#error "DA7281_LOG_BACKEND=5 (CUSTOM) requires DA7281_LOG_DEBUG to be defined"
#endif

#else
#error "Invalid DA7281_LOG_BACKEND value. Must be 0-5."
#endif /* DA7281_LOG_BACKEND */

#ifdef __cplusplus
}
#endif

#endif /* DA7281_CONFIG_H */
