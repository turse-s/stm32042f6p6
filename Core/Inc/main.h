/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

#define TARGET_VOLTAGE         1.0f

/* ---------- PI温控参数 ---------- */
#define TARGET_TEMP         5.0f    /* 目标温度 (°C), 固定值          */
#define EMERGENCY_MARGIN    1.5f    /* 距露点<此值紧急关停 (°C)       */
#define DEW_HYSTERESIS      1.0f    /* 防结露保护回差 (°C)            */
#define KP                  25.0f   /* 比例系数 (每°C偏差输出25%PWM)  */
#define KI                  1.0f    /* 积分系数 (每°C·秒累加1%PWM)    */
#define LOOP_PERIOD_MS      50    /* 控制周期 (ms)                  */
#define PWM_MAX             100.0f
#define PWM_MIN             0.0f

/* NTC 断线/短路检测: 100kΩ NTC + 100kΩ 上拉到 3.3V */
#define ADC_NTC_MIN         300     /* ADC<300 → NTC短路或温度>85°C    */
#define ADC_NTC_MAX         3900    /* ADC>3900 → NTC断线或温度<-30°C  */

static float pi_integral = 0.0f;    /* 积分累加项, 带抗饱和保护       */
static uint8_t dew_protect_active = 0; /* 防结露保护状态 (带回差)     */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
