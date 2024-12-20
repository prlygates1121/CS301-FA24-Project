#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "24l01.h"
#include "tim.h"
#include "inv_mpu.h"

extern UART_HandleTypeDef huart1;
extern uint8_t nrf_tx_buf[32];
char DATA_TO_SEND[800];



/* --------------------------------------------- Custom Delay -------------------------------------------- */

void HAL_Delay_us(uint32_t us) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}

/* ------------------------------------------- Servo Functions ------------------------------------------- */

void set_joint_angle(float angle)
{
	angle = (uint16_t)(50.0 * angle / 9.0 + 249.0);
	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, angle);
}

void turn_joint(uint8_t direction) {
	switch (direction) {
		case AHEAD:
			set_joint_angle(82);
			break;
		case LEFT:
			set_joint_angle(182);
			break;
		case RIGHT:
			set_joint_angle(3);
			break;
		default:
			break;
	}
	HAL_Delay(500);
}

/* ------------------------------------- Ultrasonic Sensor Functions ------------------------------------- */

uint32_t get_distance() {
	HAL_GPIO_WritePin(US_SEND_GPIO_Port, US_SEND_Pin, GPIO_PIN_SET);
	HAL_Delay_us(200);
	HAL_GPIO_WritePin(US_SEND_GPIO_Port, US_SEND_Pin, GPIO_PIN_RESET);
	return (US_distance * 2 > 255) ? 255 : (US_distance * 2);
}

/* --------------------------------------- Communication Functions --------------------------------------- */

void test_communication() {
	uint8_t test_data_received[32];
	uint16_t t = 0;
	// Receive test data from the control board
	NRF24L01_RX_Mode();
	while (1) {
	  if (NRF24L01_RxPacket(test_data_received) == 0) {
		  uint8_t equal = 1;
		  for (uint8_t i = 0; i < 32; i++) {
			  if (test_data_received[i] != i) {
				  equal = 0;
				  break;
			  }
		  }
		  if (equal == 1) {
			  break;
		  }
	  }
	  t++;
	  if (t == 10000) {
		  t = 0;
		  TOGGLE_LED1;
	  }
	}
	TURN_ON_LED1;
	// Send back the received data
	NRF24L01_TX_Mode();
	NRF24L01_TxPacket(test_data_received);
	TURN_OFF_LED1;
}

uint8_t nrf_send(uint8_t* txbuf, uint8_t blocking) {
	uint8_t return_val;
	NRF24L01_TX_Mode();
	if (blocking == 0) {
		return_val = NRF24L01_TxPacket(txbuf);
	} else {
		while (NRF24L01_TxPacket(txbuf) != TX_OK);
		return_val = TX_OK;
	}
	NRF24L01_RX_Mode();
	return return_val;
}

//int ground[4][4] = {
//    {0, 1, 0, 0},
//    {0, 1, 1, 0},
//    {0, 1, 0, 0},
//    {0, 0, 0, 1},
//};
int graph[4][4] = {
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
};

int start[2];
int end[2];
uint8_t point[2];											//小车坐标
int total_obstacle = 5;
int obstacle_cnt = 0;
int clear_cnt = 1;
int step[4][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1},};	//右、左、后、前
uint32_t dist[1];

//模拟探路，实操改为探测前、左、右，并将后方坐标无条件加入返回数组
void detect(int x, int y, uint8_t next_step[]) {
	uint8_t detect_command[32] = {0};
	// 探测四个方向有无障碍
	for(int i=0; i<4; i++) {
		next_step[i] = 0;
		int x_d = x + step[i][0];
		int y_d = y + step[i][1];
		if (x_d < 0 || x_d >= 4 || y_d < 0 || y_d >= 4) {
			//越界
			continue;
		}
		//没探测过
		if (!graph[x_d][y_d]) {
			switch (i) {
				// 看右边
				case 0:
					turn_joint(RIGHT);
					break;
				// 看左边
				case 1:
					turn_joint(LEFT);
					break;
				// 转过头看后边
				case 2:
					turn_joint(AHEAD);
					car_turn_around_right();
					break;
				// 看前边
				case 3:
					turn_joint(AHEAD);
					break;
				default:
					break;
			}
			for (uint8_t i = 0; i < 3; i++) {
				HAL_Delay(1000);
				dist[0] = get_distance();
			}
			nrf_tx_buf[0] = 0;
			nrf_tx_buf[1] = dist[0];
			nrf_send(nrf_tx_buf, 0);
			if (dist[0] <= 90) {
				//有障碍
				TURN_ON_BUZZER;
				graph[x_d][y_d] = 1;
//				LCD_ShowPicture(20+(50*x_d)+2, 10+(50*y_d)+2, 38, 45, obstacle);
				detect_command[0] = 1;
				detect_command[1] = x_d;
				detect_command[2] = y_d;
				nrf_send(detect_command, 0);
				obstacle_cnt++;
				HAL_Delay(200);
				TURN_OFF_BUZZER;
			} else {
				//无障碍
				graph[x_d][y_d] = 2;
//				LCD_ShowPicture(20+(50*x_d)+2, 10+(50*y_d)+2, 39, 40, left_red);
				detect_command[0] = 2;
				detect_command[1] = x_d;
				detect_command[2] = y_d;
				nrf_send(detect_command, 0);
				clear_cnt++;
				next_step[i] = 1;
			}
			if (i == 2) {
				car_turn_around_left();
			}
		}
	}
}

