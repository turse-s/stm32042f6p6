/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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
#include "gpio.h"

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
//void MX_GPIO_Init(void)
//{

//  /* GPIO Ports Clock Enable */
//  __HAL_RCC_GPIOF_CLK_ENABLE();
//  __HAL_RCC_GPIOA_CLK_ENABLE();
//  __HAL_RCC_GPIOB_CLK_ENABLE();

//}


/******************************************************************************
* @brief  gpio read status
* @param  *gpio: gpio
* @retval 0: ok; other: err;
*******************************************************************************/
int gpioRead(sGpio *gpio)
{
    gpio->sta = (eGpioSta)HAL_GPIO_ReadPin((GPIO_TypeDef*)gpio->port, gpio->pin);

    return 0;
}

/******************************************************************************
* @brief  gpio write status
* @param  *gpio: gpio
* @retval 0: ok; other: err;
*******************************************************************************/
int gpioWrite(sGpio *gpio)
{
    HAL_GPIO_WritePin((GPIO_TypeDef*)gpio->port, gpio->pin, (GPIO_PinState)gpio->sta);

    return 0;
}

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
