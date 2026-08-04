/**
  ******************************************************************************
  * @file    led.h
  * @brief   This file provides LED control functions including initialization, 
  *          on/off control and toggle operations.
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
#ifndef __LED_H__
#define __LED_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */


typedef enum
{
    LED_STA_OFF = 0,
    LED_STA_ON,
    LED_STA_FLASH,
} eLedSta;

// led type
typedef struct
{
    unsigned short num;
    unsigned int timeCnt;
    unsigned int cycleTime;
    unsigned int offTime;
    unsigned char enLev;
    eLedSta sta;
    sGpio gpio;
} sLed;
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void LED_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
