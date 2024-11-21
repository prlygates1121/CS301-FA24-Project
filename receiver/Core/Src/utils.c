#include <stdint.h>
#include "utils.h"
#include "tim.h"

/* --------------------------------------------- Custom Delay --------------------------------------------- */

void HAL_Delay_us(uint32_t us) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}

/* -------------------------------------------- Servo Settings -------------------------------------------- */

void set_joint_angle(float angle)
{
	angle = (uint16_t)(50.0 * angle / 9.0 + 249.0);
	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, angle);
}

/* -------------------------------------- Ultrasonic Sensor Settings -------------------------------------- */


uint16_t UT_get_distance() {
	HAL_GPIO_WritePin(US_SEND_GPIO_Port, US_SEND_Pin, GPIO_PIN_SET);
	HAL_Delay_us(20);
	HAL_GPIO_WritePin(US_SEND_GPIO_Port, US_SEND_Pin, GPIO_PIN_RESET);
	return US_distance * 10;
}


/* -------------------------------------------- Motor Settings -------------------------------------------- */

// Set motor speed and direction
void mySpeedSet(uint8_t channel, int32_t speed)
{
    uint16_t pwm_value;

    // Limit speed range to [-100, 100]
    if (speed > 100) speed = 100;
    if (speed < -100) speed = -100;

    // Calculate PWM value based on speed
    pwm_value = (speed >= 0) ? (7201 - speed * 72) : (7201 + speed * 72);

    // Set PWM duty cycle and direction based on channel
    switch (channel)
    {
        case 1: // Left front motor
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_value);
            HAL_GPIO_WritePin(MOTOR_DIR_FRONT_L_GPIO_Port, MOTOR_DIR_FRONT_L_Pin, speed >= 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case 2: // Right front motor
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm_value);
            HAL_GPIO_WritePin(MOTOR_DIR_FRONT_R_GPIO_Port, MOTOR_DIR_FRONT_R_Pin, speed >= 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
            break;

        case 3: // Left rear motor
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, pwm_value);
            HAL_GPIO_WritePin(MOTOR_DIR_REAR_L_GPIO_Port, MOTOR_DIR_REAR_L_Pin, speed >= 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case 4: // Right rear motor
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, pwm_value);
            HAL_GPIO_WritePin(MOTOR_DIR_REAR_R_GPIO_Port, MOTOR_DIR_REAR_R_Pin, speed >= 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
            break;

        default:
            // Invalid channel, handle error if necessary
            break;
    }
}

// Move the car forward at a specified speed for a specified time
void myCarRun(int8_t speed, uint32_t time)
{
    mySpeedSet(1, -speed);   // Left front motor
    mySpeedSet(2, speed);   // Right front motor
    mySpeedSet(3, -speed);   // Left rear motor
    mySpeedSet(4, speed);   // Right rear motor
    HAL_Delay(time);
}

// Stop the car by setting all motors' speed to 0
void myCarBrake(uint32_t time)
{
    mySpeedSet(1, 0); // Left front motor
    mySpeedSet(2, 0); // Right front motor
    mySpeedSet(3, 0); // Left rear motor
    mySpeedSet(4, 0); // Right rear motor
    HAL_GPIO_WritePin(MOTOR_DIR_FRONT_L_GPIO_Port, MOTOR_DIR_FRONT_L_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_DIR_FRONT_R_GPIO_Port, MOTOR_DIR_FRONT_R_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_DIR_REAR_L_GPIO_Port, MOTOR_DIR_REAR_L_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_DIR_REAR_R_GPIO_Port, MOTOR_DIR_REAR_R_Pin, GPIO_PIN_RESET);

//    HAL_GPIO_WritePin(LEFT_FRONT_MOTOR_DIR_PORT, LEFT_FRONT_MOTOR_DIR_PIN, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(RIGHT_FRONT_MOTOR_DIR_PORT, RIGHT_FRONT_MOTOR_DIR_PIN, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(LEFT_REAR_MOTOR_DIR_PORT, LEFT_REAR_MOTOR_DIR_PIN, GPIO_PIN_RESET);
//    HAL_GPIO_WritePin(RIGHT_REAR_MOTOR_DIR_PORT, RIGHT_REAR_MOTOR_DIR_PIN, GPIO_PIN_RESET);
    HAL_Delay(time);
}

// Turn the car left by stopping the left motors and moving the right motors forward
void myCarLeft(int8_t speed, uint32_t time)
{
    mySpeedSet(1, 0);      // Stop left front motor
    mySpeedSet(2, speed);  // Right front motor forward
    mySpeedSet(3, 0);      // Stop left rear motor
    mySpeedSet(4, speed);  // Right rear motor forward
    HAL_Delay(time);
}

// Turn the car right by stopping the right motors and moving the left motors forward
void myCarRight(int8_t speed, uint32_t time)
{
    mySpeedSet(1, speed);  // Left front motor forward
    mySpeedSet(2, 0);      // Stop right front motor
    mySpeedSet(3, speed);  // Left rear motor forward
    mySpeedSet(4, 0);      // Stop right rear motor
    HAL_Delay(time);
}

// Spin the car left by rotating left wheels backward and right wheels forward
void myCarSpinLeft(int8_t speed, uint32_t time)
{
    mySpeedSet(1, -speed); // Left front motor backward
    mySpeedSet(2, speed);  // Right front motor forward
    mySpeedSet(3, -speed); // Left rear motor backward
    mySpeedSet(4, speed);  // Right rear motor forward
    HAL_Delay(time);
}

// Spin the car right by rotating right wheels backward and left wheels forward
void myCarSpinRight(int8_t speed, uint32_t time)
{
    mySpeedSet(1, speed);  // Left front motor forward
    mySpeedSet(2, -speed); // Right front motor backward
    mySpeedSet(3, speed);  // Left rear motor forward
    mySpeedSet(4, -speed); // Right rear motor backward
    HAL_Delay(time);
}
