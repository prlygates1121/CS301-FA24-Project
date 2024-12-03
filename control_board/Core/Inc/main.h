/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern int mode;
extern uint8_t arrow_status[7];
extern uint8_t grid_status[16];
extern uint8_t path[16];
extern int pnt;
extern uint32_t lastTime;
extern uint32_t time_threshold;
extern int signal;
extern unsigned char DATA_TO_SEND[800];
extern int last_x;
extern int last_y;
extern UART_HandleTypeDef huart1;
extern int start_point;
extern int end_point;
extern int trap_point;
extern int move_signal;
extern int control;
extern uint32_t debounceTime;
extern int debounceLabel;
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
#define KEY_WKUP_Pin GPIO_PIN_0
#define KEY_WKUP_GPIO_Port GPIOA
#define MX_nRF24_IRQ_Pin GPIO_PIN_2
#define MX_nRF24_IRQ_GPIO_Port GPIOA
#define MX_nRF24_IRQ_EXTI_IRQn EXTI2_IRQn
#define MX_nRF24_CE_Pin GPIO_PIN_3
#define MX_nRF24_CE_GPIO_Port GPIOA
#define MX_nRF24_CEA4_Pin GPIO_PIN_4
#define MX_nRF24_CEA4_GPIO_Port GPIOA
#define KEY0_Pin GPIO_PIN_5
#define KEY0_GPIO_Port GPIOC
#define LED0_Pin GPIO_PIN_8
#define LED0_GPIO_Port GPIOA
#define KEY1_Pin GPIO_PIN_15
#define KEY1_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOD

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
