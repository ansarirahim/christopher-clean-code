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
 * Register Addresses (8-bit) - In Sequential Order per Table 20
 * ======================================================================== */

/** Chip revision register (Read-only) - Table 20: CHIP_REV, Reset=0xCA */
#define DA7281_REG_CHIP_REV             (0x00U)

/** IRQ event register 1 (write 1 to clear) - Table 20: IRQ_EVENT1 */
#define DA7281_REG_IRQ_EVENT1           (0x03U)

/** IRQ event warning diagnostic - Table 20: IRQ_EVENT_WARNING_DIAG */
#define DA7281_REG_IRQ_EVENT_WARNING_DIAG (0x04U)

/** IRQ event sequencer diagnostic - Table 20: IRQ_EVENT_SEQ_DIAG */
#define DA7281_REG_IRQ_EVENT_SEQ_DIAG   (0x05U)

/** IRQ status register 1 (read faults) - Table 20: IRQ_STATUS1 */
#define DA7281_REG_IRQ_STATUS1          (0x06U)

/** IRQ mask register 1 - Table 20: IRQ_MASK1 */
#define DA7281_REG_IRQ_MASK1            (0x07U)

/** I2C interface control 1 - Table 20: CIF_I2C1 */
#define DA7281_REG_CIF_I2C1             (0x08U)

/** I2C interface control 2 - Table 20: CIF_I2C2 */
#define DA7281_REG_CIF_I2C2             (0x09U)

/** LRA period high byte - Table 20: FRQ_LRA_PER_H */
#define DA7281_REG_LRA_PER_H            (0x0AU)

/** LRA period low byte - Table 20: FRQ_LRA_PER_L */
#define DA7281_REG_LRA_PER_L            (0x0BU)

/** Actuator nominal max voltage - Table 20: ACTUATOR1 (ACTUATOR_NOMMAX) */
#define DA7281_REG_ACTUATOR_NOMMAX      (0x0CU)

/** Actuator absolute max voltage - Table 20: ACTUATOR2 (ACTUATOR_ABSMAX) */
#define DA7281_REG_ACTUATOR_ABSMAX      (0x0DU)

/** Actuator max current - Table 20: ACTUATOR3 (IMAX) */
#define DA7281_REG_ACTUATOR_IMAX        (0x0EU)

/** V2I factor high byte - Table 20: CALIB_V2I_H */
#define DA7281_REG_V2I_FACTOR_H         (0x0FU)

/** V2I factor low byte - Table 20: CALIB_V2I_L */
#define DA7281_REG_V2I_FACTOR_L         (0x10U)

/** Calibration impedance high byte - Table 20: CALIB_IMP_H */
#define DA7281_REG_CALIB_IMP_H          (0x11U)

/** Calibration impedance low byte - Table 20: CALIB_IMP_L */
#define DA7281_REG_CALIB_IMP_L          (0x12U)

/** Top configuration register 1 (bit 5=actuator type) - Table 20: TOP_CFG1 */
#define DA7281_REG_TOP_CFG1             (0x13U)

/** Top configuration register 2 - Table 20: TOP_CFG2 */
#define DA7281_REG_TOP_CFG2             (0x14U)

/** Top configuration register 3 - Table 20: TOP_CFG3 */
#define DA7281_REG_TOP_CFG3             (0x15U)

/** Top configuration register 4 - Table 20: TOP_CFG4 */
#define DA7281_REG_TOP_CFG4             (0x16U)

/** Top interrupt configuration 1 - Table 20: TOP_INT_CFG1 */
#define DA7281_REG_TOP_INT_CFG1         (0x17U)

/** Top interrupt configuration 6 high byte - Table 20: TOP_INT_CFG6_H */
#define DA7281_REG_TOP_INT_CFG6_H       (0x1CU)

/** Top interrupt configuration 6 low byte - Table 20: TOP_INT_CFG6_L */
#define DA7281_REG_TOP_INT_CFG6_L       (0x1DU)

/** Top interrupt configuration 7 high byte - Table 20: TOP_INT_CFG7_H */
#define DA7281_REG_TOP_INT_CFG7_H       (0x1EU)

/** Top interrupt configuration 7 low byte - Table 20: TOP_INT_CFG7_L */
#define DA7281_REG_TOP_INT_CFG7_L       (0x1FU)

/** Top interrupt configuration 8 - Table 20: TOP_INT_CFG8 */
#define DA7281_REG_TOP_INT_CFG8         (0x20U)

/** Top control register 1 (bits [2:0]=operation mode) - Table 20: TOP_CTL1 */
#define DA7281_REG_TOP_CTL1             (0x22U)

/** Top control register 2 (override value) - Table 20: TOP_CTL2 */
#define DA7281_REG_TOP_CTL2             (0x23U)

/** Sequencer control 1 - Table 20: SEQ_CTL1 */
#define DA7281_REG_SEQ_CTL1             (0x24U)

/** Sequencer control 2 - Table 20: SEQ_CTL2 */
#define DA7281_REG_SEQ_CTL2             (0x28U)

/** GPI control - Table 20: GPI_CTL */
#define DA7281_REG_GPI_CTL              (0x2BU)

/** Memory control 1 - Table 20: MEM_CTL1 */
#define DA7281_REG_MEM_CTL1             (0x2CU)

/** Memory control 2 - Table 20: MEM_CTL2 */
#define DA7281_REG_MEM_CTL2             (0x2DU)

/** Polarity - Table 20: POLARITY */
#define DA7281_REG_POLARITY             (0x43U)

