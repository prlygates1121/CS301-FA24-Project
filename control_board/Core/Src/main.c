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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_v4.h"
#include "24l01.h"
#include "utils.h"
#include "touch.h"
#include "24cxx.h"
#include "stdbool.h"

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
uint8_t arrow_status[7] = {RELEASED};//5 stop 6 le 7 ri
int mode = 1;//0: control   1:draw   2:auto
uint8_t grid_status[16] = {0};
uint8_t path[16] = {-1};
int pnt = 0;
uint32_t lastTime = 0;
uint32_t debounceTime = -1;
uint32_t time_threshold = 70;
int signal = 0;
unsigned char DATA_TO_SEND[800];
int last_x = -1;
int last_y = -1;
int start_point = 0;//0:empty 1:choosed 2:setted
int end_point = 0;//0:empty 1:choosed 2:setted
int trap_point = 0;//0:empty 1:choosed 2:setted
int move_signal = 0;//0:still 1:move
int control = 0;//0:empty 1:choosed 2:setted
int debounceLabel = 0;//0:没按过 1:第一次按了

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//extern UART_HandleTypeDef huart1;
extern uint8_t rxBuffer[20];
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
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim1);
//  HAL_UART_Receive_IT(&huart1, (uint8_t *)rxBuffer, 1);
  lcd_init();
  tp_dev.init();
  NRF24L01_Init();

  // Checking NRF24L01 hardware
//  while (NRF24L01_Check()) {
//		lcd_show_string(30, 130, 200, 16, 16, "NRF24L01 Error", RED);
//		HAL_Delay(200);
//		lcd_fill(30, 130, 239, 130+16, WHITE);
//		HAL_Delay(200);
//  }

  // Testing data transmission to/from the car board
  //test_communication();

  NRF24L01_TX_Mode();
  uint8_t command[32];

  draw_mode_selection_panel();
  draw_control_panel();

  uint16_t t = 0;
  uint16_t x_touch, y_touch;

  //init
  for (int i = 0; i < 16; i++) {
      path[i] = -1;
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  // scan the touch
	  tp_dev.scan(0);
	  // if the screen is touched
	  if (tp_dev.sta & TP_PRES_DOWN) {//触摸屏被按下
		  x_touch = tp_dev.x[0];
		  y_touch = tp_dev.y[0];
		  // if the touch coordinate is valid
		  if (x_touch < lcddev.width && y_touch < lcddev.height) {
			  mode_switch_panel(x_touch, y_touch);
			  switch(mode){
				  case 0:
					  mode_control(x_touch, y_touch, 1);
					  break;
				  case 1:
					  mode_route(x_touch, y_touch, 1);
					  break;
				  case 2:
					  mode_auto(x_touch, y_touch);
					  break;
			  }
		  }
	  } else { // if the screen is not touched
		  //TODO debounce
		  GPIO_PinState start = HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);
		  if(start == GPIO_PIN_RESET)
		  {
			HAL_Delay(35);
			GPIO_PinState after = HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin);
			if(after == GPIO_PIN_RESET){
			  if (start_point == 1){
				  start_point = 2;
				  sprintf(DATA_TO_SEND, "start setted\n");
				  HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			  }else if (end_point == 1){
				  end_point = 2;
				  sprintf(DATA_TO_SEND, "end setted\n");
				  HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			  }else if (trap_point == 1){
				  trap_point = 0;//cause more than one trap to set
				  sprintf(DATA_TO_SEND, "trap setted\n");
				  HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			  }else if (control == 1){
				  control = 0;//reset to choose s/e/t mode
				  sprintf(DATA_TO_SEND, "route setted\n");
				  HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			  }
			}
		  }
		  start = HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
		  if(start == GPIO_PIN_RESET)
		  {
			HAL_Delay(35);
			GPIO_PinState after = HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin);
			if(after == GPIO_PIN_RESET){
			  move_signal = 1;
			  sprintf(DATA_TO_SEND, "moving!\n");
			  HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			}
		  }
		  //KEY_WKUP
		  start = HAL_GPIO_ReadPin(KEY_WKUP_GPIO_Port, KEY_WKUP_Pin);
		  if(start == GPIO_PIN_SET)
		  {
			HAL_Delay(35);
			GPIO_PinState after = HAL_GPIO_ReadPin(KEY_WKUP_GPIO_Port,KEY_WKUP_Pin);
			if(after == GPIO_PIN_SET){
				for (int i = 0; i < 16; i++) {
					path[i] = -1;
				}
				last_x = -1;
				last_y = -1;
				for(int i = 0; i < 4; i++){
					for(int j = 0; j < 4; j++){
						int index = (4*j) + i;
						if(grid_status[index] == 1){
							grid_status[index] = 0;
							//0 up / 1 down / 2 left / 3 right
							lcd_fill( 20+(50*i), 10+(50*j), 20+(50*i)+50, 10+(50*j)+50, WHITE);
							//lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
							lcd_draw_rectangle(20+(50*i), 10+(50*j), 20+(50*i)+50, 10+(50*j)+50, BLACK);
						}else if(grid_status[index] == 2 || grid_status[index] == 3){
							lcd_fill( 20+(50*i), 10+(50*j), 20+(50*i)+50, 10+(50*j)+50, WHITE);
							//lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
							lcd_draw_rectangle(20+(50*i), 10+(50*j), 20+(50*i)+50, 10+(50*j)+50, BLACK);
							lcd_update_endpoint(i, j);
						}
					}
				}
				sprintf(DATA_TO_SEND, "clear route\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			}
		  }

		  switch(mode){
			  case 0:
				  mode_control(x_touch, y_touch, 0);
				  break;
			  case 1:
				  mode_route(x_touch, y_touch, 0);
				  break;
			  case 2:
				  mode_auto(x_touch, y_touch);
				  break;
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
