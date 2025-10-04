/**
 * @file bq27441.c
 * @brief BQ27441 battery fuel gauge driver implementation
 *
 * This driver provides I2C communication and battery monitoring functions for the BQ27441
 * fuel gauge IC. It supports reading battery voltage, current, state of charge, and other
 * battery parameters.
 */


#include "bq27441.h"
#include "errornum.h"
#include "stm32_config.h"
#include "i2c.h"

/**
 * @brief Initialize the BQ27441 battery fuel gauge
 * @return 0 on success, error code on failure
 */
uint8_t bq27441_init(void)
{
    uint8_t ret = 0;

    return ret;
}

/**
 * @brief Write data to BQ27441 register via I2C
 * @param reg_addr Register address to write to
 * @param reg_data Pointer to 16-bit data to write
 * @return 0 on success, EINVAL for invalid parameters, EIO on I2C error
 */
uint8_t bq27441_write_reg(uint8_t reg_addr, uint16_t *reg_data)
{
    HAL_StatusTypeDef status;

    if (reg_data == NULL || reg_addr > BQ27441_CMD_STATE_OF_CHARGE_UNFILTERED)
    {
        return EINVAL;
    }

    status = HAL_I2C_Mem_Write(&BATT_I2C_HANDLE, BQ27441_I2C_ADDR << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)reg_data, 2, 1000);

    if (status != HAL_OK)
    {
        return EIO;
    }

    return 0;
}

/**
 * @brief Read data from BQ27441 register via I2C
 * @param reg_addr Register address to read from
 * @param reg_data Pointer to store 16-bit data
 * @return 0 on success, EINVAL for invalid parameters, EIO on I2C error
 */
uint8_t bq27441_read_reg(uint8_t reg_addr, uint16_t *reg_data)
{
    HAL_StatusTypeDef status;

    if (reg_data == NULL || reg_addr > BQ27441_CMD_STATE_OF_CHARGE_UNFILTERED)
    {
        return EINVAL;
    }

    status = HAL_I2C_Mem_Read(&BATT_I2C_HANDLE, BQ27441_I2C_ADDR << 1,
                              reg_addr, I2C_MEMADD_SIZE_8BIT, (uint8_t *)reg_data, 2, 1000);
    if (status != HAL_OK)
    {
        return EIO;
    }

    return 0;
}

/**
 * @brief Read control register from BQ27441
 * @param subcmd Control subcommand to send
 * @param response Pointer to store 16-bit response
 * @return 0 on success, EIO on I2C error
 */
uint8_t bq27441_read_ctrl_reg(uint16_t subcmd, uint16_t *response)
{
    HAL_StatusTypeDef status;

    uint8_t cmd_buf[2] = {
        (uint8_t)(subcmd & 0xFF),
        (uint8_t)((subcmd >> 8) & 0xFF)};

    status = HAL_I2C_Mem_Write(&BATT_I2C_HANDLE, BQ27441_I2C_ADDR << 1,
                               BQ27441_CMD_CNTL, I2C_MEMADD_SIZE_8BIT, cmd_buf, 2, 1000);

    if (status != HAL_OK)
        return EIO;

    HAL_Delay(2);

    return bq27441_read_reg(BQ27441_CMD_CNTL, response);
}

/**
 * @brief Read battery state of charge
 * @return State of charge percentage (0-100), 0 on error
 */
uint16_t bq27441_SOC(void)
{
    uint16_t data;
    uint8_t ret = bq27441_read_reg(BQ27441_CMD_STATE_OF_CHARGE, &data);

    if (ret)
    {
        return 0;
    }

    return data;
}

/**
 * @brief Read average battery current
 * @return Average current in mA, 0 on error
 */
int16_t bq27441_avg_current(void)
{
    uint16_t data;
    uint8_t ret = bq27441_read_reg(BQ27441_CMD_AVERAGE_CURRENT, &data);

    if (ret)
    {
        return 0;
    }

    return data;
}

/**
 * @brief Read battery voltage
 * @return Battery voltage in mV, 0 on error
 */
uint16_t bq27441_voltage(void)
{
    uint16_t data;
    uint8_t ret = bq27441_read_reg(BQ27441_CMD_VOLTAGE, &data);

    if (ret)
    {
        return 0;
    }

    return data;
}

/**
 * @brief Read remaining battery capacity
 * @return Remaining capacity in mAh, 0 on error
 */
uint16_t bq27441_available_capacity(void)
{
    uint16_t data;
    uint8_t ret = bq27441_read_reg(BQ27441_CMD_REMAINING_CAPACITY, &data);

    if (ret)
    {
        return 0;
    }

    return data;
}

/**
 * @brief Read full charge capacity
 * @return Full charge capacity in mAh, 0 on error
 */
uint16_t bq27441_full_capacity(void)
{
    uint16_t data;
    uint8_t ret = bq27441_read_reg(BQ27441_CMD_FULL_CHARGE_CAPACITY, &data);

    if (ret)
    {
        return 0;
    }

    return data;
}

/**
 * @brief Read battery state of health
 * @return State of health percentage (0-100), 0 on error
 */
uint16_t bq27441_health(void)
{
    uint16_t data;
    uint8_t ret = bq27441_read_reg(BQ27441_CMD_STATE_OF_HEALTH, &data);

    if (ret)
    {
        return 0;
    }

    return data;
}