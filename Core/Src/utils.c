#include "utils.h"
#include "tim.h"

void setJointAngle(float angle)
{
	angle = (uint16_t)(50.0 * angle / 9.0 + 249.0);
	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, angle);
}
