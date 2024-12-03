#include <stdint.h>
#include "utils.h"
#include "24l01.h"
#include "lcd_v4.h"
#include "images.h"
#include "tim.h"
#include "main.h"
#include "stdbool.h"

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
	//发送
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

//lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t column,uint16_t row,unsigned short *pic)
{
	uint16_t m,h;
	uint16_t *data=(uint16_t *)pic;
	for(h=0+y;h<row+y;h++) //60
	{
		for(m=0+x;m<column+x;m++) //180
		{
			lcd_draw_point(m,h,*data++);
		}
	}

}

void Draw_Big_Point(uint16_t x, uint16_t y, uint32_t color)
{
	//lcd_draw_point(uint16_t x, uint16_t y, uint32_t color)
//	POINT_COLOR=RED;
	lcd_draw_point(x,y, RED);//���ĵ�
	lcd_draw_point(x+1,y, RED);
	lcd_draw_point(x,y+1, RED);
	lcd_draw_point(x+1,y+1, RED);
}


void draw_control_panel() {
	if (mode == 0){
		lcd_fill(0, 0, 240, 288, WHITE);
		draw_arrow(WHITE, RIGHT);
		draw_arrow(WHITE, DOWN);
		draw_arrow(WHITE, LEFT);
		draw_arrow(WHITE, UP);
		draw_stop_black();
		draw_left_spin_black();
		draw_right_spin_black();
	}else if (mode == 1){
		lcd_fill(0, 0, 240, 288, WHITE);
		control = 0;
		lcd_grid();
		lcd_button();
	}else if (mode == 2){
		lcd_fill(0, 0, 240, 288, WHITE);
		lcd_grid();
	}

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

//stop
void draw_stop_black(){
	//LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t column,uint16_t row,unsigned short *pic)
	LCD_ShowPicture(101, 115, 40, 37, stop_black);
}

void draw_stop_red(){
	LCD_ShowPicture(101, 115, 39, 40, stop_red);
}

//left spin
void draw_left_spin_black(){
	LCD_ShowPicture(35, 50, 40, 38, left_black);
}

void draw_left_spin_red(){
	LCD_ShowPicture(35, 50, 39, 40, left_red);
}

//right spin
void draw_right_spin_black(){
	LCD_ShowPicture(165, 50, 40, 38, right_black);
}

void draw_right_spin_red(){
	LCD_ShowPicture(165, 50, 39, 40, right_red);
}

void lcd_grid(){
	//lcd_fill(uint16_t sx, uint16_t sy, uint16_t ex, uint16_t ey, uint32_t color)
	lcd_fill(20, 10, 220, 210, WHITE);
	//lcd_draw_rectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
	lcd_draw_rectangle(20, 10, 220, 210, BLACK);
	//void lcd_draw_line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
	//横着
	lcd_draw_line(20, 60, 220, 60, BLACK);
	lcd_draw_line(20, 110, 220, 110, BLACK);
	lcd_draw_line(20, 160, 220, 160, BLACK);

	//竖着
	lcd_draw_line(70, 10, 70, 210, BLACK);
	lcd_draw_line(120, 10, 120, 210, BLACK);
	lcd_draw_line(170, 10, 170, 210, BLACK);
}

void lcd_button(){
	lcd_fill(0, 220, 240, 288, WHITE);
	lcd_draw_rectangle(10, 230, 57, 270, BLACK);
	lcd_show_string(18, 244, 47, 12, 12, "START", BLACK);

	lcd_draw_rectangle(67, 230, 114, 270, BLACK);
	lcd_show_string(82, 244, 47, 12, 12, "END", BLACK);

	lcd_draw_rectangle(124, 230, 171, 270, BLACK);
	lcd_show_string(134, 244, 47, 12, 12, "TRAP", BLACK);

	lcd_draw_rectangle(181, 230, 228, 270, BLACK);
	lcd_show_string(190, 244, 47, 12, 12, "ROUTE", BLACK);

}

void lcd_update_endpoint(int i, int j){
	LCD_ShowPicture(20+(50*i)+2, 10+(50*j)+2, 40, 45, endpoint);
}

void mode_switch_panel(uint16_t x_touch, uint16_t y_touch){
	if ((x_touch > 0 && x_touch < 240 && y_touch > 290 && y_touch < 320)){
		if (x_touch > 0 && x_touch < 120 && y_touch > 290 && y_touch < 320){
			//left
			if (mode != 0){
				mode--;
			}
			sprintf(DATA_TO_SEND, "press left\n");
			HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
		}
		if (x_touch > 120 && x_touch < 240 && y_touch > 290 && y_touch < 320){
			//right
			if (mode != 2){
				mode++;
			}
			sprintf(DATA_TO_SEND, "press right\n");
			HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
		}
//lcd_draw_rectangle(50, 285 + 4, lcddev.width - 50, lcddev.height - 9, BLACK);
		draw_control_panel();
		switch(mode){
			case 0:
				lcd_fill(52, 291, 188, 310, WHITE);
				lcd_show_string(94, 291, 100, 16, 16, "CONTROL", BLACK);
				break;
			case 1:
				control = 0;
				lcd_fill(52, 291, 188, 310, WHITE);
				lcd_show_string(94, 291, 100, 16, 16, "ROUTE", BLACK);
				break;
			case 2:
				lcd_fill(52, 291, 188, 310, WHITE);
				lcd_show_string(94, 291, 100, 16, 16, "AUTO", BLACK);
				break;
		}
	}

}

void mode_control(uint16_t x_touch, uint16_t y_touch, int touched){
	if(touched){
		//screen being touched
		//stop
		  if (x_touch > 86 && x_touch < 156 && y_touch > 100 && y_touch < 170){
			  if (arrow_status[4] == RELEASED){
				  draw_stop_red();
				  //TODO command[0] = MOVE_RIGHT;
				  //NRF24L01_TxPacket(command);
				  arrow_status[4] = PRESSED;
			  }
		  } else {
			  if (arrow_status[4] == PRESSED) {
				  draw_stop_black();
				  arrow_status[4] = RELEASED;
			  }
		  }
		  //left spin
		  if (x_touch > 20 && x_touch < 90 && y_touch > 35 && y_touch < 105){
			  if (arrow_status[5] == RELEASED){
				  draw_left_spin_red();
				  //TODO command[0] = MOVE_RIGHT;
				  //NRF24L01_TxPacket(command);
				  arrow_status[5] = PRESSED;
			  }
		  } else {
			  if (arrow_status[5] == PRESSED) {
				  draw_left_spin_black();
				  arrow_status[5] = RELEASED;
			  }
		  }
		  //right spin
		  if (x_touch > 150 && x_touch < 220 && y_touch > 35 && y_touch < 105){
			  if (arrow_status[6] == RELEASED){
				  draw_right_spin_red();
				  //TODO command[0] = MOVE_RIGHT;
				  //NRF24L01_TxPacket(command);
				  arrow_status[6] = PRESSED;
			  }
		  } else {
			  if (arrow_status[6] == PRESSED) {
				  draw_right_spin_black();
				  arrow_status[6] = RELEASED;
			  }
		  }
		  // if the touch is on the right arrow
		  if (x_touch > 150 && x_touch < 220 && y_touch > 100 && y_touch < 170) {
			  if (arrow_status[0] == RELEASED) {
				  draw_arrow(BLACK, RIGHT);
//				  command[0] = MOVE_RIGHT;
//					  NRF24L01_TxPacket(command);
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
//				  command[0] = MOVE_BACKWARD;
//					  NRF24L01_TxPacket(command);
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
//				  command[0] = MOVE_LEFT;
//					  NRF24L01_TxPacket(command);
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
//				  command[0] = MOVE_FORWARD;
//					  NRF24L01_TxPacket(command);
				  arrow_status[3] = PRESSED;
			  }
		  } else {
			  if (arrow_status[3] == PRESSED) {
				  draw_arrow(WHITE, UP);
				  arrow_status[3] = RELEASED;
			  }
		  }
	}else{
		//screen untouched
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
		  if (arrow_status[4] == PRESSED){
			  draw_stop_black();
			  arrow_status[4] = RELEASED;
		  }
		  if (arrow_status[5] == PRESSED){
			  draw_left_spin_black();
			  arrow_status[5] = RELEASED;
		  }
		  if (arrow_status[6] == PRESSED){
			  draw_right_spin_black();
			  arrow_status[6] = RELEASED;
		  }
		  // send BRAKE command to the car board if BRAKE has not been sent
//		  if (command[0] != BRAKE) {
//			  HAL_Delay(100);
//			  command[0] = BRAKE;
//			  NRF24L01_TxPacket(command);
//		  }
	}
}

void mode_route(uint16_t x_touch, uint16_t y_touch, int touched){
	if (touched){
		//screen is touched
		// button panel
		if (x_touch > 10 && x_touch < 57 && y_touch > 230 && y_touch < 270){
			//start point  endpoint
			if (start_point == 0){//0:empty 1:choosed 2:setted
				start_point = 1;
				sprintf(DATA_TO_SEND, "select start\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			}
		}
		if (x_touch > 67 && x_touch < 114 && y_touch > 230 && y_touch < 270){
			//end point  endpoint
			if (end_point == 0){//0:empty 1:choosed 2:setted
				end_point = 1;
				sprintf(DATA_TO_SEND, "select end\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			}
		}
		if (x_touch > 124 && x_touch < 171 && y_touch > 230 && y_touch < 270){
			//trap  obstacle
			if (trap_point == 0){//0:empty 1:choosed 2:setted
				trap_point = 1;
				sprintf(DATA_TO_SEND, "select trap\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			}
		}
		if (x_touch > 181 && x_touch < 228 && y_touch > 230 && y_touch < 270){
			//move the car  0:still 1:move   -> changed to key1
			//NOW: route
			if (control == 0){
				control = 1;
				sprintf(DATA_TO_SEND, "start routing\n");
				HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			}
//			else if (control == 1){
//				control = 0;
//			}
		}
		// grid panel
		int x_index = 0;
		int y_index = 0;
		int index = 0;
		if (x_touch > 20 && x_touch < 220 && y_touch > 20 && y_touch < 220){

			switch(control){
				case 0://start end trap
					x_index = (x_touch-20) / 50;
					y_index = (y_touch-20) / 50;
					index = (4*y_index) + x_index;
					//grid status: 0-空 1-path 2-start 3-end 4-trap
					if(grid_status[index] == 0){//empty
						if (debounceLabel == 0){
							debounceLabel = 1;
							debounceTime = HAL_GetTick();
						}else{
							if (HAL_GetTick() - debounceTime > 100){
								debounceLabel = 0;
								if (start_point == 1){
									grid_status[index] = 2;
									LCD_ShowPicture(20+(50*x_index)+2, 10+(50*y_index)+2, 40, 45, endpoint);
									sprintf(DATA_TO_SEND, "start press\n");
									HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
								}else if(end_point == 1){
									grid_status[index] = 3;
									LCD_ShowPicture(20+(50*x_index)+2, 10+(50*y_index)+2, 40, 45, endpoint);
									sprintf(DATA_TO_SEND, "end press\n");
									HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
								}else if(trap_point == 1){
									grid_status[index] = 4;
									LCD_ShowPicture(20+(50*x_index)+2, 10+(50*y_index)+2, 38, 45, obstacle);
									sprintf(DATA_TO_SEND, "trap press\n");
									HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
								}
							}
						}
//						}
					}else{//
						if (debounceLabel == 0){
							debounceLabel = 1;
							debounceTime = HAL_GetTick();
						}else{
							if (HAL_GetTick() - debounceTime > 100){
								debounceLabel = 0;
								grid_status[index] = 0;
								lcd_fill( 20+(50*x_index)+2, 10+(50*y_index)+2, 20+(50*x_index)+2+45, 10+(50*y_index)+2+45, WHITE);
								sprintf(DATA_TO_SEND, "this grid clear\n");
								HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
							}

						}

					}
					break;
				case 1://routing
					Draw_Big_Point(x_touch,y_touch,RED);//画图
					if (!signal){
						sprintf(DATA_TO_SEND, "start painting\n");
						HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
					}
					signal = 1;
					x_index = (x_touch-20) / 50;
					y_index = (y_touch-20) / 50;
					uint32_t currentTime = HAL_GetTick();
					//touch on index(th) grid
					index = (4*y_index) + x_index;
					if(grid_status[index]==0 || grid_status[index]==2 || grid_status[index]==3){
						if(grid_status[index] == 0){
							grid_status[index] = 1;
						}
						//0 up / 1 down / 2 left / 3 right
						if (last_x != -1 && last_y != -1){
							lastTime = currentTime;
							if (x_index == last_x && y_index > last_y){
								//down
								path[pnt] = ROUTE_DOWN;
								pnt++;
							}else if (x_index == last_x && y_index < last_y){
								//up
								path[pnt] = ROUTE_UP;
								pnt++;
							}else if (x_index < last_x && y_index == last_y){
								//left
								path[pnt] = ROUTE_LEFT;
								pnt++;
							}else if (x_index > last_x && y_index == last_y){
								//right
								path[pnt] = ROUTE_RIGHT;
								pnt++;
							}
						}
						last_x = x_index;
						last_y = y_index;
					}
//					else if(grid_status[index] == 1){
//						grid_status[index] = 0;
//						//0 up / 1 down / 2 left / 3 right
//						for (int i = 0; i < 16; i++) {
//							path[i] = -1;
//						}
//						lcd_fill( 20+(50*x_index)+2, 10+(50*y_index)+2, 20+(50*x_index)+2+45, 10+(50*y_index)+2+45, WHITE);
//						last_x = -1;
//						last_y = -1;
//					}
					break;
			}
		  }
	}else{
		//screen untouched
		if(move_signal){
			move_signal = 0;
			start_point = 0;
			end_point = 0;
			trap_point = 0;
			control = 0;

			bool b = (HAL_GetTick() - lastTime) < time_threshold;
			  if (signal && !b){//路线画完了
				  sprintf(DATA_TO_SEND, "finish painting\n");
				  HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
				  signal = 0;
				  //transmit path
				  for(int j = 0; j < 16; j++){
					  if (path[j] != 255){
						  sprintf(DATA_TO_SEND, "%u", path[j]);
						  HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
					  }
				  }
				  lcd_grid();
				  pnt = 0;
				  for (int i = 0; i < 16; i++) {
						path[i] = -1;
				  }
				  for (int i = 0; i < 16; i++) {
					  grid_status[i] = 0;
				  }
			  }
		}

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
	switch(mode){
	case 0:
		lcd_show_string(94, 291, 100, 16, 16, "CONTROL", BLACK);
		break;
	case 1:
		lcd_show_string(94, 291, 100, 16, 16, "ROUTE", BLACK);
		break;
	case 2:
		lcd_show_string(94, 291, 100, 16, 16, "AUTO", BLACK);
		break;
	}
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


