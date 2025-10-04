/**
 * @file lsm6dsv.c
 * @brief LSM6DSV IMU sensor driver implementation
 *
 * This driver provides I2C communication and sensor control functions for the LSM6DSV
 * 6-axis IMU (accelerometer and gyroscope). It supports high-performance mode operation
 * and data reading from the sensor registers.
 */

#include "lsm6dsv.h"
#include "errornum.h"
#include "stm32_config.h"
#include "i2c.h"


/**
 * @brief Initialize the LSM6DSV IMU sensor
 * @return 0 on success, error code on failure
 */
uint8_t lsm6dsv_init(void)
{
    uint8_t ret = 0;
    uint8_t buff = (1 << 7);

    // reboot for safety
    ret |= lsm6dsv_write_reg(LSM6DSV_CTRL3, &buff, 1);

    HAL_Delay(100);

    // software reset to defaults
    buff = 1;
    ret = lsm6dsv_write_reg(LSM6DSV_CTRL3, &buff, 1);
    if (ret)
        return ret;

    // enable sensors: high performance mode, 30hz
    buff = 0b00000100;
    ret |= lsm6dsv_write_reg(LSM6DSV_CTRL1, &buff, 1);
    ret |= lsm6dsv_write_reg(LSM6DSV_CTRL2, &buff, 1);

    HAL_Delay(100);

    return ret;
}

/**
 * @brief Write data to LSM6DSV register via I2C
 * @param reg_addr Register address to write to
 * @param reg_data Pointer to data to write
 * @param len Number of bytes to write
 * @return 0 on success, EINVAL for invalid parameters, EIO on I2C error
 */
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

/**
 * @brief Read data from LSM6DSV register via I2C
 * @param reg_addr Register address to read from
 * @param reg_data Pointer to buffer to store read data
 * @param len Number of bytes to read
 * @return 0 on success, EINVAL for invalid parameters, EIO on I2C error
 */
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

/**
 * @brief Read temperature from LSM6DSV sensor
 * @param temp Pointer to store temperature value in Celsius
 * @return 0 on success, error code on failure
 */
uint8_t lsm6dsv_get_temp(float *temp)
{
    uint8_t ret = 0;
    uint8_t buf[2];
    int16_t val;

    ret |= lsm6dsv_read_reg(LSM6DSV_OUT_TEMP_L, buf, 2);

    if (ret)
        return ret;

    // from base 25c 256 lsb/°c
    val = (int16_t)((buf[1] << 8) | buf[0]);
    *temp = (val / 256.0f) + 25.0f;

    return ret;
}

/**
 * @brief Read accelerometer data from LSM6DSV sensor
 * @param ax Pointer to store X-axis acceleration in g
 * @param ay Pointer to store Y-axis acceleration in g
 * @param az Pointer to store Z-axis acceleration in g
 * @return 0 on success, error code on failure
 */
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

    // assuming fs = ±2g → 0.061 mg/lsb
    const float sensitivity = 0.061f / 1000.0f; // g/lsb

    *ax = raw_x * sensitivity;
    *ay = raw_y * sensitivity;
    *az = raw_z * sensitivity;

    return ret;
}

/**
 * @brief Read gyroscope data from LSM6DSV sensor
 * @param gx Pointer to store X-axis angular velocity in dps
 * @param gy Pointer to store Y-axis angular velocity in dps
 * @param gz Pointer to store Z-axis angular velocity in dps
 * @return 0 on success, error code on failure
 */
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

    // assuming fs = ±125 dps → 4.375 mdps/lsb
    const float sensitivity = 4.375f / 1000.0f; // dps/lsb

    *gx = raw_x * sensitivity;
    *gy = raw_y * sensitivity;
    *gz = raw_z * sensitivity;

    return ret;
}

/**
 * @brief Read all sensor data (temperature, gyroscope, accelerometer)
 * @param data Pointer to lsm6dsv_data_t structure to store all sensor data
 * @return 0 on success, error code on failure
 */
uint8_t lsm6dsv_get_all(lsm6dsv_data_t *data)
{
    uint8_t ret = 0;
    ret |= lsm6dsv_get_temp(&data->temp);
    ret |= lsm6dsv_get_gyro(&data->gx, &data->gy, &data->gz);
    ret |= lsm6dsv_get_accel(&data->ax, &data->ay, &data->az);

    return ret;
}
