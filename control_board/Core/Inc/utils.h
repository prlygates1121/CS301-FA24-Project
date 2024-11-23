#define KEY0_PRESSED (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == GPIO_PIN_RESET)
#define KEY1_PRESSED (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
#define KEY_WAKEUP_PRESSED (HAL_GPIO_ReadPin(KEY_WAKEUP_GPIO_Port, KEY_WAKEUP_Pin) == GPIO_PIN_SET)

void HAL_Delay_us(uint32_t us);

void test_communication();
