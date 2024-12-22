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
//#include "myMotor.h"
#include "utils.h"
#include "strings.h"
#include "24l01.h"
#include "atk_ms6050.h"
#include "inv_mpu.h"
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
const uint32_t interval_millis = 500;
uint32_t current_millis = 0;
uint8_t nrf_rx_buf[32] = {0};
uint8_t nrf_tx_buf[32] = {0};
uint8_t command_ready = 0;
extern float desired_yaw;

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
  MX_TIM5_Init();
  MX_TIM2_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  HAL_TIM_Base_Start(&htim1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_3);
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
  HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1);

  car_brake();

  /*探测*/
  uint8_t ret;
    /* 初始化ATK-MS6050 */
    ret = atk_ms6050_init();
    if (ret != 0)
    {
  //      HAL_UART_Transmit(&huart1, (uint8_t*)"ATK-MS6050 init failed!\r\n", strlen("ATK-MS6050 init failed!\r\n"), HAL_MAX_DELAY);
        while (1)
        {
      	  TOGGLE_LED1;
            HAL_Delay(200);
        }
    }

    /* 初始化ATK-MS6050 DMP */
    ret = atk_ms6050_dmp_init();
    if (ret != 0)
    {
  //      HAL_UART_Transmit(&huart1, (uint8_t*)"ATK-MS6050 DMP init failed!\r\n", strlen("ATK-MS6050 DMP init failed!\r\n"), HAL_MAX_DELAY);
        while (1)
        {
        	TOGGLE_LED2;
            HAL_Delay(200);
        }
    }

    //HAL_Delay(20000);
    desired_yaw = get_yaw();
  /////////////////////////////////////////////////////////
    car_run_up_one(0);
//  dfs(3, 0);
//  go_through(3, 0);
//  TURN_ON_LED1;
  while(1){continue;}
  /////////////////////////////////////////////////////////

  TURN_OFF_BUZZER;
  car_brake();
  NRF24L01_Init();


  // Adjust servo
  turn_joint(AHEAD);

  // Check if NRF24L01 module is working
  while (NRF24L01_Check()) {
	  HAL_Delay(100);
	  TURN_ON_LED1;
  }
  TURN_OFF_LED1;

  // Test communication with the control board
  test_communication();

  //uint8_t ret;
  /* 初始化ATK-MS6050 */
  ret = atk_ms6050_init();
  if (ret != 0)
  {
//      HAL_UART_Transmit(&huart1, (uint8_t*)"ATK-MS6050 init failed!\r\n", strlen("ATK-MS6050 init failed!\r\n"), HAL_MAX_DELAY);
      while (1)
      {
    	  TOGGLE_LED1;
          HAL_Delay(200);
      }
  }

  /* 初始化ATK-MS6050 DMP */
  ret = atk_ms6050_dmp_init();
  if (ret != 0)
  {
//      HAL_UART_Transmit(&huart1, (uint8_t*)"ATK-MS6050 DMP init failed!\r\n", strlen("ATK-MS6050 DMP init failed!\r\n"), HAL_MAX_DELAY);
      while (1)
      {
      	TOGGLE_LED2;
          HAL_Delay(200);
      }
  }

  HAL_Delay(20000);
  desired_yaw = get_yaw();


  NRF24L01_RX_Mode();
  command_ready = 0;
//  demo_run();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
while (1)
  {
	if (command_ready) {
//		HAL_UART_Transmit(&huart1, what, 1, HAL_MAX_DELAY);
		process_command(nrf_rx_buf);
		command_ready = 0;
	}
	if (HAL_GetTick() - current_millis >= interval_millis) {
		HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
		uint8_t distance_fetched = get_distance();
		nrf_tx_buf[0] = 0;
		nrf_tx_buf[1] = distance_fetched;
		nrf_send(nrf_tx_buf, 0);
		current_millis = HAL_GetTick();
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
