/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "tim.h"
#include "led.h"
#include "sensor.h"
#include "usart.h"
#include <math.h>

ADC_HandleTypeDef hadc1;

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

float NTC_GetTemp(uint16_t adc_val)
{
    float voltage = adc_val * 3.3f / 4095.0f;
    float Rt = 100000.0f * (3.3f - voltage) / voltage;
    float temp;
    temp = 1.0f / (1.0f / 298.15f + log(Rt / 100000.0f) / 3950.0f);
    temp = temp - 273.15f;
    return temp;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */

    HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
    SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
    LED_GPIO_Init();
    MX_I2C1_Init();
    MX_TIM3_Init();
    MX_USART2_UART_Init();
  
//  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

    adc1.sta = ADC_STA_IDLE;
    adc1.channelCnt = ADC_CHANNEL_NUM;  // 2路
    
    if (adcInit(&hadc1, ADC1, &adc1) != 0) {
        Error_Handler();    // 初始化失败
    }
    
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    timPwmPerCtrl(&htim3, TIM_PWM_CHANNEL_1, 5000);
  /* USER CODE END 2 */
    AHT20_Init();
    
    unsigned int per_myriad;
    uint16_t avg0 = 0, avg1 = 0;
    uint8_t pi_skip = 0;
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
    while (1)
    {
        ledCtrl(&Led1);
    /* USER CODE END WHILE */
      /* ---------- 1. 读取全部传感器 ---------- */
      
        uint32_t sum0 = 0, sum1 = 0;
        for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
            sum0 += adc1.channelVal[0];
            sum1 += adc1.channelVal[1];
            HAL_Delay(3);
        }
        avg0 = sum0 / ADC_BUFFER_SIZE;
        avg1 = sum1 / ADC_BUFFER_SIZE;


//      sensor.voltage1 = adc1.channelVal[3] * 3.3f / 4095.0f;
//      sensor.voltage2 = adc1.channelVal[2] * 3.3f / 4095.0f;
        sensor.tecTemp = NTC_GetTemp(avg0);
        sensor.airTemp = NTC_GetTemp(avg1);
        uint8_t aht20_ok = (AHT20_Read(&sensor.boardTemp, &sensor.boardHumi) == 0);

//      /* 逐路检查传感器有效性 */
//      uint8_t ntc0_valid = (adc1.channelVal[0] >= ADC_NTC_MIN)
//                        && (adc1.channelVal[0] <= ADC_NTC_MAX);
//      uint8_t ntc1_valid = (adc1.channelVal[1] >= ADC_NTC_MIN)
//                        && (adc1.channelVal[1] <= ADC_NTC_MAX);
//      
      /* ---------- 2. 计算制冷表面温度（仅有效NTC参与，AHT20不参与温控反馈） ---------- */
//      if (ntc0_valid) { surfaceTemp += ntc0_temp; ntc_valid_cnt++; }
//      if (ntc1_valid) { surfaceTemp += ntc1_temp; ntc_valid_cnt++; }
//      if (aht20_ok)   { surfaceTemp += airTemp; ntc_valid_cnt++; }
//      
//      /* NTC全部故障 → 关停制冷 */
//      if (ntc_valid_cnt == 0) {
//          timPwmPerCtrl(&htim3, TIM_CHANNEL_4, 0);
//          pi_integral = 0.0f;
//          HAL_Delay(LOOP_PERIOD_MS);
//          continue;
//      }
//      surfaceTemp /= (float)ntc_valid_cnt;
//      
      /* ---------- 3. 露点与防结露保护（使用【制冷表面温度】判断结露风险） ---------- */
      
        sensor.dewPointTemp = CalcDewPoint(sensor.boardTemp, sensor.boardHumi);
        float dewDist = sensor.tecTemp - sensor.dewPointTemp;
        float output;
        uint8_t pi_skip = 0;
      
        if (!dew_protect_active) {
          /* 下降沿: 距露点 < 1.5°C 进入保护 */
            if (dewDist < EMERGENCY_MARGIN) {
                dew_protect_active = 1;
                pi_integral = 0.0f;
                output = 0.0f;
                pi_skip = 1;
            }
        } else {
          /* 上升沿: 恢复到露点 + 2.5°C 才解除保护 */
            if (dewDist > (EMERGENCY_MARGIN + DEW_HYSTERESIS)) {
                dew_protect_active = 0;
              /* 解除保护, 继续执行 PI */
            } else {
                output = 0.0f;
                pi_skip = 1;  /* 保持关停, 跳过 PI 防止积分累积 */
            }
        }
      
      /* ---------- 4. PI控制器 (仅在非保护状态下执行) ---------- */
        if (!pi_skip) {
            float error = sensor.tecTemp - TARGET_TEMP;
            float p_term = KP * error;

            pi_integral += KI * error * (LOOP_PERIOD_MS / 1000.0f);

            if (pi_integral > PWM_MAX) pi_integral = PWM_MAX;
            if (pi_integral < 0.0f)    pi_integral = 0.0f;

            output = p_term + pi_integral;
            sensor.pwm_per_myriad = output;

            if (output > PWM_MAX) output = PWM_MAX;
            if (output < PWM_MIN) output = PWM_MIN;
        }
      
        per_myriad = (unsigned int)(output * 100.0f + 0.5f);
        if (per_myriad > 10000) per_myriad = 10000; 

        timPwmPerCtrl(&htim3, TIM_PWM_CHANNEL_1, (unsigned int)per_myriad);
      
        HAL_Delay(LOOP_PERIOD_MS);
      
    /* USER CODE BEGIN 3 */
    }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI14|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_SYSCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
