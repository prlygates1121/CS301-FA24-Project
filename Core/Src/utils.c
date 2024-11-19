#include <stdint.h>
#include "utils.h"
#include "tim.h"

void HAL_Delay_us(uint32_t us) {
	__HAL_TIM_SET_COUNTER(&htim1, 0);
	while (__HAL_TIM_GET_COUNTER(&htim1) < us);
}

void set_joint_angle(float angle)
{
	angle = (uint16_t)(50.0 * angle / 9.0 + 249.0);
	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, angle);
}

uint16_t UT_get_distance() {
	HAL_GPIO_WritePin(UT_SEND_GPIO_Port, UT_SEND_Pin, GPIO_PIN_SET);
	HAL_Delay_us(20);
	HAL_GPIO_WritePin(UT_SEND_GPIO_Port, UT_SEND_Pin, GPIO_PIN_RESET);
	return UT_distance * 10;
}

uint16_t UT_get_stable_distance() {
	float distance_average = 0;
	for (uint8_t i = 0; i < 5; i++) {
		distance_average += UT_get_distance();
	}
	return distance_average / 5;
}
