#include <stdint.h>
#include "utils.h"
#include "24l01.h"
#include "lcd_v4.h"
#include "images.h"
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

void draw_control_panel() {
	draw_arrow(WHITE, RIGHT);
	draw_arrow(WHITE, DOWN);
	draw_arrow(WHITE, LEFT);
	draw_arrow(WHITE, UP);
}

void draw_arrow(uint32_t color, uint8_t direction) {
	switch (direction) {
		case RIGHT:
			if (color == WHITE) {
				lcd_show_image(150, 100, arrow_white, RIGHT);
			} else {
				lcd_show_image(150, 100, arrow_black, RIGHT);
			}
			break;
		case DOWN:
			if (color == WHITE) {
				lcd_show_image(86, 165, arrow_white, DOWN);
			} else {
				lcd_show_image(86, 165, arrow_black, DOWN);
			}
			break;
		case LEFT:
			if (color == WHITE) {
				lcd_show_image(20, 100, arrow_white, LEFT);
			} else {
				lcd_show_image(20, 100, arrow_black, LEFT);
			}
			break;
		case UP:
			if (color == WHITE) {
				lcd_show_image(86, 35, arrow_white, UP);
			} else {
				lcd_show_image(86, 35, arrow_black, UP);
			}
			break;
		default:
			break;
	}
}

void draw_mode_selection_panel() {
	// draw the mode selection panel
	lcd_fill(0, 285, lcddev.height, lcddev.width, GRAY);

	// draw the arrow button
	uint16_t start_y = 300;
	lcd_draw_line(25, start_y, 35, start_y - 10, BLACK);
	lcd_draw_line(25, start_y, 35, start_y + 10, BLACK);
	lcd_draw_line(lcddev.width - 25, start_y, lcddev.width - 35, start_y - 10, BLACK);
	lcd_draw_line(lcddev.width - 25, start_y, lcddev.width - 35, start_y + 10, BLACK);

	// draw the current mode (defaults to control mode)
	lcd_fill(50, 285 + 4, lcddev.width - 50, lcddev.height - 9, WHITE);
	lcd_draw_rectangle(50, 285 + 4, lcddev.width - 50, lcddev.height - 9, BLACK);
	lcd_show_string(94, 291, 100, 16, 16, "CONTROL", BLACK);
}

void lcd_show_image(uint16_t x, uint16_t y, const uint8_t* image, uint8_t direction) {
	const uint8_t *pointer = image;
	uint16_t width = (*(pointer + 3) << 8) | (*(pointer + 2));
	uint16_t height = (*(pointer + 5) << 8) | (*(pointer + 4));
	pointer += 8;

	switch (direction) {
		// original direction
		case 0:
			for (uint16_t yy = y; yy < y + height; yy++) {
				for (uint16_t xx = x; xx < x + width; xx++) {
					lcd_draw_point(xx, yy, ((*(pointer + 1)) << 8) | (*pointer));
					pointer += 2;
				}
			}
			break;
		// 90 degree clockwise rotation
		case 1:
			for (uint16_t xx = x; xx < x + width; xx++) {
				for (uint16_t yy = y; yy < y + height; yy++) {
					lcd_draw_point(xx, yy, ((*(pointer + 1)) << 8) | (*pointer));
					pointer += 2;
				}
			}
			break;
		// 180 degree clockwise rotation
		case 2:
			for (uint16_t yy = y + height - 1; yy >= y; yy--) {
				for (uint16_t xx = x + width - 1; xx >= x; xx--) {
					lcd_draw_point(xx, yy, ((*(pointer + 1)) << 8) | (*pointer));
					pointer += 2;
				}
			}
			break;
		// 270 degree clockwise rotation
		case 3:
			for (uint16_t xx = x + width - 1; xx >= x; xx--) {
				for (uint16_t yy = y + height - 1; yy >= y; yy--) {
					lcd_draw_point(xx, yy, ((*(pointer + 1)) << 8) | (*pointer));
					pointer += 2;
				}
			}
			break;
		// default: do nothing
		default:
			break;
	}
}
