#include "sensor.h"
#include "i2c.h"


uint8_t aht20_raw_data[6];
float temperature = 0.0f;
float humidity = 0.0f;
/**
  * @brief  触发AHT20测量并读取温湿度数据
  * @retval 0: 成功, 其他: 失败
  */
uint8_t AHT20_Read(float *temp, float *humi)
{
    uint8_t cmd[3];
    uint32_t hum_raw, temp_raw;
    uint8_t status;

    cmd[0] = AHT20_CMD_MEASURE;
    cmd[1] = 0x33;
    cmd[2] = 0x00;
    if (HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDR, cmd, 3, 100) != HAL_OK) {
        return 1;
    }

    HAL_Delay(80);

    if (HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDR, aht20_raw_data, 6, 100) != HAL_OK) {
        return 2; 
    }

    status = aht20_raw_data[0];
    if (status & 0x80) {
        return 3; // 传感器忙
    }
    
    hum_raw = ((uint32_t)aht20_raw_data[1] << 12) | 
              ((uint32_t)aht20_raw_data[2] << 4) | 
              (aht20_raw_data[3] >> 4);
    
    temp_raw = ((uint32_t)(aht20_raw_data[3] & 0x0F) << 16) | 
               ((uint32_t)aht20_raw_data[4] << 8) | 
               aht20_raw_data[5];

    *humi = (hum_raw / (float)(1 << 20)) * 100.0f;
    *temp = (temp_raw / (float)(1 << 20)) * 200.0f - 50.0f;

    return 0; // 成功
}
/* USER CODE END 0 */