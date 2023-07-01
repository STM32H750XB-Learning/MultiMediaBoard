/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

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
#define LED_R_Pin GPIO_PIN_15
#define LED_R_GPIO_Port GPIOC
#define LED_B_Pin GPIO_PIN_8
#define LED_B_GPIO_Port GPIOI
#define ILI9488_PIN_DC_Pin GPIO_PIN_7
#define ILI9488_PIN_DC_GPIO_Port GPIOC
#define ILI9488_PIN_BL_Pin GPIO_PIN_6
#define ILI9488_PIN_BL_GPIO_Port GPIOC
#define KEY_USR_Pin GPIO_PIN_4
#define KEY_USR_GPIO_Port GPIOH
#define ILI9488_PIN_RESET_Pin GPIO_PIN_3
#define ILI9488_PIN_RESET_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
#define LED_R_ON()     	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET)
#define LED_R_OFF()    	HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET)
#define LED_R_TOGGLE()	HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin)

#define LED_B_ON()     	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_RESET)
#define LED_B_OFF()    	HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, GPIO_PIN_SET)
#define LED_B_TOGGLE()	HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin)

#define KEY_USR_READ()	HAL_GPIO_ReadPin(KEY_USR_GPIO_Port, KEY_USR_Pin)

#define ILI9488_PIN_DC_HIGH()		HAL_GPIO_WritePin(ILI9488_PIN_DC_GPIO_Port, ILI9488_PIN_DC_Pin, GPIO_PIN_SET)
#define ILI9488_PIN_DC_LOW()		HAL_GPIO_WritePin(ILI9488_PIN_DC_GPIO_Port, ILI9488_PIN_DC_Pin, GPIO_PIN_RESET)

#define ILI9488_PIN_BL_HIGH()		HAL_GPIO_WritePin(ILI9488_PIN_BL_GPIO_Port, ILI9488_PIN_BL_Pin, GPIO_PIN_SET)
#define ILI9488_PIN_BL_LOW()		HAL_GPIO_WritePin(ILI9488_PIN_BL_GPIO_Port, ILI9488_PIN_BL_Pin, GPIO_PIN_RESET)

#define ILI9488_PIN_RESET_HIGH()	HAL_GPIO_WritePin(ILI9488_PIN_RESET_GPIO_Port, ILI9488_PIN_RESET_Pin, GPIO_PIN_SET)
#define ILI9488_PIN_RESET_LOW()		HAL_GPIO_WritePin(ILI9488_PIN_RESET_GPIO_Port, ILI9488_PIN_RESET_Pin, GPIO_PIN_RESET)

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
