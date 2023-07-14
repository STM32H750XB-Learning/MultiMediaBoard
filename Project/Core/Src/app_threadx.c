/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
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
#include "app_threadx.h"

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
TX_SEMAPHORE spi_tx_semaphore;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
    UINT ret = TX_SUCCESS;

    /* USER CODE BEGIN App_ThreadX_Init */
    /* Create stack check.  */
    tx_thread_stack_error_notify(stack_error_handler);

    /* Create spi tx.  */
    tx_semaphore_create(&spi_tx_semaphore, "spi tx sem", 1);
    /* USER CODE END App_ThreadX_Init */

    return ret;
}

/**
* @brief  Function that implements the kernel's initialization.
* @param  None
* @retval None
*/
void MX_ThreadX_Init(void)
{
    /* USER CODE BEGIN  Before_Kernel_Start */

    /* USER CODE END  Before_Kernel_Start */

    tx_kernel_enter();

    /* USER CODE BEGIN  Kernel_Start_Error */

    /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
void stack_error_handler(TX_THREAD *thread_ptr)
{
    int maxlen = TX_NAME_MAX;

    object_split(maxlen, "=");
    tx_kprintf("==============================================================================\r\n");
    tx_kprintf("The stack of the following thread overflows...\r\n");

    /* Traversing Thread Control List */
    tx_kprintf("%s", thread_ptr->tx_thread_name);

    while(1);		// halt at here
}
/* USER CODE END 1 */
