#include <stdint.h>
#include "utils.h"
#include "24l01.h"
#include "tim.h"

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

/* ------------------------------------- Ultrasonic Sensor Functions ------------------------------------- */

uint16_t UT_get_distance() {
	HAL_GPIO_WritePin(US_SEND_GPIO_Port, US_SEND_Pin, GPIO_PIN_SET);
	HAL_Delay_us(20);
	HAL_GPIO_WritePin(US_SEND_GPIO_Port, US_SEND_Pin, GPIO_PIN_RESET);
	return US_distance * 10;
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

void process_command(uint8_t command[]) {
	switch (command[0]) {
		case MOVE_FORWARD:
			car_run(55);
			HAL_Delay(500);
			car_brake();
			break;
		case MOVE_BACKWARD:
			car_run(-55);
			HAL_Delay(500);
			car_brake();
			break;
		case MOVE_LEFT:
			car_move_left(55);
			HAL_Delay(500);
			car_brake();
			break;
		case MOVE_RIGHT:
			car_move_right(55);
			HAL_Delay(500);
			car_brake();
			break;
		case BRAKE:
			car_brake();
			break;
		default:
			for (uint8_t i = 0; i < 16; i++) {
				if (command[i] == -1) {
					break;
				}
				switch (command[i]) {
					case ROUTE_UP:
						car_run(80);
						HAL_Delay(600);
						car_brake();
						break;
					case ROUTE_DOWN:
						car_run(-80);
						HAL_Delay(600);
						car_brake();
						break;
					case ROUTE_LEFT:
						car_move_left(80);
						HAL_Delay(600);
						car_brake();
						break;
					case ROUTE_RIGHT:
						car_move_right(80);
						HAL_Delay(600);
						car_brake();
						break;
				}
				HAL_Delay(500);
			}
			break;
	}
}


/* -------------------------------------------- Motor Functions ------------------------------------------- */

// Set motor speed and direction
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
            // Invalid channel, handle error if necessary
            break;
    }
}

// Move the car forward at a specified speed for a specified time
void car_run(int32_t speed)
{
    set_speed(1, -speed);
    set_speed(2, speed);
    set_speed(3, -speed);
    set_speed(4, speed);
}

// Stop the car by setting all motors' speed to 0
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
    set_speed(1, 0);      // Stop left front motor
    set_speed(2, speed);  // Right front motor forward
    set_speed(3, 0);      // Stop left rear motor
    set_speed(4, speed);  // Right rear motor forward
}

// Turn the car right by stopping the right motors and moving the left motors forward
void car_turn_right(int32_t speed)
{
    set_speed(1, speed);  // Left front motor forward
    set_speed(2, 0);      // Stop right front motor
    set_speed(3, speed);  // Left rear motor forward
    set_speed(4, 0);      // Stop right rear motor
}

void car_move_right(int32_t speed) {
	set_speed(3, speed);
	set_speed(4, speed);
	set_speed(1, -speed);
	set_speed(2, -speed);
}

void car_move_left(int32_t speed) {
	set_speed(3, -speed);
	set_speed(4, -speed);
	set_speed(1, speed);
	set_speed(2, speed);
}


// Spin the car left by rotating left wheels backward and right wheels forward
void car_spin_left(int32_t speed)
{
    set_speed(1, -speed); // Left front motor backward
    set_speed(2, speed);  // Right front motor forward
    set_speed(3, -speed); // Left rear motor backward
    set_speed(4, speed);  // Right rear motor forward
}

// Spin the car right by rotating right wheels backward and left wheels forward
void car_spin_right(int32_t speed)
{
    set_speed(1, speed);  // Left front motor forward
    set_speed(2, -speed); // Right front motor backward
    set_speed(3, speed);  // Left rear motor forward
    set_speed(4, -speed); // Right rear motor backward
}