uint8_t current_pos[2];

uint8_t dfs(int x, int y) {
	uint8_t dfs_command[32] = {0};
	uint8_t to_end = 0; //是否能到终点

	uint8_t next_step[4] = {0, 0, 0, 0};//记录右、左、后、前是否可以走
	detect(x, y, next_step);

	for (int i=0; i<4; i++) {
		if (next_step[i]) {
			//可以走，就走
			point[0] = x + step[i][0];
			point[1] = y + step[i][1];

			switch (i) {
				// 往右走一格
				case 0:
					car_run_right_one();
					break;
				// 往左走一格
				case 1:
					car_run_left_one();
					break;
				// 往后走一格
				case 2:
					car_run_down_one();
					break;
				// 往前走一格
				case 3:
					car_run_up_one();
					break;
				default:
					break;
			}
			sprintf(DATA_TO_SEND, "DFS dives deeper: (%d, %d)\n", x + step[i][0], y + step[i][1]);
			HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			dfs_command[0] = 3;
			dfs_command[1] = point[0];
			dfs_command[2] = point[1];
			nrf_send(dfs_command, 0);
			if (dfs(point[0], point[1])) {
				to_end = 1;
			}
			switch (i) {
				// 往左回一格
				case 0:
					car_run_left_one();
					break;
				// 往右回一格
				case 1:
					car_run_right_one();
					break;
				// 往前回一格
				case 2:
					car_run_up_one();
					break;
				// 往后回一格
				case 3:
					car_run_down_one();
					break;
				default:
					break;
			}
			point[0] = x;
			point[1] = y;
			sprintf(DATA_TO_SEND, "DFS floats up: (%d, %d)\n", x, y);
			HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			dfs_command[0] = 3;
			dfs_command[1] = point[0];
			dfs_command[2] = point[1];
			nrf_send(dfs_command, 0);
		}
	}
	to_end |= (x == end[0]) && (y == end[1]);
	if ((x == end[0]) && (y == end[1])) {
		if (obstacle_cnt == total_obstacle) {
			//到终点时找全了,就停着不走了
			for (int x_show=0; x_show<4; x_show++) {
				for (int y_show=0; y_show<4; y_show++) {
					if (graph[x_show][y_show] == 1) {
						dfs_command[0] = 1;
						dfs_command[1] = x_show;
						dfs_command[2] = y_show;
						nrf_send(dfs_command, 0);
						HAL_Delay(500);
					}
				}
			}
			HAL_Delay(100000);
		} else if (clear_cnt == 16 - total_obstacle) {
			//到终点时找全了,就停着不走了
			for (int i=0; i<4; i++) {
				for (int j=0; j<4; j++) {
					if (!graph[i][j]) {
						graph[i][j] = 1;
					}
				}
			}
			for (int x_show=0; x_show<4; x_show++) {
				for (int y_show=0; y_show<4; y_show++) {
					if (graph[x_show][y_show] == 1) {
//						LCD_ShowPicture(20+(50*x_show)+2, 10+(50*y_show)+2, 38, 45, obstacle);
						dfs_command[0] = 1;
						dfs_command[1] = x_show;
						dfs_command[2] = y_show;
						nrf_send(dfs_command, 0);
						HAL_Delay(500);
					}
				}
			}
			HAL_Delay(100000);
		}
	}
	if (to_end) {
		graph[x][y] = 3;
	}
	return to_end;
}

