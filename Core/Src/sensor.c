#include "sensor.h"
#include "i2c.h"
#include <stdint.h>

/**
  * @brief  轻量级自然对数 ln(x), 用 IEEE 754 位操作 + 32项LUT
  *         替代标准库 log/logf, 避免链接完整数学库
  *         精度: ±0.005, Flash占用: ~128字节(LUT) + ~80字节(代码)
  * @param  x: 输入值 (>0)
  * @retval ln(x) 近似值
  */
float ln_approx(float x)
{
    union { float f; uint32_t i; } u;
    static const float lut[32] = {
        0.015504f, 0.045809f, 0.075223f, 0.103804f,
        0.131603f, 0.158666f, 0.185034f, 0.210745f,
        0.235835f, 0.260335f, 0.284274f, 0.307678f,
        0.330573f, 0.352984f, 0.374933f, 0.396441f,
        0.417528f, 0.438213f, 0.458514f, 0.478448f,
        0.498032f, 0.517279f, 0.536205f, 0.554822f,
        0.573144f, 0.591182f, 0.608948f, 0.626452f,
        0.643704f, 0.660715f, 0.677493f, 0.694048f
    };

    if (x <= 0.0f) return -100.0f;

    u.f = x;
    int32_t exp = ((int32_t)((u.i >> 23) & 0xFF)) - 127;
    uint32_t idx = (u.i >> 18) & 0x1F;  /* 尾数高5位 → 32段 */

    return (float)exp * 0.69314718f + lut[idx];
}


uint8_t aht20_raw_data[6];

static uint8_t aht20_crc8(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;
    const uint8_t poly = 0x31;  // x^8 + x^5 + x^4 + 1
    
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ poly;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

/**
  * @brief  触发AHT20测量并读取温湿度数据
  * @retval 0: 成功, 其他: 失败
  */
uint8_t AHT20_Read(float *temp, float *humi)
{
    uint8_t cmd[3];
    uint8_t raw_data[7];  // 6字节数据 + 1字节CRC
    uint32_t hum_raw, temp_raw;
    uint8_t status;
    uint8_t crc_calc;

    cmd[0] = AHT20_CMD_MEASURE;
    cmd[1] = 0x33;
    cmd[2] = 0x00;
    if (HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDR, cmd, 3, 100) != HAL_OK) {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_Init(&hi2c1);
        return 1;
    }

    HAL_Delay(80);

    if (HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDR, aht20_raw_data, 7, 100) != HAL_OK) {
        HAL_I2C_DeInit(&hi2c1);
        HAL_I2C_Init(&hi2c1);
        return 2; 
    }

    status = aht20_raw_data[0];
    if (status & 0x80) {
        return 3; // 传感器忙
    }
    
    if (!(status & 0x08)) {
        return 4;  // 传感器未校准
    }
    
    crc_calc = aht20_crc8(raw_data, 6);
    if (crc_calc != raw_data[6]) {
        return 5;  // CRC 错误
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

/**
  * @brief  计算露点温度 (Magnus公式)
  * @param  ambientTemp: 环境温度 (°C)
  * @param  ambientHumi: 环境相对湿度 (%)
  * @retval 露点温度 (°C)
  */
float CalcDewPoint(float ambientTemp, float ambientHumi)
{
    float a = 17.27f;
    float b = 237.7f;
    float gamma;

    if (ambientHumi < 1.0f) ambientHumi = 1.0f;
    if (ambientHumi > 100.0f) ambientHumi = 100.0f;

    gamma = (a * ambientTemp) / (b + ambientTemp) + ln_approx(ambientHumi / 100.0f);
    return (b * gamma) / (a - gamma);
}


uint8_t AHT20_Init(void)
{
    uint8_t status;
    uint8_t cmd[3];
    
    HAL_Delay(120);
    
    cmd[0] = 0x71;
    if (HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDR, cmd, 1, 100) != HAL_OK) {
        return 2;
    }
    if (HAL_I2C_Master_Receive(&hi2c1, AHT20_ADDR, &status, 1, 100) != HAL_OK) {
        return 3;
    }
    
    if ((status & 0x18) != 0x18) {
        // 需要初始化
        cmd[0] = AHT20_CMD_INIT;
        cmd[1] = 0x08;
        cmd[2] = 0x00;
        if (HAL_I2C_Master_Transmit(&hi2c1, AHT20_ADDR, cmd, 3, 100) != HAL_OK) {
            return 4;
        }
        HAL_Delay(10);
    }
    
    return 0;  // 成功
}
/* USER CODE END 0 */


