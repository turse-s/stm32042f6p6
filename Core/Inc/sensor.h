#ifndef _SENSOR_H
#define _SENSOR_H

#include "main.h"
#include "math.h"


#define AHT20_ADDR         0x70
#define AHT20_CMD_INIT     0xBE
#define AHT20_CMD_MEASURE  0xAC
#define AHT20_CMD_RESET    0xBA

typedef struct {
    float airTemp;      // 空气温度（来自 AHT20）
    float airHumi;      // 空气湿度（来自 AHT20）
    float ntc0_temp;    // NTC0 温度
    float ntc1_temp;    // NTC1 温度
    float voltage1;
    float voltage2;
    uint8_t aht20_ok;   // AHT20 读取成功标志（1:成功, 0:失败）
} SensorData_t;

extern SensorData_t sensor;

uint8_t AHT20_Read(float *temp, float *humi);
float CalcDewPoint(float ambientTemp, float ambientHumi);
uint8_t AHT20_Init(void);
float ln_approx(float x);
#endif
