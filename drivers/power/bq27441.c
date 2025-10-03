#include "bq27441.h"
#include "errornum.h"
#include "stm32_config.h"
#include "i2c.h"

uint8_t bq27441_init(void)
{
    uint8_t ret = 0;

    return ret;
}

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

uint8_t bq27441_read_ctrl_reg(uint16_t subcmd, uint16_t *response)
{
    HAL_StatusTypeDef status;

    uint8_t cmd_buf[2] = {
        (uint8_t)(subcmd & 0xFF),
        (uint8_t)((subcmd >> 8) & 0xFF)};

    status = HAL_I2C_Mem_Write( &BATT_I2C_HANDLE, BQ27441_I2C_ADDR << 1,
        BQ27441_CMD_CNTL, I2C_MEMADD_SIZE_8BIT, cmd_buf, 2, 1000);

    if (status != HAL_OK)
        return EIO;

    HAL_Delay(2);

    return bq27441_read_reg(BQ27441_CMD_CNTL, response);
}

uint8_t bq27441_SOC(void)
{
    uint16_t data;
    uint8_t ret = bq27441_read_reg(BQ27441_CMD_STATE_OF_CHARGE, &data);

    if(ret){
        return 0;
    }

    return data;
}

uint8_t bq27441_SOC()
{
    uint16_t data;
    uint8_t ret = bq27441_read_reg(BQ27441_CMD_STATE_OF_CHARGE, &data);

    if(ret){
        return 0;
    }

    return data;
}