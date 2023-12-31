/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.h
  * @author  MCD Application Team
  * @brief   ThreadX applicative header file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __APP_THREADX_H__
#define __APP_THREADX_H__

#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include "tx_api.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "tx_execution_profile.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TX_NAME_MAX		20
/* USER CODE END PD */

/* Main thread defines -------------------------------------------------------*/
/* USER CODE BEGIN MTD */

/* USER CODE END MTD */

/* Exported macro ------------------------------------------------------------*/

/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
UINT App_ThreadX_Init(VOID *memory_ptr);
void MX_ThreadX_Init(void);
/* USER CODE BEGIN EFP */
void tx_kprintf(const char *fmt, ...);
static inline void object_split(int len, char *ch)
{
    while (len--) tx_kprintf("%s", ch);
}
UINT shell_thread_init(VOID);
UINT button_thread_init(VOID);
UINT sys_stat_thread_init(void);
UINT lcd_thread_init(VOID);
void stack_error_handler(TX_THREAD *thread_ptr);
/* USER CODE END EFP */

/* USER CODE BEGIN 1 */
extern EXECUTION_TIME	_tx_execution_thread_time_total;
extern EXECUTION_TIME 	_tx_execution_isr_time_total;
extern EXECUTION_TIME	_tx_execution_idle_time_total;
extern volatile double 	cpu_usage;
extern TX_SEMAPHORE spi_tx_semaphore;


/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif /* __APP_THREADX_H__ */
