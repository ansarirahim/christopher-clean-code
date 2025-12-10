/**
 * @file da7281_registers.h
 * @brief DA7281 Register Definitions
 * @author A. R. Ansari
 * @date 2024-11-21
 * 
 * Complete register map and bit field definitions for Dialog DA7281
 * haptic driver IC. All definitions verified against datasheet v1.1.
 */

#ifndef DA7281_REGISTERS_H
#define DA7281_REGISTERS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Register Addresses (8-bit)
 * ======================================================================== */

/** Chip ID register (Read-only) - Expected value: 0xCA per datasheet Table 20 */
#define DA7281_REG_CHIP_ID              (0x00U)

/** Chip revision register (Read-only) */
#define DA7281_REG_CHIP_REV             (0x01U)

/** Top configuration register 1 */
#define DA7281_REG_TOP_CFG1             (0x13U)

/** Top configuration register 2 */
#define DA7281_REG_TOP_CFG2             (0x14U)

/** Top configuration register 3 */
#define DA7281_REG_TOP_CFG3             (0x15U)

/** Top configuration register 4 */
#define DA7281_REG_TOP_CFG4             (0x16U)

/** Top control register 1 */
#define DA7281_REG_TOP_CTL1             (0x22U)

/** Top control register 2 */
#define DA7281_REG_TOP_CTL2             (0x23U)

/** Top interrupt configuration register */
#define DA7281_REG_TOP_INT_CFG          (0x08U)

/** Top interrupt status register */
#define DA7281_REG_TOP_INT_STATUS       (0x09U)

/** Frequency tracking configuration register */
#define DA7281_REG_FREQ_TRACK_CFG       (0x0AU)

/** Actuator type register */
#define DA7281_REG_ACTUATOR_TYPE        (0x0BU)

/** Calib V2I factor register */
#define DA7281_REG_CALIB_V2I_H          (0x0CU)
#define DA7281_REG_CALIB_V2I_L          (0x0DU)

/** Top control register 3 */
#define DA7281_REG_TOP_CTL3             (0x0EU)

/** SNP memory registers (128 bytes: 0x10 - 0x8F) */
#define DA7281_REG_SNP_MEM_START        (0x10U)
#define DA7281_REG_SNP_MEM_END          (0x8FU)
#define DA7281_SNP_MEM_SIZE             (128U)

/** General configuration register 1 */
#define DA7281_REG_GEN_CFG1             (0x90U)

/** General configuration register 2 */
#define DA7281_REG_GEN_CFG2             (0x91U)

/** General configuration register 3 */
#define DA7281_REG_GEN_CFG3             (0x92U)

/** General configuration register 4 */
#define DA7281_REG_GEN_CFG4             (0x93U)

/** General configuration register 5 */
#define DA7281_REG_GEN_CFG5             (0x94U)

/** General configuration register 6 */
#define DA7281_REG_GEN_CFG6             (0x95U)

/** LRA resonance period register (high byte) */
#define DA7281_REG_LRA_PER_H            (0x96U)

/** LRA resonance period register (low byte) */
#define DA7281_REG_LRA_PER_L            (0x97U)

/** Voltage to current factor register (high byte) */
#define DA7281_REG_V2I_FACTOR_H         (0x98U)

/** Voltage to current factor register (low byte) */
#define DA7281_REG_V2I_FACTOR_L         (0x99U)

/** Frequency tracking configuration register 2 */
#define DA7281_REG_FREQ_TRACK_CFG2      (0x9AU)

/** Actuator nominal max voltage register */
#define DA7281_REG_ACTUATOR_NOMMAX      (0x9BU)

/** Actuator absolute max voltage register */
#define DA7281_REG_ACTUATOR_ABSMAX      (0x9CU)

/** Actuator max current register */
#define DA7281_REG_ACTUATOR_IMAX        (0x9DU)

/** Boost configuration register */
#define DA7281_REG_BOOST_CFG            (0x9EU)

/** Boost option register */
#define DA7281_REG_BOOST_OPTION         (0x9FU)

/** Sequencer configuration register 1 */
#define DA7281_REG_SEQ_CFG1             (0xA0U)

/** Sequencer configuration register 2 */
#define DA7281_REG_SEQ_CFG2             (0xA1U)

/** Sequencer control register */
#define DA7281_REG_SEQ_CTL              (0xA2U)

/** GPIO configuration register 1 */
#define DA7281_REG_GPIO_CFG1            (0xA3U)

/** GPIO configuration register 2 */
#define DA7281_REG_GPIO_CFG2            (0xA4U)

/** GPIO configuration register 3 */
#define DA7281_REG_GPIO_CFG3            (0xA5U)

