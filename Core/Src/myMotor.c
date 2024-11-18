#include "myMotor.h"

TIM_HandleTypeDef htim4; // Define htim4 globally in myMotor.c for use in PWM control

// Error handler function for debugging purposes
void myError_Handler(void);

// Initialize TIM4 for PWM control and motor control GPIOs
void myMotorInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    // Enable GPIOB clock
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Enable TIM4 clock
    __HAL_RCC_TIM4_CLK_ENABLE();

    // Configure direction control pins as push-pull outputs
    GPIO_InitStruct.Pin = MOTOR_DIR_PINS;  // Using the combined macro for all direction pins
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LEFT_FRONT_MOTOR_DIR_PORT, &GPIO_InitStruct);  // All direction control pins are on GPIOB

    // Configure PWM control pins as alternate function push-pull
    GPIO_InitStruct.Pin = MOTOR_PWM_PINS;  // Using the combined macro for all PWM pins
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // Alternate function for PWM
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LEFT_FRONT_MOTOR_PWM_PORT, &GPIO_InitStruct);  // All PWM control pins are on GPIOB

    // TIM4 Base Configuration for PWM
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = 0;
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = 7200 - 1;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        myError_Handler();
    }

    // Start TIM4 base generation in PWM mode
    if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
    {
        myError_Handler();
    }

    // PWM channel configuration for each motor
    sConfigOC.OCMode = TIM_OCMODE_PWM2;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;

    // Initialize PWM channels for all motors
    if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, LEFT_FRONT_MOTOR_PWM_CHANNEL) != HAL_OK ||
        HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, RIGHT_FRONT_MOTOR_PWM_CHANNEL) != HAL_OK ||
        HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, LEFT_REAR_MOTOR_PWM_CHANNEL) != HAL_OK ||
        HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, RIGHT_REAR_MOTOR_PWM_CHANNEL) != HAL_OK)
    {
        myError_Handler();
    }

    // Start PWM channels for all motors
    HAL_TIM_PWM_Start(&htim4, LEFT_FRONT_MOTOR_PWM_CHANNEL);  // Left front motor
    HAL_TIM_PWM_Start(&htim4, RIGHT_FRONT_MOTOR_PWM_CHANNEL); // Right front motor
    HAL_TIM_PWM_Start(&htim4, LEFT_REAR_MOTOR_PWM_CHANNEL);   // Left rear motor
    HAL_TIM_PWM_Start(&htim4, RIGHT_REAR_MOTOR_PWM_CHANNEL);  // Right rear motor
}

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
            __HAL_TIM_SET_COMPARE(&htim4, LEFT_FRONT_MOTOR_PWM_CHANNEL, pwm_value);
            HAL_GPIO_WritePin(LEFT_FRONT_MOTOR_DIR_PORT, LEFT_FRONT_MOTOR_DIR_PIN, speed >= 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case 2: // Right front motor
            __HAL_TIM_SET_COMPARE(&htim4, RIGHT_FRONT_MOTOR_PWM_CHANNEL, pwm_value);
            HAL_GPIO_WritePin(RIGHT_FRONT_MOTOR_DIR_PORT, RIGHT_FRONT_MOTOR_DIR_PIN, speed >= 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
            break;

        case 3: // Left rear motor
            __HAL_TIM_SET_COMPARE(&htim4, LEFT_REAR_MOTOR_PWM_CHANNEL, pwm_value);
            HAL_GPIO_WritePin(LEFT_REAR_MOTOR_DIR_PORT, LEFT_REAR_MOTOR_DIR_PIN, speed >= 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
            break;

        case 4: // Right rear motor
            __HAL_TIM_SET_COMPARE(&htim4, RIGHT_REAR_MOTOR_PWM_CHANNEL, pwm_value);
            HAL_GPIO_WritePin(RIGHT_REAR_MOTOR_DIR_PORT, RIGHT_REAR_MOTOR_DIR_PIN, speed >= 0 ? GPIO_PIN_RESET : GPIO_PIN_SET);
            break;

        default:
            // Invalid channel, handle error if necessary
            break;
    }
}

// Error handler function
void myError_Handler(void)
{
    // User can add error handling here
    while (1);
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
    HAL_GPIO_WritePin(LEFT_FRONT_MOTOR_DIR_PORT, LEFT_FRONT_MOTOR_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_FRONT_MOTOR_DIR_PORT, RIGHT_FRONT_MOTOR_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LEFT_REAR_MOTOR_DIR_PORT, LEFT_REAR_MOTOR_DIR_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RIGHT_REAR_MOTOR_DIR_PORT, RIGHT_REAR_MOTOR_DIR_PIN, GPIO_PIN_RESET);
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
