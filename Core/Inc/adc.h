/**
  ******************************************************************************
  * @file    adc.h
  * @brief   This file contains all the function prototypes for
  *          the adc.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __ADC_H__
#define __ADC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h" 
#define ADC_CHANNEL_NUM     3
#define ADC_BUFFER_SIZE     10  // 每个通道采集10次取平均

typedef enum {
    ADC_STA_IDLE = 0,
    ADC_STA_BUSY,
    ADC_STA_COMPLETE
} eAdcSta;

/* ADC 结构体 */
typedef struct {
    eAdcSta sta;
    unsigned char channelCnt;
    unsigned short channelVal[ADC_CHANNEL_NUM];
    unsigned short dmaBuffer[ADC_CHANNEL_NUM * ADC_BUFFER_SIZE];  // DMA缓冲区
    unsigned short filteredVal[ADC_CHANNEL_NUM];  // 滤波后的值
    volatile uint8_t bufferReady;
} sAdc;

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern ADC_HandleTypeDef hadc;
extern sAdc adc1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_ADC_Init(void);
int adcInit(ADC_HandleTypeDef *hadc, ADC_TypeDef *adcx, sAdc *adc);
void ProcessHalfBuffer(sAdc *adc, uint16_t start, uint16_t end);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
