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
  NRF24L01_Init();
  uint8_t key, mode;
  uint16_t t = 0;
  uint8_t tmp_buf[33];

  // Checking NRF24L01 hardware
  while (NRF24L01_Check()) {
		lcd_show_string(30, 130, 200, 16, 16, "NRF24L01 Error", RED);
		HAL_Delay(200);
		lcd_fill(30, 130, 239, 130+16, WHITE);
		HAL_Delay(200);
  }

  // Testing data transmission to/from the car board
  test_communication();
  while (1) {
		if (KEY0_PRESSED) {
			mode=0;
			break;
		} else if (KEY1_PRESSED) {
			mode=1;
			break;
		}
		t++;
		if (t == 100) lcd_show_string(10, 150, 230, 16, 16, "KEY0:RX_Mode  KEY1:TX_Mode", BLUE);
		if (t == 200) {
			lcd_fill(10, 150, 230, 150+16, WHITE);
			t = 0;
		}
		HAL_Delay(5);
  }
	lcd_fill(10,150,240,166,WHITE);
	if (mode==0) {
		lcd_show_string(30,150,200,16,16,"NRF24L01 RX_Mode", BLUE);
		lcd_show_string(30,170,200,16,16,"Received DATA:", BLUE);
		NRF24L01_RX_Mode();

	} else {
		lcd_show_string(30,150,200,16,16,"NRF24L01 TX_Mode", BLUE);
		NRF24L01_TX_Mode();
		mode=' ';
	}
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if (mode == 0) {
			if(NRF24L01_RxPacket(tmp_buf)==0)
			{
				tmp_buf[32]=0;
				lcd_show_string(0,190,lcddev.width-1,32,16,tmp_buf, BLUE);
			} else HAL_Delay_us(100);
			t++;
			if(t == 10000)
			{
				t=0;
				HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
			}
	  } else {
		  HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
		  HAL_Delay(700);
		  if (NRF24L01_TxPacket(tmp_buf)==TX_OK) {
				lcd_show_string(30,170,239,32,16,"Sended DATA:", BLUE);
				lcd_show_string(0, 190, lcddev.width-1, 32, 16, tmp_buf, BLUE);
				key=mode;
				for(t=0;t<32;t++)
				{
					key++;
					if(key>('~'))key=' ';
					tmp_buf[t]=key;
				}
				mode++;
				if (mode>'~') mode=' ';
				tmp_buf[32]=0;
			} else {
				lcd_fill(0, 170, lcddev.width, 170+16*3, WHITE);
				lcd_show_string(30, 170, lcddev.width-1, 32, 16, "Send Failed ", BLUE);
			};
			HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
			HAL_Delay(700);
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