void go_through(int x, int y) {
	uint8_t go_command[32] = {0};

	for (int i=0; i<4; i++) {
		int x_d = x + step[i][0];
		int y_d = y + step[i][1];
		//越界
		if (x_d < 0 || x_d >= 4 || y_d < 0 || y_d >= 4) {
			continue;
		}
		//如果这条路通向终点
		if (graph[x_d][y_d] == 3) {
			graph[x][y] = 4;
			point[0] = x_d;
			point[1] = y_d;
			//走
			switch (i) {
				// 往右走一格
				case 0:
					car_run_right_one();
					break;
				// 往左走一格
				case 1:
					car_run_left_one();
					break;
				// 往后走一格
				case 2:
					car_run_down_one();
					break;
				// 往前走一格
				case 3:
					car_run_up_one();
					break;
				default:
					break;
			}
			sprintf(DATA_TO_SEND, "On my way to the end: (%d, %d)\n", x_d, y_d);
			HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			go_command[0] = 4;
			go_command[1] = point[0];
			go_command[2] = point[1];
			nrf_send(go_command, 0);
			//如果已经到了终点
			if (x_d == end[0] && y_d == end[1]) {
				if (obstacle_cnt != total_obstacle) {
					for (int k=0; k<4; k++) {
						for (int j=0; j<4; j++) {
							if (!graph[k][j]) {
								graph[k][j] = 1;
							}
						}
					}
				}
				for (int x_show=0; x_show<4; x_show++) {
					for (int y_show=0; y_show<4; y_show++) {
						if (graph[x_show][y_show] == 1) {
//							LCD_ShowPicture(20+(50*x_show)+2, 10+(50*y_show)+2, 38, 45, obstacle);
							go_command[0] = 1;
							go_command[1] = x_show;
							go_command[2] = y_show;
							nrf_send(go_command, 0);
							HAL_Delay(500);
						}
					}
				}
//				HAL_Delay(100000);
			} else {
				go_through(x_d, y_d);
			}
		}
	}
}

void process_command(uint8_t command[]) {
	switch (command[0]) {
		case MOVE_FORWARD:
			car_run(65, 500);
			car_brake();
			break;
		case MOVE_BACKWARD:
			car_run(-65, 500);
			car_brake();
			break;
		case MOVE_LEFT:
			car_move_left(65, 500);
			car_brake();
			break;
		case MOVE_RIGHT:
			car_move_right(65, 500);
			car_brake();
			break;
		case SPIN_LEFT:
			car_spin_left(55);
			HAL_Delay(1120);
			car_brake();
			break;
		case SPIN_RIGHT:
			car_spin_right(55);
			HAL_Delay(1120);
			car_brake();
		case BRAKE:
			car_brake();
			break;
		case SERVO_AHEAD:
			turn_joint(AHEAD);
			HAL_Delay(500);
			break;
		case SERVO_LEFT:
			turn_joint(LEFT);
			HAL_Delay(500);
			break;
		case SERVO_RIGHT:
			turn_joint(RIGHT);
			HAL_Delay(500);
			break;
		case TURN_AROUND_RIGHT:
			car_turn_around_right();
			break;
		case TURN_AROUND_LEFT:
			car_turn_around_left();
			break;
		case DETECTION_MODE:
			start[0] = command[1];
			start[1] = command[2];
			end[0] = command[3];
			end[1] = command[4];
			point[0] = start[0];
			point[1] = start[1];
			graph[start[0]][start[1]] = 2;
			sprintf(DATA_TO_SEND, "Starting at (%d, %d) to the end (%d, %d)\n", start[0], start[1], end[0], end[1]);
			HAL_UART_Transmit(&huart1, (uint8_t*)DATA_TO_SEND, strlen(DATA_TO_SEND), HAL_MAX_DELAY);
			dfs(start[0], start[1]);
			go_through(start[0], start[1]);
			break;
		default:
			for (uint8_t i = 0; i < 16; i++) {
				if (command[i] == -1) {
					break;
				}
				switch (command[i]) {
					case ROUTE_UP:
						car_run_up_one();
						break;
					case ROUTE_DOWN:
						car_run_down_one();
						break;
					case ROUTE_LEFT:
						car_run_left_one();
						break;
					case ROUTE_RIGHT:
						car_run_right_one();
						break;
				}
				HAL_Delay(500);
			}
			break;
	}
}


