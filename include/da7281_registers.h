/**
 * @file da7281_registers.h
 * @brief DA7281 Register Definitions
 * @author A. R. Ansari
 * @date 2025-12-10
 * 
 * Complete register map and bit field definitions for Dialog DA7281
 * haptic driver IC. All definitions verified against datasheet v3.1.
 */

#ifndef DA7281_REGISTERS_H
#define DA7281_REGISTERS_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================
 * Register Addresses (8-bit)
 * ======================================================================== */

/** Chip revision register (Read-only, datasheet Table 20, expected 0xCA/legacy 0xBA) */
#define DA7281_REG_CHIP_REV             (0x00U)

/** Top configuration register 1 */
#define DA7281_REG_TOP_CFG1             (0x13U)

/** Top configuration register 2 */
#define DA7281_REG_TOP_CFG2             (0x14U)

/** Top configuration register 3 */
#define DA7281_REG_TOP_CFG3             (0x15U)

/** Top configuration register 4 */
#define DA7281_REG_TOP_CFG4             (0x16U)

/** Top control register 1 (operation mode bits [2:0]) */
#define DA7281_REG_TOP_CTL1             (0x22U)

/** Top control register 2 */
#define DA7281_REG_TOP_CTL2             (0x23U)

/** IRQ event register 1 (write 1 to clear fault bits) */
#define DA7281_REG_IRQ_EVENT1           (0x03U)

/** IRQ status register 1 (read to detect faults) */
#define DA7281_REG_IRQ_STATUS1          (0x05U)

/** LRA resonance period register (high byte) */
#define DA7281_REG_LRA_PER_H            (0x07U)

/** LRA resonance period register (low byte) */
#define DA7281_REG_LRA_PER_L            (0x08U)

/** Voltage to current factor register (high byte) */
#define DA7281_REG_V2I_FACTOR_H         (0x09U)

/** Voltage to current factor register (low byte) */
#define DA7281_REG_V2I_FACTOR_L         (0x0AU)

/** Actuator nominal max voltage register */
#define DA7281_REG_ACTUATOR_NOMMAX      (0x0BU)

/** Actuator absolute max voltage register */
#define DA7281_REG_ACTUATOR_ABSMAX      (0x0CU)

/** Actuator max current register */
#define DA7281_REG_ACTUATOR_IMAX        (0x0DU)

/* ========================================================================
 * Register Bit Field Definitions
 * ======================================================================== */

/* TOP_CTL1 (0x22) - Operation Mode Control */
#define DA7281_TOP_CTL1_OP_MODE_MASK    (0x07U)
#define DA7281_TOP_CTL1_OP_MODE_SHIFT   (0U)
#define DA7281_OP_MODE_INACTIVE         (0x00U)
#define DA7281_OP_MODE_DRO              (0x01U)  /**< Direct Register Override */
#define DA7281_OP_MODE_PWM              (0x02U)  /**< PWM Input Mode */
#define DA7281_OP_MODE_RTWM             (0x03U)  /**< Real-Time Waveform Mode */
#define DA7281_OP_MODE_ETWM             (0x04U)  /**< Embedded Waveform Mode */
#define DA7281_OP_MODE_STANDBY          (0x06U)  /**< Standby Mode */

/* TOP_CFG1 - Actuator Type (bit 5) */
#define DA7281_TOP_CFG1_ACTUATOR_TYPE   (0x20U)  /**< Bit 5: 0=ERM, 1=LRA */
#define DA7281_ACTUATOR_TYPE_ERM        (0x00U)  /**< ERM actuator */
#define DA7281_ACTUATOR_TYPE_LRA        (0x20U)  /**< LRA actuator (bit 5 = 1) */

/* TOP_CFG1 - Amplifier Enable */
#define DA7281_TOP_CFG1_AMP_EN          (0x08U)

/* TOP_CFG1 - Acceleration Enable */
#define DA7281_TOP_CFG1_ACCEL_EN        (0x04U)

/* TOP_CFG1 - Rapid Stop Enable */
#define DA7281_TOP_CFG1_RAPID_STOP_EN   (0x40U)

/* TOP_CFG1 - Amplitude Register Update */
#define DA7281_TOP_CFG1_AMP_REG_UPDATE  (0x80U)

/* TOP_CTL2 (0x23) - Override Value */
#define DA7281_TOP_CTL2_OVERRIDE_VAL_MASK (0xFFU) /**< Override amplitude value */

/* TOP_CTL1 (0x22) - Sequencer Start */
#define DA7281_TOP_CTL1_SEQ_START       (0x01U)

/* TOP_CTL1 - Sequencer Continue */
#define DA7281_TOP_CTL1_SEQ_CONTINUE    (0x02U)

/* ACTUATOR_NOMMAX (0x9B) - Voltage scaling factor */
#define DA7281_ACTUATOR_NOMMAX_SCALE    (23.4F)  /**< mV per LSB */

/* ACTUATOR_ABSMAX (0x9C) - Voltage scaling factor */
#define DA7281_ACTUATOR_ABSMAX_SCALE    (23.4F)    /**< mV per LSB */

/* ACTUATOR_IMAX - Current calculation constants (datasheet formula) */
#define DA7281_ACTUATOR_IMAX_OFFSET     (28.6F)    /**< mA offset */
#define DA7281_ACTUATOR_IMAX_SCALE      (7.2F)     /**< mA per LSB */

/* LRA_PER - Period calculation constant (datasheet formula) */
#define DA7281_LRA_PER_TIME_SCALE       (1.33332e-9F) /**< seconds per LSB */

/* V2I_FACTOR - Calculation constants (datasheet formula) */
#define DA7281_V2I_FACTOR_DIVISOR       (1.6104F)   /**< Formula divisor */
#define DA7281_V2I_FACTOR_IMAX_OFFSET   (4.0F)      /**< IMAX offset in formula */

/* Expected chip revision value (DA7281 Datasheet v3.1, Table 20) */
#define DA7281_CHIP_REV_VALUE           (0xCAU)
/* Legacy chip revision value observed on early boards */
#define DA7281_CHIP_REV_LEGACY_VALUE    (0xBAU)

#ifdef __cplusplus
}
#endif

#endif /* DA7281_REGISTERS_H */
