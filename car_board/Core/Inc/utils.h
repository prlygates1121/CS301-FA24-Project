#include <stdint.h>

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

// Joint Angles
#define AHEAD 0
#define LEFT 1
#define RIGHT 2

// 循迹
#define SEARCH_R_PIN       GPIO_PIN_13  // 定义右侧搜索引脚为GPIO的第13引脚
#define SEARCH_R_GPIO      GPIOC        // 定义右侧搜索引脚所在的GPIO组为GPIOC
#define SEARCH_R_IO        HAL_GPIO_ReadPin(SEARCH_R_GPIO, SEARCH_R_PIN)  // 定义一个宏，用于读取右侧搜索引脚的输入数据位

#define SEARCH_L_PIN       GPIO_PIN_11  // 定义左侧搜索引脚为GPIO的第11引脚
#define SEARCH_L_GPIO      GPIOC        // 定义左侧搜索引脚所在的GPIO组同样为GPIOC
#define SEARCH_L_IO        HAL_GPIO_ReadPin(SEARCH_L_GPIO, SEARCH_L_PIN)  // 定义一个宏，用于读取左侧搜索引脚的输入数据位

#define BLACK_AREA         1            // 定义黑色区域的值为1
#define WHITE_AREA         0            // 定义白色区域的值为0

#define COMM_UP 'A'
#define COMM_DOWN 'B'
#define COMM_LEFT 'C'
#define COMM_RIGHT 'D'
#define COMM_STOP 'I'

extern float US_distance;

/* --------------------------------------------- Custom Delay ---------------------------------------------- */

void HAL_Delay_us(uint32_t us);

/* -------------------------------------------- Servo Functions -------------------------------------------- */

void set_joint_angle(float angle);

void turn_joint(uint8_t direction);

/* -------------------------------------- Ultrasonic Sensor Functions -------------------------------------- */

uint32_t get_distance();

/* ---------------------------------------- Communication Functions ---------------------------------------- */

void test_communication();
uint8_t nrf_send(uint8_t* txbuf, uint8_t blocking);
void process_command(uint8_t command[]);

/* -------------------------------------------- Motor Functions -------------------------------------------- */

float get_yaw(void);
void set_speed(uint8_t channel, int32_t speed);
void car_run(int32_t speed, uint32_t ms);
void car_brake();
void car_turn_left(int32_t speed);
void car_turn_right(int32_t speed);
void car_move_right(int32_t speed, uint32_t ms);
void car_move_left(int32_t speed, uint32_t ms);
void car_spin_left(int32_t speed);
void car_spin_right(int32_t speed);
void car_turn_around_right();
void car_turn_around_left();
void car_run_up_one();
void car_run_down_one();
void car_run_left_one();
void car_run_right_one();

/* -------------------------------------------- Routing Functions -------------------------------------------- */
void SearchRun(void);
