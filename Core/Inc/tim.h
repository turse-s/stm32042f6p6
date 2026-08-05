/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

typedef enum
{
    TIM_PWM_CHANNEL_1 = 1,
    TIM_PWM_CHANNEL_2 = 2,
    TIM_PWM_CHANNEL_3 = 4,
    TIM_PWM_CHANNEL_4 = 8,
    TIM_PWM_CHANNEL_ALL = 0x0F,
} eTimPwmChan;



extern TIM_HandleTypeDef htim3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM3_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void timPwmPerCtrl(TIM_HandleTypeDef *htim, eTimPwmChan channel, unsigned int per);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