/* -------------------------------------------- Motor Functions ------------------------------------------- */

// Definitions
#define PWM_BASE_VALUE 7201
#define SPEED_SCALE 72
#define MAX_SPEED 100
#define MIN_SPEED -100

typedef struct {
    float Kp;
    float Ki;
    float Kd;
    float previous_error;
    float integral;
} PID_Controller;

// Global Variables
// PID_Controller pid_yaw = {2.f, 0.0f, -0.5f, 0.0f, 0.0f};
PID_Controller pid_yaw = {2.f, 0.f, -0.5f, 0.0f, 0.0f};
float stabilization_yaw = 0.0f;
float desired_yaw = 0.0f;

// Function Prototypes
float PID_Update(PID_Controller *pid, float setpoint, float measured, float dt);
void set_speed(uint8_t channel, int32_t speed);
void setup(void);

float get_yaw(void)
{
    float yaw;
    while (atk_ms6050_dmp_get_data(&yaw) != 0);
    return yaw;
}

float PID_Update(PID_Controller *pid, float setpoint, float measured, float dt)
{
    float error = setpoint - measured;
    pid->integral += error * dt;
    float derivative = (error - pid->previous_error) / dt;
    pid->previous_error = error;
    return (pid->Kp * error) + (pid->Ki * pid->integral) + (pid->Kd * derivative);
}

void set_speed(uint8_t channel, int32_t speed)
{
    uint16_t pwm_value;
    // Limit speed range to [-100, 100]
    if (speed > 100) speed = 100;
    if (speed < -100) speed = -100;

    // Set PWM duty cycle and direction based on channel
    switch (channel)
    {
        case 1: // Left rear
            if (speed > 0) speed = 101 - speed;
            pwm_value = (speed >= 0) ? (7201 - speed * 72) : (7201 + speed * 72);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_value);
            HAL_GPIO_WritePin(MOTOR_DIR_FRONT_L_GPIO_Port, MOTOR_DIR_FRONT_L_Pin, speed >= 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case 2: // Right front
        	if (speed < 0) speed = -101 - speed;
            pwm_value = (speed >= 0) ? (7201 - speed * 72) : (7201 + speed * 72);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm_value);
            HAL_GPIO_WritePin(MOTOR_DIR_FRONT_R_GPIO_Port, MOTOR_DIR_FRONT_R_Pin, speed >= 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
            break;

        case 3: // Left front
            if (speed > 0) speed = 101 - speed;
            pwm_value = (speed >= 0) ? (7201 - speed * 72) : (7201 + speed * 72);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, pwm_value);
            HAL_GPIO_WritePin(MOTOR_DIR_REAR_L_GPIO_Port, MOTOR_DIR_REAR_L_Pin, speed >= 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case 4: // Right rear
        	if (speed < 0) speed = -101 - speed;
            pwm_value = (speed >= 0) ? (7201 - speed * 72) : (7201 + speed * 72);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, pwm_value);
            HAL_GPIO_WritePin(MOTOR_DIR_REAR_R_GPIO_Port, MOTOR_DIR_REAR_R_Pin, speed >= 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
            break;

        default:
            break;
    }
}

#define INTERVAL 300.0f;

