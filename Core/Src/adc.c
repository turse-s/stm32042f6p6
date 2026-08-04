/**
  ******************************************************************************
  * @file    adc.c
  * @brief   This file provides code for the configuration
  *          of the ADC instances.
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
#include "adc.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

DMA_HandleTypeDef hdma_adc;
ADC_HandleTypeDef hadc;
sAdc adc1;

/* ADC init function */
int adcInit(ADC_HandleTypeDef *hadc, ADC_TypeDef *adcx, sAdc *adc)
{

  /* USER CODE BEGIN ADC_Init 0 */

  /* USER CODE END ADC_Init 0 */

    int err = 0;
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC_Init 1 */

  /* USER CODE END ADC_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc->Instance = adcx;
  hadc->Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc->Init.Resolution = ADC_RESOLUTION_12B;
  hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc->Init.ScanConvMode = ADC_SCAN_DIRECTION_FORWARD;
  hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc->Init.LowPowerAutoWait = DISABLE;
  hadc->Init.LowPowerAutoPowerOff = DISABLE;
  hadc->Init.ContinuousConvMode = ENABLE;
  hadc->Init.DiscontinuousConvMode = DISABLE;
  hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc->Init.DMAContinuousRequests = ENABLE;
  hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
//  hadc->Init.NbrOfConversion = adc->channelCnt;  
//  hadc.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  if (HAL_ADC_Init(hadc) != HAL_OK)
  {
    Error_Handler();
  }
  
//    if (HAL_ADCEx_Calibration_Start(hadc) != HAL_OK) {
//        Error_Handler();
//    }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
    err++;
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
    err++;
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = 3;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
    err++;
  }
  /** Configure for the selected ADC regular channel to be converted.
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = 4;
  if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  
    if (HAL_ADC_Start_DMA(hadc, (uint32_t*)adc->channelVal, adc->channelCnt * ADC_BUFFER_SIZE) != HAL_OK) {
        err++;
    }
  /* USER CODE BEGIN ADC_Init 2 */

  /* USER CODE END ADC_Init 2 */
    return err;

}

void HAL_ADC_MspInit(ADC_HandleTypeDef* adcHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspInit 0 */

  /* USER CODE END ADC1_MspInit 0 */
    /* ADC1 clock enable */
    __HAL_RCC_ADC1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
      
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
    HAL_NVIC_SetPriority(DMA1_Ch1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(DMA1_Ch1_IRQn);

    /**ADC GPIO Configuration
    PA0     ------> ADC_IN0
    PA1     ------> ADC_IN1
    PA4     ------> ADC_IN4
    PA5     ------> ADC_IN5
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    /* ADC1 DMA Init */
    /* ADC Init */
    hdma_adc.Instance = DMA1_Channel1;
    hdma_adc.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc.Init.Mode = DMA_CIRCULAR;
    hdma_adc.Init.Priority = DMA_PRIORITY_HIGH;
    if (HAL_DMA_Init(&hdma_adc) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(adcHandle,DMA_Handle,hdma_adc);

  /* USER CODE BEGIN ADC1_MspInit 1 */

  /* USER CODE END ADC1_MspInit 1 */
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* adcHandle)
{

  if(adcHandle->Instance==ADC1)
  {
  /* USER CODE BEGIN ADC1_MspDeInit 0 */

  /* USER CODE END ADC1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_ADC1_CLK_DISABLE();

    /**ADC GPIO Configuration
    PA0     ------> ADC_IN0
    PA1     ------> ADC_IN1
    PA4     ------> ADC_IN4
    PA5     ------> ADC_IN5
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5);
      
      /* ADC1 DMA DeInit */
    HAL_DMA_DeInit(adcHandle->DMA_Handle);
    HAL_NVIC_DisableIRQ(ADC1_COMP_IRQn);

  /* USER CODE BEGIN ADC1_MspDeInit 1 */

  /* USER CODE END ADC1_MspDeInit 1 */
  }
}



// DMA半传输完成和完全传输完成回调
void HAL_DMA_ConvHalfCpltCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma->Instance == DMA1_Channel1) {
        // 前半段缓冲区已满，可以处理前半段数据
        ProcessHalfBuffer(&adc1, 0, ADC_BUFFER_SIZE/2);
    }
}

void HAL_DMA_ConvCpltCallback(DMA_HandleTypeDef *hdma)
{
    if (hdma->Instance == DMA1_Channel1) {
        // 后半段缓冲区已满，处理后半段数据
        ProcessHalfBuffer(&adc1, ADC_BUFFER_SIZE/2, ADC_BUFFER_SIZE);
        adc1.bufferReady = 1;  // 通知主循环有新数据
    }
}

// 处理缓冲区数据
void ProcessHalfBuffer(sAdc *adc, uint16_t start, uint16_t end)
{
    // 对每个通道的数据进行滤波处理
    for (int ch = 0; ch < adc->channelCnt; ch++) {
        uint32_t sum = 0;
        for (int i = start; i < end; i++) {
            sum += adc->dmaBuffer[i * adc->channelCnt + ch];
        }
        adc->filteredVal[ch] = sum / (end - start);  // 平均值滤波
    }
}
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
