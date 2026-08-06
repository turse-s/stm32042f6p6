/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"
#include "sensor.h"
#include "parser.h"

UART_HandleTypeDef huart2;
sUart uart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */


/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&huart2);
}

void DMA1_Channel4_5_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&hdma_usart2_tx);
    HAL_DMA_IRQHandler(&hdma_usart2_rx);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    uint16_t copy_len = 0;
    if (huart->Instance == USART2)
    {
        copy_len = (Size > UART_RX_BUF_SIZE) ? UART_RX_BUF_SIZE : Size;
        // 将DMA接收缓冲区中的数据存入环形缓冲区
        for (uint16_t i = 0; i < copy_len; i++) {
            uart2.rxBuf[i] = uart2.ringBuf[i];
        }
        uart2.rxCnt = copy_len;
        uart2.frameReady = 1;
        // 重新启动 DMA 接收，等待下一帧
        HAL_UARTEx_ReceiveToIdle_DMA(huart, uart2.ringBuf, UART_RX_BUF_SIZE);
    }
}

void uart_Send(uint16_t cmd, float data)
{
    int sendData;
    int32_t abs_val;        // 临时存放绝对值（放大100倍）

    if (data < 0) {
        abs_val = (int32_t)(-data * 100);      // 负数取绝对值
        sendData = (uint16_t)(abs_val & 0x7FFF); // 最高位（Bit15）置 0，表示负数
    } else {
        abs_val = (int32_t)(data * 100);
        sendData = (uint16_t)(abs_val | 0x8000); // 最高位（Bit15）置 1，表示正数
    }
    
    uart2.txBuf[0] = 0xff;
    uart2.txBuf[1] = 0x01;
    uart2.txBuf[2] = (cmd ) >> 8;;
    uart2.txBuf[3] = (cmd ) & 0xff;
    uart2.txBuf[4] = (sendData ) >> 8;;
    uart2.txBuf[5] = (sendData ) & 0xff;
    uart2.txBuf[6] = uart2.txBuf[1] + uart2.txBuf[2] + uart2.txBuf[3] + uart2.txBuf[4] + uart2.txBuf[5];
    
    HAL_UART_Transmit_DMA(&huart2, uart2.txBuf, 7);
}


void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
      
      /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
        /* USER CODE BEGIN USART2_MspInit 1 */
    __HAL_UART_ENABLE_IT(uartHandle, UART_IT_IDLE);
  /* USER CODE BEGIN USART2_MspInit 1 */
  
    hdma_usart2_rx.Instance = DMA1_Channel5;   // USART2_RX 对应通道5[reference:4]
    hdma_usart2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_rx.Init.Mode = DMA_NORMAL;   // 循环模式
    hdma_usart2_rx.Init.Priority = DMA_PRIORITY_MEDIUM;
    HAL_DMA_Init(&hdma_usart2_rx);

    // 将 DMA 句柄与 UART 句柄关联
    __HAL_LINKDMA(uartHandle, hdmarx, hdma_usart2_rx);

    // 2. 配置 DMA TX (发送) —— 使用 DMA1 通道4 (可选，如果要用DMA发送)
    hdma_usart2_tx.Instance = DMA1_Channel4;   // USART2_TX 对应通道4[reference:5]
    hdma_usart2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_usart2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart2_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart2_tx.Init.Mode = DMA_NORMAL;
    hdma_usart2_tx.Init.Priority = DMA_PRIORITY_MEDIUM;
    HAL_DMA_Init(&hdma_usart2_tx);

    // 将 DMA 句柄与 UART 句柄关联
    __HAL_LINKDMA(uartHandle, hdmatx, hdma_usart2_tx);
    
    HAL_NVIC_SetPriority(DMA1_Channel4_5_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_5_IRQn);

  /* USER CODE END USART2_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);
      
  /* USART2 interrupt DeInit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    /* USER CODE BEGIN USART2_MspDeInit 1 */
    __HAL_UART_DISABLE_IT(uartHandle,UART_IT_RXNE);

  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