void car_run(int32_t speed, uint32_t ms)
{
	uint32_t start_time = HAL_GetTick();
	uint32_t last_time = start_time;
	const float control_interval = INTERVAL; // Control loop interval in milliseconds

	while (1)
	{
		uint32_t current_time = HAL_GetTick();
		float elapsed_time = current_time - last_time;
		if (current_time - start_time >= ms) {
			car_brake();
			return;
		}
		if (elapsed_time >= control_interval)
		{
			last_time = current_time;

			float yaw = get_yaw();

			// 2. Compute PID output
			float dt = control_interval / 1000.0f; // Convert ms to seconds
			stabilization_yaw = PID_Update(&pid_yaw, desired_yaw, yaw, dt);

			// 3. Execute movement command with stabilization
			// Example: Move forward at speed 50
		    // Base speed commands
		    int32_t base_speed_front_left = -speed;
		    int32_t base_speed_front_right = speed;
		    int32_t base_speed_rear_left = -speed;
		    int32_t base_speed_rear_right = speed;

		    // Apply yaw stabilization adjustments
		    base_speed_front_left += stabilization_yaw;
		    base_speed_rear_left += stabilization_yaw;
		    base_speed_front_right += stabilization_yaw;
		    base_speed_rear_right += stabilization_yaw;

		    // Limit speeds to [MIN_SPEED, MAX_SPEED]
		    base_speed_front_left = (base_speed_front_left > MAX_SPEED) ? MAX_SPEED : ((base_speed_front_left < MIN_SPEED) ? MIN_SPEED : base_speed_front_left);
		    base_speed_front_right = (base_speed_front_right > MAX_SPEED) ? MAX_SPEED : ((base_speed_front_right < MIN_SPEED) ? MIN_SPEED : base_speed_front_right);
		    base_speed_rear_left = (base_speed_rear_left > MAX_SPEED) ? MAX_SPEED : ((base_speed_rear_left < MIN_SPEED) ? MIN_SPEED : base_speed_rear_left);
		    base_speed_rear_right = (base_speed_rear_right > MAX_SPEED) ? MAX_SPEED : ((base_speed_rear_right < MIN_SPEED) ? MIN_SPEED : base_speed_rear_right);

		    // Set motor speeds
		    set_speed(1, base_speed_rear_left);    // Left Rear
		    set_speed(2, base_speed_front_right);  // Right Front
		    set_speed(3, base_speed_front_left);   // Left Front
		    set_speed(4, base_speed_rear_right);   // Right Rear
		}
	}

}

void car_brake()
{
    set_speed(1, 0);
    set_speed(2, 0);
    set_speed(3, 0);
    set_speed(4, 0);
    HAL_GPIO_WritePin(MOTOR_DIR_FRONT_L_GPIO_Port, MOTOR_DIR_FRONT_L_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_DIR_FRONT_R_GPIO_Port, MOTOR_DIR_FRONT_R_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_DIR_REAR_L_GPIO_Port, MOTOR_DIR_REAR_L_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_DIR_REAR_R_GPIO_Port, MOTOR_DIR_REAR_R_Pin, GPIO_PIN_RESET);
}

// Turn the car left by stopping the left motors and moving the right motors forward
void car_turn_left(int32_t speed)
{
    set_speed(1, 0);
    set_speed(2, speed);
    set_speed(3, 0);
    set_speed(4, speed);
}

// Turn the car right by stopping the right motors and moving the left motors forward
void car_turn_right(int32_t speed)
{
    set_speed(1, speed);
    set_speed(2, 0);
    set_speed(3, speed);
    set_speed(4, 0);
}

void car_move_right(int32_t speed, uint32_t ms) {
//	set_speed(3, speed);
//	set_speed(4, speed);
//	set_speed(1, -speed);
//	set_speed(2, -speed);

	uint32_t start_time = HAL_GetTick();
	uint32_t last_time = start_time;
	const float control_interval = INTERVAL; // Control loop interval in milliseconds

	while (1)
	{
		uint32_t current_time = HAL_GetTick();
		float elapsed_time = current_time - last_time;
		if (current_time - start_time >= ms) {
			car_brake();
			return;
		}
		if (elapsed_time >= control_interval)
		{
			last_time = current_time;

			float yaw = get_yaw();

			// 2. Compute PID output
			float dt = control_interval / 1000.0f; // Convert ms to seconds
			stabilization_yaw = PID_Update(&pid_yaw, desired_yaw, yaw, dt);

			// 3. Execute movement command with stabilization
			// Example: Move forward at speed 50
			// Base speed commands
			int32_t base_speed_front_left = speed;
			int32_t base_speed_front_right = -speed;
			int32_t base_speed_rear_left = -speed;
			int32_t base_speed_rear_right = speed;

			// Apply yaw stabilization adjustments
			base_speed_front_left += stabilization_yaw;
			base_speed_rear_left -= stabilization_yaw;
			base_speed_front_right -= stabilization_yaw;
			base_speed_rear_right += stabilization_yaw;

			// Limit speeds to [MIN_SPEED, MAX_SPEED]
			base_speed_front_left = (base_speed_front_left > MAX_SPEED) ? MAX_SPEED : ((base_speed_front_left < MIN_SPEED) ? MIN_SPEED : base_speed_front_left);
			base_speed_front_right = (base_speed_front_right > MAX_SPEED) ? MAX_SPEED : ((base_speed_front_right < MIN_SPEED) ? MIN_SPEED : base_speed_front_right);
			base_speed_rear_left = (base_speed_rear_left > MAX_SPEED) ? MAX_SPEED : ((base_speed_rear_left < MIN_SPEED) ? MIN_SPEED : base_speed_rear_left);
			base_speed_rear_right = (base_speed_rear_right > MAX_SPEED) ? MAX_SPEED : ((base_speed_rear_right < MIN_SPEED) ? MIN_SPEED : base_speed_rear_right);

			// Set motor speeds
			set_speed(1, base_speed_rear_left);    // Left Rear
			set_speed(2, base_speed_front_right);  // Right Front
			set_speed(3, base_speed_front_left);   // Left Front
			set_speed(4, base_speed_rear_right);   // Right Rear
		}
	}
}

