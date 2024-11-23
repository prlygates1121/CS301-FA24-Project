/* ------------------------------------------------ Macros ------------------------------------------------ */

// Buttons
#define KEY1_PRESSED (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
#define KEY2_PRESSED (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)

// LEDs
#define TURN_ON_LED1 if (HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin) == GPIO_PIN_SET) HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET)
#define TURN_OFF_LED1 if (HAL_GPIO_ReadPin(LED1_GPIO_Port, LED1_Pin) == GPIO_PIN_RESET) HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET)
#define TOGGLE_LED1 HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin)
#define TURN_ON LED2 if (HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin) == GPIO_PIN_SET) HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET)
#define TURN_OFF_LED2 if (HAL_GPIO_ReadPin(LED2_GPIO_Port, LED2_Pin) == GPIO_PIN_RESET) HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET)
#define TOGGLE_LED2 HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin)

// Detect obstacles
#define IR_R_HAS_OBSTACLE (HAL_GPIO_ReadPin(IR_OBST_R_GPIO_Port, IR_OBST_R_Pin) == GPIO_PIN_RESET)
#define IR_L_HAS_OBSTACLE (HAL_GPIO_ReadPin(IR_OBST_L_GPIO_Port, IR_OBST_L_Pin) == GPIO_PIN_RESET)

// Detect ground color
#define IR_R_GROUND_WHITE (HAL_GPIO_ReadPin(IR_GND_R_GPIO_Port, IR_GND_R_Pin) == GPIO_PIN_RESET)
#define IR_L_GROUND_WHITE (HAL_GPIO_ReadPin(IR_GND_L_GPIO_Port, IR_GND_L_Pin) == GPIO_PIN_RESET)
#define IR_R_GROUND_BLACK (HAL_GPIO_ReadPin(IR_GND_R_GPIO_Port, IR_GND_R_Pin) == GPIO_PIN_SET)
#define IR_L_GROUND_BLACK (HAL_GPIO_ReadPin(IR_GND_L_GPIO_Port, IR_GND_L_Pin) == GPIO_PIN_SET)

// Detect ultrasonic waves
#define US_NOT_RECEIVED (HAL_GPIO_ReadPin(US_STATUS_GPIO_Port, US_STATUS_Pin) == GPIO_PIN_SET)

// Toggle buzzer
#define TURN_ON_BUZZER if (HAL_GPIO_ReadPin(BUZZER_GPIO_Port, BUZZER_Pin) == GPIO_PIN_SET) HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)
#define TURN_OFF_BUZZER if (HAL_GPIO_ReadPin(BUZZER_GPIO_Port, BUZZER_Pin) == GPIO_PIN_RESET) HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)

extern float US_distance;

/* --------------------------------------------- Custom Delay ---------------------------------------------- */

void HAL_Delay_us(uint32_t us);

/* -------------------------------------------- Servo Functions -------------------------------------------- */

void set_joint_angle(float angle);

/* -------------------------------------- Ultrasonic Sensor Functions -------------------------------------- */

// measure distance via ultrasonic module
uint16_t UT_get_distance();

/* ---------------------------------------- Communication Functions ---------------------------------------- */

void test_communication();

/* -------------------------------------------- Motor Functions -------------------------------------------- */

void set_speed(uint8_t channel, int32_t speed);
void car_run(int8_t speed);
void car_brake();
void car_turn_left(int8_t speed);
void car_turn_right(int8_t speed);
void car_spin_left(int8_t speed);
void car_spin_right(int8_t speed);