/** Polarity configuration register */
#define DA7281_REG_POLARITY_CFG         (0xA6U)

/** LRA auto-resonance configuration register 1 */
#define DA7281_REG_LRA_AUTO_RES_CFG1    (0xA7U)

/** LRA auto-resonance configuration register 2 */
#define DA7281_REG_LRA_AUTO_RES_CFG2    (0xA8U)

/** Override amplitude register */
#define DA7281_REG_OVERRIDE_AMP         (0xA9U)

/** Self-test configuration register */
#define DA7281_REG_SELFTEST_CFG         (0xAAU)

/** Self-test result register (Read-only) */
#define DA7281_REG_SELFTEST_RESULT      (0xABU)

/* ========================================================================
 * Register Bit Field Definitions
 * ======================================================================== */

/* TOP_CFG1 (0x02) - Operation Mode Control */
#define DA7281_TOP_CFG1_OP_MODE_MASK    (0x07U)
#define DA7281_TOP_CFG1_OP_MODE_SHIFT   (0U)
#define DA7281_OP_MODE_INACTIVE         (0x00U)
#define DA7281_OP_MODE_DRO              (0x01U)  /**< Direct Register Override */
#define DA7281_OP_MODE_PWM              (0x02U)  /**< PWM Input Mode */
#define DA7281_OP_MODE_RTWM             (0x03U)  /**< Real-Time Waveform Mode */
#define DA7281_OP_MODE_ETWM             (0x04U)  /**< Embedded Waveform Mode */
#define DA7281_OP_MODE_STANDBY          (0x06U)  /**< Standby Mode */

/* TOP_CFG1 - Amplifier Enable */
#define DA7281_TOP_CFG1_AMP_EN          (0x08U)

/* TOP_CFG1 - Frequency Tracking Enable */
#define DA7281_TOP_CFG1_FREQ_TRACK_EN   (0x10U)

/* TOP_CFG1 - Acceleration Enable */
#define DA7281_TOP_CFG1_ACCEL_EN        (0x20U)

/* TOP_CFG1 - Rapid Stop Enable */
#define DA7281_TOP_CFG1_RAPID_STOP_EN   (0x40U)

/* TOP_CFG1 - Amplitude Register Update */
#define DA7281_TOP_CFG1_AMP_REG_UPDATE  (0x80U)

/* TOP_CFG2 (0x03) - Motor Type */
#define DA7281_TOP_CFG2_MOTOR_TYPE_MASK (0x03U)
#define DA7281_MOTOR_TYPE_LRA           (0x00U)  /**< Linear Resonant Actuator */
#define DA7281_MOTOR_TYPE_ERM_BAR       (0x01U)  /**< ERM with back-EMF */
#define DA7281_MOTOR_TYPE_ERM_COIN      (0x02U)  /**< ERM coin type */

/* TOP_CTL1 (0x06) - Sequencer Start */
#define DA7281_TOP_CTL1_SEQ_START       (0x01U)

/* TOP_CTL1 - Sequencer Continue */
#define DA7281_TOP_CTL1_SEQ_CONTINUE    (0x02U)

/* GEN_CFG2 (0x91) - Override Enable */
#define DA7281_GEN_CFG2_OVERRIDE_EN     (0x01U)

/* ACTUATOR_NOMMAX (0x9B) - Voltage scaling factor */
#define DA7281_ACTUATOR_NOMMAX_SCALE    (23.4375F)  /**< mV per LSB */

/* ACTUATOR_ABSMAX (0x9C) - Voltage scaling factor */
#define DA7281_ACTUATOR_ABSMAX_SCALE    (48.75F)    /**< mV per LSB */

/* ACTUATOR_IMAX (0x9D) - Current scaling factor */
#define DA7281_ACTUATOR_IMAX_SCALE      (7.8125F)   /**< mA per LSB */

/* LRA_PER - Period calculation constant */
#define DA7281_LRA_PER_TIME_SCALE       (1.024E-6F) /**< seconds per LSB */

/* V2I_FACTOR - Scaling constant */
#define DA7281_V2I_FACTOR_SCALE         (1.5F)      /**< Multiplier */

/* Expected CHIP_ID value (DA7281 Datasheet v3.1, Table 20) */
#define DA7281_CHIP_ID_VALUE            (0xCAU)

/* Self-test result bits */
#define DA7281_SELFTEST_RESULT_PASS     (0x01U)
#define DA7281_SELFTEST_RESULT_FAIL     (0x00U)

#ifdef __cplusplus
}
#endif

#endif /* DA7281_REGISTERS_H */

