/**
  ******************************************************************************
  * @file    led.c
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

/* Includes ------------------------------------------------------------------*/
#include "led.h"

sLed Led1;
sLed Led2;

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void LED_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB1 */
  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
    Led1.num = 0;
    Led1.cycleTime = 10;
    Led1.offTime = 30;
    Led1.enLev = 0;
    Led1.sta = LED_STA_FLASH;
    Led1.gpio.port = GPIO_A;
    Led1.gpio.pin = (eGpioPin)GPIO_PIN_7;
    Led1.gpio.sta = GPIO_LOW;
  
    Led2.num = 0;
    Led2.cycleTime = 10;
    Led2.offTime = 30;
    Led2.enLev = 0;
    Led2.sta = LED_STA_FLASH;
    Led2.gpio.port = GPIO_B;
    Led2.gpio.pin = (eGpioPin)GPIO_PIN_1;
    Led2.gpio.sta = GPIO_LOW;

}

/******************************************************************************
* @brief  led control
* @param  *led: led
* @retval  0: ok; other: err;
*******************************************************************************/
int ledCtrl(sLed *led)
{
    int err = 0;

    switch (led->sta) {
    case LED_STA_OFF:
        if (1 == led->enLev) {
            if (GPIO_HIGH == led->gpio.sta) {
                led->gpio.sta = GPIO_LOW;
                gpioWrite(&led->gpio);
            }
        } else {
            if (GPIO_LOW == led->gpio.sta) {
                led->gpio.sta = GPIO_HIGH;
                gpioWrite(&led->gpio);
            }
        }
        break;
    case LED_STA_ON:
        if (1 == led->enLev) {
            if (GPIO_LOW == led->gpio.sta) {
                led->gpio.sta = GPIO_HIGH;
                gpioWrite(&led->gpio);
            }
        } else {
            if (GPIO_HIGH == led->gpio.sta) {
                led->gpio.sta = GPIO_LOW;
                gpioWrite(&led->gpio);
            }
        }
        break;
    case LED_STA_FLASH:
        if ((led->timeCnt % led->cycleTime) == 0) {
            if (1 == led->enLev) {
                if (GPIO_LOW == led->gpio.sta) {
                    led->gpio.sta = GPIO_HIGH;
                    gpioWrite(&led->gpio);
                } else {
                    led->gpio.sta = GPIO_LOW;
                    gpioWrite(&led->gpio);
                }
            } else {
                if (GPIO_HIGH == led->gpio.sta) {
                    led->gpio.sta = GPIO_LOW;
                    gpioWrite(&led->gpio);
                } else {
                    led->gpio.sta = GPIO_HIGH;
                    gpioWrite(&led->gpio);
                }
            }
        }
        led->timeCnt++;
        if (led->timeCnt >= led->offTime) {
            led->timeCnt = 0;
#ifdef ENABLE_LED_AUTO_OFF
            led->sta = LED_STA_OFF;
#endif
        }
        break;
    default:
        err = 1;
        break;
    }

    return err;
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
