#include "lsm6dsv.h"
#include "errornum.h"
#include "stm32_config.h"
#include "i2c.h"

uint8_t lsm6dsv_init(void);

uint8_t lsm6dsv_write_reg(uint8_t reg_addr, uint8_t *reg_data, uint16_t data_size)
{
    HAL_StatusTypeDef status;

    if (reg_data == NULL || reg_addr == NULL || reg_addr > LSM6DSV_FIFO_DATA_OUT_Z_H)
    {
        return EINVAL;
    }
    
    status = HAL_I2C_Mem_Write(&IMU_I2C_HANDLE, LSM6DSV_I2C_ADDR, reg_addr, 1, reg_data, data_size, 100 );

    if (status != HAL_OK)
    {
        return EIO;
    }

    return 0; 
}

uint8_t lsm6dsv_read_reg(uint8_t reg_addr, uint8_t *reg_data)
{
    uint8_t data;
    HAL_StatusTypeDef status;

    if (reg_data == NULL || reg_addr == NULL || reg_addr > LSM6DSV_FIFO_DATA_OUT_Z_H)
    {
        return EINVAL;
    }

    status = HAL_I2C_Mem_Read(&IMU_I2C_HANDLE,LSM6DSV_I2C_ADDR << 1, reg_addr, 1, &data, 1, 100);
    if (status != HAL_OK)
    {
        return EIO;
    }

    *reg_data = data;
    return 0; 
}

// uint8_t lsm6dsv_get_status(uint8_t status)
// {
//     lsm6dsv_read_reg(LSM6DSV_STATUS_REG);
// }