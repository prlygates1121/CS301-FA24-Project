#include <stdint.h>
#include "utils.h"
#include "24l01.h"
#include "lcd_v4.h"
#include "tim.h"

void HAL_Delay_us(uint32_t us) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}

// Testing data transmission to/from the car board
void test_communication() {
	uint8_t test_data_sent[32];
	uint8_t test_data_received[32];
	for (uint8_t i = 0; i < 32; i++) {
	  test_data_sent[i] = i;
	}
	NRF24L01_TX_Mode();
	while (NRF24L01_TxPacket(test_data_sent) != TX_OK) {
		lcd_show_string(10, 170, 239, 32, 16, "Testing data transmission...", BLUE);
		HAL_GPIO_TogglePin(LED0_GPIO_Port, LED0_Pin);
		HAL_Delay(200);
		lcd_fill(10, 170, 240, 170 + 16, WHITE);
		HAL_Delay(200);
	}
	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	NRF24L01_RX_Mode();
	while (1) {
		if (NRF24L01_RxPacket(test_data_received) == 0) {
			uint8_t equal = 1;
			for (uint8_t i = 0; i < 32; i++) {
				if (test_data_received[i] != test_data_sent[i]) {
					equal = 0;
					break;
				}
			}
			if (equal == 1) {
				lcd_clear(WHITE);
				lcd_show_string(30, 300, 239, 32, 12, "Testing data transmission: OK", BLUE);
				HAL_Delay(1000);
				lcd_clear(WHITE);
				break;
			} else {
				lcd_show_string(30, 20, 239, 32, 16, "Received corrupted data:", BLUE);
				lcd_show_num(30, 70, test_data_received[0], 1, 16, BLACK);
				lcd_show_num(30, 90, test_data_received[1], 1, 16, BLACK);
				lcd_show_num(30, 110, test_data_received[2], 1, 16, BLACK);
				lcd_show_num(30, 130, test_data_received[3], 1, 16, BLACK);
				lcd_show_num(30, 150, test_data_received[4], 1, 16, BLACK);
				lcd_show_num(30, 170, test_data_received[5], 1, 16, BLACK);
			}
		}
	}
}
