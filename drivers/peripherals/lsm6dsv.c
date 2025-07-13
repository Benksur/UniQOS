#include "lsm6dsv.h"
#include "errornum.h"
#include "stm32_config.h"
#include "i2c.h"

uint8_t lsm6dsv_init(void)
{
    uint8_t ret = 0;
    uint8_t buff = (1<<7);

    // Reboot for safety
    ret |= lsm6dsv_write_reg(LSM6DSV_CTRL3, &buff, 1);

    HAL_Delay(100);

    //software reset to defaults
    buff = 1;
    ret = lsm6dsv_write_reg(LSM6DSV_CTRL3, &buff, 1);
    if (ret)
        return ret;

    // Enalbe Sensors: High Performance Mode, 30Hz 
    buff = 0b00000100; 
    ret |= lsm6dsv_write_reg(LSM6DSV_CTRL1, &buff, 1);
    ret |= lsm6dsv_write_reg(LSM6DSV_CTRL2, &buff, 1);


    HAL_Delay(100);

    return ret;
}

uint8_t lsm6dsv_write_reg(uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    HAL_StatusTypeDef status;

    if (reg_data == NULL || reg_addr > LSM6DSV_FIFO_DATA_OUT_Z_H)
    {
        return EINVAL;
    }

    status = HAL_I2C_Mem_Write(&IMU_I2C_HANDLE, LSM6DSV_I2C_ADDR << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, len, 1000);

    if (status != HAL_OK)
    {
        return EIO;
    }

    return 0;
}

uint8_t lsm6dsv_read_reg(uint8_t reg_addr, uint8_t *reg_data, uint16_t len)
{
    HAL_StatusTypeDef status;

    if (reg_data == NULL || reg_addr > LSM6DSV_FIFO_DATA_OUT_Z_H)
    {
        return EINVAL;
    }

    status = HAL_I2C_Mem_Read(&IMU_I2C_HANDLE, LSM6DSV_I2C_ADDR << 1, reg_addr, I2C_MEMADD_SIZE_8BIT, reg_data, len, 1000);
    if (status != HAL_OK)
    {
        return EIO;
    }

    return 0;
}

uint8_t lsm6dsv_get_temp(float *temp)
{
    uint8_t ret = 0;
    uint8_t buf[2];
    int16_t val;

    ret |= lsm6dsv_read_reg(LSM6DSV_OUT_TEMP_L, buf, 2);

    if (ret)
        return ret;

    // from base 25c 256 LSB/°C
    val = (int16_t)((buf[1] << 8) | buf[0]);
    *temp = (val / 256.0f) + 25.0f;

    return ret;
}

uint8_t lsm6dsv_get_accel(float *ax, float *ay, float *az)
{
    uint8_t ret = 0;
    uint8_t buf[6];
    int16_t raw_x, raw_y, raw_z;

    ret = lsm6dsv_read_reg(LSM6DSV_OUTX_L_A, buf, 6);

    if (ret)
        return ret;

    raw_x = (int16_t)(buf[1] << 8 | buf[0]);
    raw_y = (int16_t)(buf[3] << 8 | buf[2]);
    raw_z = (int16_t)(buf[5] << 8 | buf[4]);

    // Assuming FS = ±2g → 0.061 mg/LSB
    const float sensitivity = 0.061f / 1000.0f; // g/LSB

    *ax = raw_x * sensitivity;
    *ay = raw_y * sensitivity;
    *az = raw_z * sensitivity;

    return ret;
}

uint8_t lsm6dsv_get_gyro(float *gx, float *gy, float *gz)
{
    uint8_t ret = 0;
    uint8_t buf[6];
    int16_t raw_x, raw_y, raw_z;

    ret = lsm6dsv_read_reg(LSM6DSV_OUTX_L_G, buf, 6);

    if (ret)
        return ret;

    raw_x = (int16_t)(buf[1] << 8 | buf[0]);
    raw_y = (int16_t)(buf[3] << 8 | buf[2]);
    raw_z = (int16_t)(buf[5] << 8 | buf[4]);

    // Assuming FS = ±125 dps → 4.375 mdps/LSB
    const float sensitivity = 4.375f / 1000.0f; // dps/LSB

    *gx = raw_x * sensitivity;
    *gy = raw_y * sensitivity;
    *gz = raw_z * sensitivity;

    return ret;
}

uint8_t lsm6dsv_get_all(lsm6dsv_data_t *data)
{
    uint8_t ret = 0;
    ret |= lsm6dsv_get_temp(&data->temp);
    ret |= lsm6dsv_get_gyro(&data->gx, &data->gy, &data->gz);
    ret |= lsm6dsv_get_accel(&data->ax, &data->ay, &data->az);

    return ret;
}
