
// keys
#define KEY0_PRESSED (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET)
#define KEY1_PRESSED (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
#define KEY_WAKEUP_PRESSED (HAL_GPIO_ReadPin(KEY_WAKEUP_GPIO_Port, KEY_WAKEUP_Pin) == GPIO_PIN_SET)

// arrow directions
#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define UP 3

// arrow status
#define RELEASED 0
#define PRESSED 1

// Commands for communication
#define MOVE_FORWARD 1
#define MOVE_BACKWARD 2
#define MOVE_LEFT 3
#define MOVE_RIGHT 4
#define SPIN_LEFT 5
#define SPIN_RIGHT 6
#define BRAKE 7

// Commands for routing
#define ROUTE_UP 8
#define ROUTE_DOWN 9
#define ROUTE_LEFT 10
#define ROUTE_RIGHT 11

// Commands for turning servo joint
#define SERVO_AHEAD 12
#define SERVO_LEFT 13
#define SERVO_RIGHT 14

#define TURN_AROUND_RIGHT 15
#define TURN_AROUND_LEFT 16

#define DETECTION_MODE 17

void HAL_Delay_us(uint32_t us);

void test_communication();

uint8_t nrf_send(uint8_t* txbuf, uint8_t blocking);

void draw_control_panel();

void draw_arrow(uint32_t color, uint8_t direction);

void draw_mode_selection_panel();

void LCD_ShowPicture(uint16_t x,uint16_t y,uint16_t column,uint16_t row,unsigned short *pic);

void lcd_show_image(uint16_t x, uint16_t y, const uint8_t* image, uint8_t direction);

void draw_stop_black();

void draw_stop_red();

void draw_left_spin_black();

void draw_left_spin_red();

void draw_right_spin_black();

void draw_right_spin_red();

void lcd_grid();

void lcd_button();

void mode_control(uint16_t x_touch, uint16_t y_touch, int touched);
