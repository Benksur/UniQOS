#include "lsm6dsv.h"
#include "errornum.h"
#include "stm32_config.h"
#include "i2c.h"

uint8_t lsm6dsv_init(void);

uint8_t lsm6dsv_write_reg(uint8_t reg_addr, uint8_t reg_data)
{
    // uint8_t data[2];
    // data[0] = (reg_addr << 1) | ((reg_data >> 8) & 0x01);
    // data[1] = reg_data & 0xFF;

    if (HAL_I2C_Master_Transmit(&IMU_I2C_HANDLE, LSM6DSV_I2C_ADDR << 1, &reg_data, 2, 100) != HAL_OK)
    {
        return EIO;
    }

    return 0; 
}

uint8_t lsm6dsv_read_reg(uint8_t reg_addr, uint8_t *reg_data)
{
    uint8_t addr_byte;
    uint8_t data;
    HAL_StatusTypeDef status;

    if (reg_data == NULL)
    {
        return EINVAL;
    }

    addr_byte = reg_addr << 1;

    status = HAL_I2C_Master_Transmit(&IMU_I2C_HANDLE, LSM6DSV_I2C_ADDR << 1, &addr_byte, 1, 100);
    if (status != HAL_OK)
    {
        return EIO;
    }

    status = HAL_I2C_Master_Receive(&AUDIO_I2C_HANDLE, LSM6DSV_I2C_ADDR << 1, &data, 1, 100);
    if (status != HAL_OK)
    {
        return EIO;
    }
    *reg_data = data;
    return 0; 
}