#ifndef MY_MOTOR_H
#define MY_MOTOR_H

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_tim.h"

// Define the default timer
#define MOTOR_TIM htim4

// Motor GPIO and Timer Channel Definitions
// Direction control pins
#define LEFT_FRONT_MOTOR_DIR_PIN     GPIO_PIN_12
#define LEFT_FRONT_MOTOR_DIR_PORT    GPIOB

#define RIGHT_FRONT_MOTOR_DIR_PIN    GPIO_PIN_14
#define RIGHT_FRONT_MOTOR_DIR_PORT   GPIOB

#define LEFT_REAR_MOTOR_DIR_PIN      GPIO_PIN_13
#define LEFT_REAR_MOTOR_DIR_PORT     GPIOB

#define RIGHT_REAR_MOTOR_DIR_PIN     GPIO_PIN_15
#define RIGHT_REAR_MOTOR_DIR_PORT    GPIOB

// PWM control pins
#define LEFT_FRONT_MOTOR_PWM_PIN     GPIO_PIN_6
#define LEFT_FRONT_MOTOR_PWM_CHANNEL TIM_CHANNEL_1
#define LEFT_FRONT_MOTOR_PWM_PORT    GPIOB

#define RIGHT_FRONT_MOTOR_PWM_PIN    GPIO_PIN_8
#define RIGHT_FRONT_MOTOR_PWM_CHANNEL TIM_CHANNEL_3
#define RIGHT_FRONT_MOTOR_PWM_PORT   GPIOB

#define LEFT_REAR_MOTOR_PWM_PIN      GPIO_PIN_7
#define LEFT_REAR_MOTOR_PWM_CHANNEL TIM_CHANNEL_2
#define LEFT_REAR_MOTOR_PWM_PORT     GPIOB

#define RIGHT_REAR_MOTOR_PWM_PIN     GPIO_PIN_9
#define RIGHT_REAR_MOTOR_PWM_CHANNEL TIM_CHANNEL_4
#define RIGHT_REAR_MOTOR_PWM_PORT    GPIOB

// Combined direction control pins for simplified configuration
#define MOTOR_DIR_PINS (LEFT_FRONT_MOTOR_DIR_PIN | RIGHT_FRONT_MOTOR_DIR_PIN | LEFT_REAR_MOTOR_DIR_PIN | RIGHT_REAR_MOTOR_DIR_PIN)

// Combined PWM control pins for simplified configuration
#define MOTOR_PWM_PINS (LEFT_FRONT_MOTOR_PWM_PIN | RIGHT_FRONT_MOTOR_PWM_PIN | LEFT_REAR_MOTOR_PWM_PIN | RIGHT_REAR_MOTOR_PWM_PIN)

// Function Declarations
void myMotorInit(void);
void mySpeedSet(uint8_t channel, int32_t speed);

// Motion control functions
void myCarRun(int8_t speed, uint32_t time);
void myCarBrake(uint32_t time);
void myCarLeft(int8_t speed, uint32_t time);
void myCarRight(int8_t speed, uint32_t time);
void myCarSpinLeft(int8_t speed, uint32_t time);
void myCarSpinRight(int8_t speed, uint32_t time);

#endif // MY_MOTOR_H
