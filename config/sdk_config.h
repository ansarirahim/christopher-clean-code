/**
 * @file sdk_config.h
 * @brief Nordic SDK configuration for DA7281 HAL
 * 
 * Minimal configuration required for TWI/I2C and FreeRTOS integration
 */

#ifndef SDK_CONFIG_H
#define SDK_CONFIG_H

// <h> nRF_Drivers

//==========================================================
// <e> TWI_ENABLED - nrf_drv_twi - TWI/TWIM peripheral driver
//==========================================================
#ifndef TWI_ENABLED
#define TWI_ENABLED 1
#endif

// <e> TWI0_ENABLED - Enable TWI0 instance
#ifndef TWI0_ENABLED
#define TWI0_ENABLED 1
#endif

// <e> TWI1_ENABLED - Enable TWI1 instance
#ifndef TWI1_ENABLED
#define TWI1_ENABLED 1
#endif

// <o> TWI_DEFAULT_CONFIG_FREQUENCY - Frequency
// <26738688=> 100k
// <67108864=> 250k
// <104857600=> 400k
#ifndef TWI_DEFAULT_CONFIG_FREQUENCY
#define TWI_DEFAULT_CONFIG_FREQUENCY 26738688
#endif

// <o> TWI_DEFAULT_CONFIG_IRQ_PRIORITY - Interrupt priority
#ifndef TWI_DEFAULT_CONFIG_IRQ_PRIORITY
#define TWI_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <q> TWI_DEFAULT_CONFIG_CLR_BUS_INIT - Clear bus during init
#ifndef TWI_DEFAULT_CONFIG_CLR_BUS_INIT
#define TWI_DEFAULT_CONFIG_CLR_BUS_INIT 0
#endif

// </e>

//==========================================================
// <e> NRFX_TWI_ENABLED - nrfx_twi - TWI peripheral driver
//==========================================================
#ifndef NRFX_TWI_ENABLED
#define NRFX_TWI_ENABLED 1
#endif

// <e> NRFX_TWI0_ENABLED - Enable TWI0 instance
#ifndef NRFX_TWI0_ENABLED
#define NRFX_TWI0_ENABLED 1
#endif

// <e> NRFX_TWI1_ENABLED - Enable TWI1 instance
#ifndef NRFX_TWI1_ENABLED
#define NRFX_TWI1_ENABLED 1
#endif

// </e>

// </h>

//==========================================================
// <h> nRF_Libraries
//==========================================================

// <e> APP_TIMER_ENABLED - app_timer - Application timer functionality
#ifndef APP_TIMER_ENABLED
#define APP_TIMER_ENABLED 1
#endif

// <o> APP_TIMER_CONFIG_IRQ_PRIORITY - Interrupt priority
#ifndef APP_TIMER_CONFIG_IRQ_PRIORITY
#define APP_TIMER_CONFIG_IRQ_PRIORITY 6
#endif

// </e>

//==========================================================
// <h> nRF_Log
//==========================================================

// <e> NRF_LOG_ENABLED - nrf_log - Logger
#ifndef NRF_LOG_ENABLED
#define NRF_LOG_ENABLED 1
#endif

// <o> NRF_LOG_DEFAULT_LEVEL - Default Severity level
// <0=> Off
// <1=> Error
// <2=> Warning
// <3=> Info
// <4=> Debug
#ifndef NRF_LOG_DEFAULT_LEVEL
#define NRF_LOG_DEFAULT_LEVEL 3
#endif

// <e> NRF_LOG_BACKEND_RTT_ENABLED - nrf_log_backend_rtt - Log RTT backend
#ifndef NRF_LOG_BACKEND_RTT_ENABLED
#define NRF_LOG_BACKEND_RTT_ENABLED 1
#endif

// <e> NRF_LOG_BACKEND_UART_ENABLED - nrf_log_backend_uart - Log UART backend
#ifndef NRF_LOG_BACKEND_UART_ENABLED
#define NRF_LOG_BACKEND_UART_ENABLED 0
#endif

// </e>

// </h>

//==========================================================
// <h> FreeRTOS
//==========================================================

#ifndef FREERTOS
#define FREERTOS 1
#endif

// </h>

//==========================================================
// <h> Application
//==========================================================

// <o> DA7281_MAX_DEVICES - Maximum number of DA7281 devices
#ifndef DA7281_MAX_DEVICES
#define DA7281_MAX_DEVICES 4
#endif

// </h>

#endif // SDK_CONFIG_H

