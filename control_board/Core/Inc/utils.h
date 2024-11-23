
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

void HAL_Delay_us(uint32_t us);

void test_communication();

void draw_control_panel();

void draw_arrow(uint32_t color, uint8_t direction);

void draw_mode_selection_panel();

void lcd_show_image(uint16_t x, uint16_t y, const uint8_t* image, uint8_t direction);
