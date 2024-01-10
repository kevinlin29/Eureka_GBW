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
#include "stm32f4xx_hal.h"

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
#define C13_Pin GPIO_PIN_13
#define C13_GPIO_Port GPIOC
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOA
#define hx711_dt_Pin GPIO_PIN_3
#define hx711_dt_GPIO_Port GPIOA
#define hx711_sck_Pin GPIO_PIN_4
#define hx711_sck_GPIO_Port GPIOA
#define RELAY_Pin GPIO_PIN_5
#define RELAY_GPIO_Port GPIOA
#define MENU_Pin GPIO_PIN_12
#define MENU_GPIO_Port GPIOB
#define UP_Pin GPIO_PIN_13
#define UP_GPIO_Port GPIOB
#define DOWN_Pin GPIO_PIN_14
#define DOWN_GPIO_Port GPIOB
#define KEYB15_Pin GPIO_PIN_15
#define KEYB15_GPIO_Port GPIOB
#define OLED_RES_Pin GPIO_PIN_5
#define OLED_RES_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