void car_move_left(int32_t speed, uint32_t ms) {
//	set_speed(3, -speed);
//	set_speed(4, -speed);
//	set_speed(1, speed);
//	set_speed(2, speed);
	car_move_right(-speed, ms);
}


void car_spin_left(int32_t speed)
{
    set_speed(1, speed);
    set_speed(2, speed);
    set_speed(3, speed);
    set_speed(4, speed);
}

void car_spin_right(int32_t speed)
{
    set_speed(1, -speed);
    set_speed(2, -speed);
    set_speed(3, -speed);
    set_speed(4, -speed);
}

void car_turn_around_right() {
	HAL_Delay(1000);
	car_spin_right(55);
	HAL_Delay(1120);
	car_brake();
	car_spin_right(55);
	HAL_Delay(1120);
	car_brake();
}

void car_turn_around_left() {
	HAL_Delay(1000);
	car_spin_left(55);
	HAL_Delay(1120);
	car_brake();
	car_spin_left(55);
	HAL_Delay(1120);
	car_brake();
}

void car_run_up_one() {
	car_run(90, 1800);
}

void car_run_down_one() {
	car_run(-90, 1700);
}

void car_run_left_one() {
	car_move_left(90, 2450);
}

void car_run_right_one() {
	car_move_right(90, 2450);
}

/* -------------------------------------------- Routing Functions -------------------------------------------- */
char ctrl_comm_last = COMM_STOP; // 上一次的指令
char ctrl_comm; // 本次的指令

void IRSearchInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure; // 创建一个GPIO初始化结构体变量

    // 使能GPIOC的时钟
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // 配置右侧搜索引脚
    GPIO_InitStructure.Pin = SEARCH_R_PIN; // 配置使能GPIO管脚为SEARCH_R_PIN
    GPIO_InitStructure.Mode = GPIO_PULLUP; // 配置GPIO模式为输入上拉（Input Pull Up）
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // 配置GPIO端口速度为50MHz
    HAL_GPIO_Init(SEARCH_R_GPIO, &GPIO_InitStructure); // 初始化SEARCH_R_GPIO

    // 配置左侧搜索引脚
    GPIO_InitStructure.Pin = SEARCH_L_PIN; // 配置使能GPIO管脚为SEARCH_L_PIN
    GPIO_InitStructure.Mode = GPIO_PULLUP; // 配置GPIO模式为输入上拉（Input Pull Up）
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH; // 配置GPIO端口速度为50MHz
    HAL_GPIO_Init(SEARCH_L_GPIO, &GPIO_InitStructure); // 初始化SEARCH_L_GPIO
}

void SearchRun(void) {
    if(SEARCH_L_IO == WHITE_AREA && SEARCH_R_IO == WHITE_AREA)
        ctrl_comm = COMM_UP;  // 两边数值相同
    else if (IR_L_GROUND_BLACK && IR_R_GROUND_WHITE)
        ctrl_comm = COMM_LEFT;
    else if (IR_R_GROUND_BLACK && IR_L_GROUND_WHITE)
        ctrl_comm = COMM_RIGHT;
    else
        ctrl_comm = COMM_STOP;

    if(ctrl_comm_last != ctrl_comm)
    {
        ctrl_comm_last = ctrl_comm;
        switch(ctrl_comm)
        {
            case COMM_UP:
                set_speed(1, -10);
                set_speed(2, 10);
                set_speed(3, 10);
                set_speed(4, -10);
                break;
            case COMM_DOWN:
            	set_speed(1, 10);
            	set_speed(2, -10);
            	set_speed(3, -10);
            	set_speed(4, 10);
            	break;
            case COMM_RIGHT: car_spin_right(5); break;
            case COMM_LEFT:  car_spin_left(5); break;
            case COMM_STOP:  car_brake(10); break;
            default: break;
        }
    }
}