/** Top configuration 5 - Table 20: TOP_CFG5 */
#define DA7281_REG_TOP_CFG5             (0x6EU)

/** IRQ event actuator fault - Table 20: IRQ_EVENT_ACTUATOR_FAULT */
#define DA7281_REG_IRQ_EVENT_ACTUATOR_FAULT (0x81U)

/** IRQ status 2 - Table 20: IRQ_STATUS2 */
#define DA7281_REG_IRQ_STATUS2          (0x82U)

/** IRQ mask 2 - Table 20: IRQ_MASK2 */
#define DA7281_REG_IRQ_MASK2            (0x83U)

/** Waveform memory base address - Table 20: SNP_MEM_x (0x84-0xE7, x=0 to 99) */
#define DA7281_REG_SNP_MEM_BASE         (0x84U)
#define DA7281_REG_SNP_MEM_END          (0xE7U)

/* ========================================================================
 * Register Bit Field Definitions
 * ======================================================================== */

/* CHIP_REV (0x00) - Chip Revision - Table 21 */
#define DA7281_CHIP_REV_MINOR_MASK      (0xF0U)  /**< Bits [7:4] - Minor revision */
#define DA7281_CHIP_REV_MINOR_SHIFT     (4U)
#define DA7281_CHIP_REV_MAJOR_MASK      (0x0FU)  /**< Bits [3:0] - Major revision */
#define DA7281_CHIP_REV_MAJOR_SHIFT     (0U)

/* IRQ_EVENT1 (0x03) - IRQ Event Register 1 - Table 22 */
#define DA7281_IRQ_EVENT1_E_OC_FAULT        (0x80U)  /**< Bit 7 - Over-current/short-circuit fault */
#define DA7281_IRQ_EVENT1_E_ACTUATOR_FAULT  (0x40U)  /**< Bit 6 - Actuator fault */
#define DA7281_IRQ_EVENT1_E_WARNING         (0x20U)  /**< Bit 5 - System warnings */
#define DA7281_IRQ_EVENT1_E_SEQ_FAULT       (0x10U)  /**< Bit 4 - Sequence fault */
#define DA7281_IRQ_EVENT1_E_OVERTEMP_CRIT   (0x08U)  /**< Bit 3 - Critical over-temperature */
#define DA7281_IRQ_EVENT1_E_SEQ_DONE        (0x04U)  /**< Bit 2 - Sequence playback complete */
#define DA7281_IRQ_EVENT1_E_UVLO            (0x02U)  /**< Bit 1 - Under-voltage lockout */
#define DA7281_IRQ_EVENT1_E_SEQ_CONTINUE    (0x01U)  /**< Bit 0 - Sequence continue */

/* TOP_CTL1 (0x22) - Operation Mode Control */
#define DA7281_TOP_CTL1_OP_MODE_MASK    (0x07U)
#define DA7281_TOP_CTL1_OP_MODE_SHIFT   (0U)
#define DA7281_OP_MODE_INACTIVE         (0x00U)
#define DA7281_OP_MODE_DRO              (0x01U)  /**< Direct Register Override */
#define DA7281_OP_MODE_PWM              (0x02U)  /**< PWM Input Mode */
#define DA7281_OP_MODE_RTWM             (0x03U)  /**< Real-Time Waveform Mode */
#define DA7281_OP_MODE_ETWM             (0x04U)  /**< Embedded Waveform Mode */
#define DA7281_OP_MODE_STANDBY          (0x06U)  /**< Standby Mode */

/* TOP_CTL1 (0x22) - Additional Control Bits */
#define DA7281_TOP_CTL1_SEQ_START       (0x08U)  /**< Bit 3 - Sequencer start */
#define DA7281_TOP_CTL1_STANDBY_EN      (0x10U)  /**< Bit 4 - Standby enable */

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
#define DA7281_TOP_CTL2_OVERRIDE_VAL_MASK   (0xFFU)         /**< Override amplitude value */

/* ACTUATOR_NOMMAX (0x0C) - Voltage scaling factor */
#define DA7281_ACTUATOR_NOMMAX_SCALE        (23.4F)         /**< mV per LSB */

/* ACTUATOR_ABSMAX (0x0D) - Voltage scaling factor */
#define DA7281_ACTUATOR_ABSMAX_SCALE        (23.4F)         /**< mV per LSB */

/* ACTUATOR_IMAX (0x0E) - Current calculation constants (datasheet formula) */
#define DA7281_ACTUATOR_IMAX_OFFSET         (28.6F)         /**< mA offset */
#define DA7281_ACTUATOR_IMAX_SCALE          (7.2F)          /**< mA per LSB */

/* LRA_PER (0x0A/0x0B) - Period calculation constant (datasheet formula) */
#define DA7281_LRA_PER_TIME_SCALE           (1.33332e-9F)   /**< seconds per LSB */

/* V2I_FACTOR (0x0F/0x10) - Calculation constants (datasheet formula) */
#define DA7281_V2I_FACTOR_DIVISOR           (1.6104F)       /**< Formula divisor */
#define DA7281_V2I_FACTOR_IMAX_OFFSET       (4.0F)          /**< IMAX offset in formula */

/* Expected chip revision value (DA7281 Datasheet v3.1, Table 21) */
#define DA7281_CHIP_REV_VALUE               (0xCAU)
/* Legacy chip revision value observed on early boards */
#define DA7281_CHIP_REV_LEGACY_VALUE        (0xBAU)

#ifdef __cplusplus
}
#endif

#endif /* DA7281_REGISTERS_H */
