#define IR_R_HAS_OBSTACLE (HAL_GPIO_ReadPin(IR_OBST_R_GPIO_Port, IR_OBST_R_Pin) == GPIO_PIN_RESET)
#define IR_L_HAS_OBSTACLE (HAL_GPIO_ReadPin(IR_OBST_L_GPIO_Port, IR_OBST_L_Pin) == GPIO_PIN_RESET)
#define IR_R_GROUND_WHITE (HAL_GPIO_ReadPin(IR_GND_R_GPIO_Port, IR_GND_R_Pin) == GPIO_PIN_RESET)
#define IR_L_GROUND_WHITE (HAL_GPIO_ReadPin(IR_GND_L_GPIO_Port, IR_GND_L_Pin) == GPIO_PIN_RESET)
#define IR_R_GROUND_BLACK (HAL_GPIO_ReadPin(IR_GND_R_GPIO_Port, IR_GND_R_Pin) == GPIO_PIN_SET)
#define IR_L_GROUND_BLACK (HAL_GPIO_ReadPin(IR_GND_L_GPIO_Port, IR_GND_L_Pin) == GPIO_PIN_SET)
#define UT_NOT_RECEIVED (HAL_GPIO_ReadPin(UT_STATUS_GPIO_Port, UT_STATUS_Pin) == GPIO_PIN_SET)

#define TURN_ON_BUZZER if (HAL_GPIO_ReadPin(BUZZER_GPIO_Port, BUZZER_Pin) == GPIO_PIN_SET) HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)
#define TURN_OFF_BUZZER if (HAL_GPIO_ReadPin(BUZZER_GPIO_Port, BUZZER_Pin) == GPIO_PIN_RESET) HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)

extern float UT_distance;

// microsecond delay function
void HAL_Delay_us(uint32_t us);

// set the angle of the servo motor
void set_joint_angle(float angle);

// measure distance via ultrasonic module
uint16_t UT_get_distance();


uint16_t UT_get_stable_distance();
