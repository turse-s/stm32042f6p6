#ifndef _SENSOR_H
#define _SENSOR_H

#include "main.h"


#define AHT20_ADDR         0x70
#define AHT20_CMD_INIT     0xBE
#define AHT20_CMD_MEASURE  0xAC
#define AHT20_CMD_RESET    0xBA

extern uint8_t aht20_raw_data[6];

uint8_t AHT20_Read(float *temp, float *humi);
float CalcDewPoint(float ambientTemp, float ambientHumi);
uint8_t AHT20_Init(void);
float ln_approx(float x);
#endif
