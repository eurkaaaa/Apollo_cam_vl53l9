/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    i3c.c
  * @brief   This file provides code for the configuration
  *          of the I3C instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "i3c.h"

/* USER CODE BEGIN 0 */
extern DMA_HandleTypeDef handle_GPDMA1_Channel2;

extern DMA_HandleTypeDef handle_GPDMA1_Channel1;

extern DMA_HandleTypeDef handle_GPDMA1_Channel0;
/* USER CODE END 0 */

I3C_HandleTypeDef hi3c2;

/* I3C2 init function */
void MX_I3C2_Init(void)
{

  /* USER CODE BEGIN I3C2_Init 0 */

  /* USER CODE END I3C2_Init 0 */

  I3C_FifoConfTypeDef sFifoConfig = {0};
  I3C_CtrlConfTypeDef sCtrlConfig = {0};

  /* USER CODE BEGIN I3C2_Init 1 */

  /* USER CODE END I3C2_Init 1 */
  hi3c2.Instance = I3C2;
  hi3c2.Mode = HAL_I3C_MODE_CONTROLLER;
  hi3c2.Init.CtrlBusCharacteristic.SDAHoldTime = HAL_I3C_SDA_HOLD_TIME_0_5;
  hi3c2.Init.CtrlBusCharacteristic.WaitTime = HAL_I3C_OWN_ACTIVITY_STATE_0;
  hi3c2.Init.CtrlBusCharacteristic.SCLPPLowDuration = 0x5f;
  hi3c2.Init.CtrlBusCharacteristic.SCLI3CHighDuration = 0x3f;
  hi3c2.Init.CtrlBusCharacteristic.SCLODLowDuration = 0x5f;
  hi3c2.Init.CtrlBusCharacteristic.SCLI2CHighDuration = 0x00;
  hi3c2.Init.CtrlBusCharacteristic.BusFreeDuration = 0x27;
  hi3c2.Init.CtrlBusCharacteristic.BusIdleDuration = 0xc6;
  if (HAL_I3C_Init(&hi3c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure FIFO
  */
  sFifoConfig.RxFifoThreshold = HAL_I3C_RXFIFO_THRESHOLD_1_4;
  sFifoConfig.TxFifoThreshold = HAL_I3C_TXFIFO_THRESHOLD_1_4;
  sFifoConfig.ControlFifo = HAL_I3C_CONTROLFIFO_DISABLE;
  sFifoConfig.StatusFifo = HAL_I3C_STATUSFIFO_DISABLE;
  if (HAL_I3C_SetConfigFifo(&hi3c2, &sFifoConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure controller
  */
  sCtrlConfig.DynamicAddr = 0;
  sCtrlConfig.StallTime = 0x00;
  sCtrlConfig.HotJoinAllowed = DISABLE;
  sCtrlConfig.ACKStallState = DISABLE;
  sCtrlConfig.CCCStallState = DISABLE;
  sCtrlConfig.TxStallState = DISABLE;
  sCtrlConfig.RxStallState = DISABLE;
  sCtrlConfig.HighKeeperSDA = DISABLE;
  if (HAL_I3C_Ctrl_Config(&hi3c2, &sCtrlConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I3C2_Init 2 */

  /* USER CODE END I3C2_Init 2 */

}

void HAL_I3C_MspInit(I3C_HandleTypeDef* i3cHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
  if(i3cHandle->Instance==I3C2)
  {
  /* USER CODE BEGIN I3C2_MspInit 0 */

  /* USER CODE END I3C2_MspInit 0 */

  /** Initializes the peripherals clock
  */
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I3C2;
    PeriphClkInitStruct.I3c2ClockSelection = RCC_I3C2CLKSOURCE_PCLK1;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /* I3C2 clock enable */
    __HAL_RCC_I3C2_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**I3C2 GPIO Configuration
    PB11     ------> I3C2_SDA
    PB10     ------> I3C2_SCL
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_I3C2;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* I3C2 interrupt Init */
    HAL_NVIC_SetPriority(I3C2_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I3C2_EV_IRQn);
    HAL_NVIC_SetPriority(I3C2_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I3C2_ER_IRQn);
  /* USER CODE BEGIN I3C2_MspInit 1 */
    /* I3C2 DMA Init */
    /* GPDMA1_REQUEST_I3C2_TX Init */
    handle_GPDMA1_Channel2.Instance = GPDMA1_Channel2;
    handle_GPDMA1_Channel2.Init.Request = GPDMA1_REQUEST_I3C2_TX;
    handle_GPDMA1_Channel2.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_GPDMA1_Channel2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    handle_GPDMA1_Channel2.Init.SrcInc = DMA_SINC_INCREMENTED;
    handle_GPDMA1_Channel2.Init.DestInc = DMA_DINC_FIXED;
    handle_GPDMA1_Channel2.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel2.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel2.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_GPDMA1_Channel2.Init.SrcBurstLength = 1;
    handle_GPDMA1_Channel2.Init.DestBurstLength = 1;
    handle_GPDMA1_Channel2.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
    handle_GPDMA1_Channel2.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel2.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_GPDMA1_Channel2) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i3cHandle, hdmatx, handle_GPDMA1_Channel2);

    /* GPDMA1_REQUEST_I3C2_RX Init */
    handle_GPDMA1_Channel1.Instance = GPDMA1_Channel1;
    handle_GPDMA1_Channel1.Init.Request = GPDMA1_REQUEST_I3C2_RX;
    handle_GPDMA1_Channel1.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_GPDMA1_Channel1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    handle_GPDMA1_Channel1.Init.SrcInc = DMA_SINC_FIXED;
    handle_GPDMA1_Channel1.Init.DestInc = DMA_DINC_INCREMENTED;
    handle_GPDMA1_Channel1.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel1.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;
    handle_GPDMA1_Channel1.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_GPDMA1_Channel1.Init.SrcBurstLength = 1;
    handle_GPDMA1_Channel1.Init.DestBurstLength = 1;
    handle_GPDMA1_Channel1.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
    handle_GPDMA1_Channel1.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel1.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_GPDMA1_Channel1) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i3cHandle, hdmarx, handle_GPDMA1_Channel1);

    /* GPDMA1_REQUEST_I3C2_TC Init */
    handle_GPDMA1_Channel0.Instance = GPDMA1_Channel0;
    handle_GPDMA1_Channel0.Init.Request = GPDMA1_REQUEST_I3C2_TC;
    handle_GPDMA1_Channel0.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;
    handle_GPDMA1_Channel0.Init.Direction = DMA_PERIPH_TO_MEMORY;
    handle_GPDMA1_Channel0.Init.SrcInc = DMA_SINC_INCREMENTED;
    handle_GPDMA1_Channel0.Init.DestInc = DMA_DINC_FIXED;
    handle_GPDMA1_Channel0.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_WORD;
    handle_GPDMA1_Channel0.Init.DestDataWidth = DMA_DEST_DATAWIDTH_WORD;
    handle_GPDMA1_Channel0.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;
    handle_GPDMA1_Channel0.Init.SrcBurstLength = 1;
    handle_GPDMA1_Channel0.Init.DestBurstLength = 1;
    handle_GPDMA1_Channel0.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0|DMA_DEST_ALLOCATED_PORT0;
    handle_GPDMA1_Channel0.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;
    handle_GPDMA1_Channel0.Init.Mode = DMA_NORMAL;
    if (HAL_DMA_Init(&handle_GPDMA1_Channel0) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(i3cHandle, hdmacr, handle_GPDMA1_Channel0);

    /* I3C1 interrupt Init */
    HAL_NVIC_SetPriority(I3C2_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I3C2_EV_IRQn);
    HAL_NVIC_SetPriority(I3C2_ER_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I3C2_ER_IRQn);
  /* USER CODE END I3C2_MspInit 1 */
  }
}

void HAL_I3C_MspDeInit(I3C_HandleTypeDef* i3cHandle)
{

  if(i3cHandle->Instance==I3C2)
  {
  /* USER CODE BEGIN I3C2_MspDeInit 0 */

  /* USER CODE END I3C2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_I3C2_CLK_DISABLE();

    /**I3C2 GPIO Configuration
    PB11     ------> I3C2_SDA
    PB10     ------> I3C2_SCL
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_11|GPIO_PIN_10);

    /* I3C1 DMA DeInit */
    HAL_DMA_DeInit(i3cHandle->hdmatx);
    HAL_DMA_DeInit(i3cHandle->hdmarx);
    HAL_DMA_DeInit(i3cHandle->hdmacr);

    /* I3C2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(I3C2_EV_IRQn);
    HAL_NVIC_DisableIRQ(I3C2_ER_IRQn);

  /* USER CODE BEGIN I3C2_MspDeInit 1 */

  /* USER CODE END I3C2_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
