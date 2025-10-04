/**
 * @file bq27441.h
 * @brief BQ27441 battery fuel gauge driver
 * @ingroup bq27441_driver
 *
 * Driver for the Texas Instruments BQ27441 single-cell Li-Ion battery fuel gauge.
 * Provides accurate battery state of charge, capacity, voltage, and current measurements.
 */

#ifndef BQ27441_H
#define BQ27441_H

#include <stdint.h>

/** @ingroup bq27441_driver
 *  @brief I2C address of BQ27441 */
#define BQ27441_I2C_ADDR 0x55

/*
 * Register addresses from datasheet
 * NOTE: All addresses are 2 bytes wide
 */
#define BQ27441_CMD_CNTL 0x00
#define BQ27441_CMD_TEMP 0x02
#define BQ27441_CMD_VOLTAGE 0x04
#define BQ27441_CMD_FLAGS 0x06
#define BQ27441_CMD_NOMINAL_AVALIABLE_CAPACITY 0x08
#define BQ27441_CMD_FULL_AVALIABLE_CAPACITY 0x0A
#define BQ27441_CMD_REMAINING_CAPACITY 0x0C
#define BQ27441_CMD_FULL_CHARGE_CAPACITY 0x0E
#define BQ27441_CMD_AVERAGE_CURRENT 0x10
#define BQ27441_CMD_STANDBY_CURRENT 0x12
#define BQ27441_CMD_MAX_LOAD_CURRENT 0x14
#define BQ27441_CMD_AVERAGE_POWER 0x18
#define BQ27441_CMD_STATE_OF_CHARGE 0x1C
#define BQ27441_CMD_INTERNAL_TEMPERATURE 0x1E
#define BQ27441_CMD_STATE_OF_HEALTH 0x20
#define BQ27441_CMD_REMAINING_CAPACITY_UNFILTERED 0x28
#define BQ27441_CMD_REMAINING_CAPACITY_FILTERED 0x2A
#define BQ27441_CMD_FULL_CHARGE_CAPACITY_UNFILTERED 0x2C
#define BQ27441_CMD_FULL_CHARGE_CAPACITY_FILTERED 0x2E
#define BQ27441_CMD_STATE_OF_CHARGE_UNFILTERED 0x30

/* Control Subcommands */
#define BQ27441_CNTL_CONTROL_STATUS 0x0000
#define BQ27441_CNTL_DEVICE_TYPE 0x0001
#define BQ27441_CNTL_FW_VERSION 0x0002
#define BQ27441_CNTL_DM_CODE 0x0004
#define BQ27441_CNTL_PREV_MACWRITE 0x0007
#define BQ27441_CNTL_CHEM_ID 0x0008
#define BQ27441_CNTL_BAT_INSERT 0x000C
#define BQ27441_CNTL_BAT_REMOVE 0x000D
#define BQ27441_CNTL_SET_HIBERNATE 0x0011
#define BQ27441_CNTL_CLEAR_HIBERNATE 0x0012
#define BQ27441_CNTL_SET_CFGUPDATE 0x0013
#define BQ27441_CNTL_SHUTDOWN_ENABLE 0x001B
#define BQ27441_CNTL_SHUTDOWN 0x001C
#define BQ27441_CNTL_SEALED 0x0020
#define BQ27441_CNTL_TOGGLE_GPOUT 0x0023
#define BQ27441_CNTL_RESET 0x0041
#define BQ27441_CNTL_SOFT_RESET 0x0042
#define BQ27441_CNTL_EXIT_CFGUPDATE 0x0043
#define BQ27441_CNTL_EXIT_RESIM 0x0044

/*
 * Extended Data Commands
 * NOTE: the following are 2 bytes wide
 * OP_CONFIG, DESIGN_CAPACITY, BLOCK_DATA
 */
#define BQ27441_CMD_OP_CONFIG 0x3A
#define BQ27441_CMD_DESIGN_CAPACITY 0x3C
#define BQ27441_CMD_DATA_CLASS 0x3E
#define BQ27441_CMD_DATA_BLOCK 0x3F
#define BQ27441_CMD_BLOCK_DATA 0x40
#define BQ27441_CMD_BLOCK_DATA_CHECKSUM 0x60
#define BQ27441_CMD_BLOCK_DATA_CONTROL 0x61

/**
 * @ingroup bq27441_driver
 * @brief Initialize the BQ27441 fuel gauge
 * @return 0 on success, error code otherwise
 */
uint8_t bq27441_init(void);

/**
 * @ingroup bq27441_driver
 * @brief Write to a BQ27441 register
 * @param reg_addr Register address
 * @param reg_data Pointer to data to write
 * @return 0 on success, error code otherwise
 */
uint8_t bq27441_write_reg(uint8_t reg_addr, uint16_t *reg_data);

/**
 * @ingroup bq27441_driver
 * @brief Read from a BQ27441 register
 * @param reg_addr Register address
 * @param reg_data Pointer to receive register data
 * @return 0 on success, error code otherwise
 */
uint8_t bq27441_read_reg(uint8_t reg_addr, uint16_t *reg_data);

/**
 * @ingroup bq27441_driver
 * @brief Read a control register
 * @param subcmd Subcommand to execute
 * @param response Pointer to receive response
 * @return 0 on success, error code otherwise
 */
uint8_t bq27441_read_ctrl_reg(uint16_t subcmd, uint16_t *response);

/**
 * @ingroup bq27441_driver
 * @brief Get battery state of charge (SOC)
 * @return Battery percentage (0-100%)
 */
uint16_t bq27441_SOC(void);

/**
 * @ingroup bq27441_driver
 * @brief Get average current
 * @return Average current in mA (negative = discharging)
 */
int16_t bq27441_avg_current(void);

/**
 * @ingroup bq27441_driver
 * @brief Get battery voltage
 * @return Battery voltage in mV
 */
uint16_t bq27441_voltage(void);

/**
 * @ingroup bq27441_driver
 * @brief Get available battery capacity (load compensated)
 * @return Available capacity in mAh
 */
uint16_t bq27441_available_capacity(void);

/**
 * @ingroup bq27441_driver
 * @brief Get full battery capacity (load compensated)
 * @return Full capacity in mAh
 */
uint16_t bq27441_full_capacity(void);

/**
 * @ingroup bq27441_driver
 * @brief Get battery state of health (SOH)
 * @return Health value where LSB = SOH percentage, MSB = status flags
 */
uint16_t bq27441_health(void);

#endif /* BQ27441_H */