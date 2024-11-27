/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_v4.h"
#include "24l01.h"
#include "utils.h"
#include "touch.h"
#include "24cxx.h"
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
uint8_t arrow_status[4] = {RELEASED};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
  lcd_init();
  tp_dev.init();
  NRF24L01_Init();

  // Checking NRF24L01 hardware
  while (NRF24L01_Check()) {
		lcd_show_string(30, 130, 200, 16, 16, "NRF24L01 Error", RED);
		HAL_Delay(200);
		lcd_fill(30, 130, 239, 130+16, WHITE);
		HAL_Delay(200);
  }

  // Testing data transmission to/from the car board
  test_communication();

  NRF24L01_TX_Mode();
  uint8_t command[32];

  draw_mode_selection_panel();
  draw_control_panel();

  uint16_t t = 0;
  uint16_t x_touch, y_touch;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // scan the touch
	  tp_dev.scan(0);
	  // if the screen is touched
	  if (tp_dev.sta & TP_PRES_DOWN) {
		  x_touch = tp_dev.x[0];
		  y_touch = tp_dev.y[0];
		  // if the touch coordinate is valid
		  if (x_touch < lcddev.width && y_touch < lcddev.height) {
			  // if the touch is on the right arrow
			  if (x_touch > 150 && x_touch < 220 && y_touch > 100 && y_touch < 170) {
				  if (arrow_status[0] == RELEASED) {
					  draw_arrow(BLACK, RIGHT);
					  command[0] = MOVE_RIGHT;
					  NRF24L01_TxPacket(command);
					  arrow_status[0] = PRESSED;
				  }
			  } else {
				  if (arrow_status[0] == PRESSED) {
					  draw_arrow(WHITE, RIGHT);
					  arrow_status[0] = RELEASED;
				  }
			  }
			  // if the touch is on the down arrow
			  if (x_touch > 86 && x_touch < 156 && y_touch > 165 && y_touch < 235) {
				  if (arrow_status[1] == RELEASED) {
					  draw_arrow(BLACK, DOWN);
					  command[0] = MOVE_BACKWARD;
					  NRF24L01_TxPacket(command);
					  arrow_status[1] = PRESSED;
				  }
			  } else {
				  if (arrow_status[1] == PRESSED) {
					  draw_arrow(WHITE, DOWN);
					  arrow_status[1] = RELEASED;
				  }
			  }
			  // if the touch is on the left arrow
			  if (x_touch > 20 && x_touch < 90 && y_touch > 100 && y_touch < 170) {
				  if (arrow_status[2] == RELEASED) {
					  draw_arrow(BLACK, LEFT);
					  command[0] = MOVE_LEFT;
					  NRF24L01_TxPacket(command);
					  arrow_status[2] = PRESSED;
				  }
			  } else {
				  if (arrow_status[2] == PRESSED) {
					  draw_arrow(WHITE, LEFT);
					  arrow_status[2] = RELEASED;
				  }
			  }
			  // if the touch is on the up arrow
			  if (x_touch > 86 && x_touch < 156 && y_touch > 35 && y_touch < 105) {
				  if (arrow_status[3] == RELEASED) {
					  draw_arrow(BLACK, UP);
					  command[0] = MOVE_FORWARD;
					  NRF24L01_TxPacket(command);
					  arrow_status[3] = PRESSED;
				  }
			  } else {
				  if (arrow_status[3] == PRESSED) {
					  draw_arrow(WHITE, UP);
					  arrow_status[3] = RELEASED;
				  }
			  }
		  }
	  } else { // if the screen is not touched
		  // set all arrow status to RELEASED if they are PRESSED
		  if (arrow_status[0] == PRESSED) {
			  draw_arrow(WHITE, RIGHT);
			  arrow_status[0] = RELEASED;
		  }
		  if (arrow_status[1] == PRESSED) {
			  draw_arrow(WHITE, DOWN);
			  arrow_status[1] = RELEASED;
		  }
		  if (arrow_status[2] == PRESSED) {
			  draw_arrow(WHITE, LEFT);
			  arrow_status[2] = RELEASED;
		  }
		  if (arrow_status[3] == PRESSED) {
			  draw_arrow(WHITE, UP);
			  arrow_status[3] = RELEASED;
		  }
		  // send BRAKE command to the car board if BRAKE has not been sent
		  if (command[0] != BRAKE) {
			  HAL_Delay(100);
			  command[0] = BRAKE;
			  NRF24L01_TxPacket(command);
		  }
	  }
	  t++;
	  if (t == 15000) {
		  t = 0;
		  HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
