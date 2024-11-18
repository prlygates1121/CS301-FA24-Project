#include "servo.h"
#include "tim.h"

void SetJointAngle(float angle)
{
	angle = (uint16_t)(50.0 * angle / 9.0 + 249.0);
//	TIM_SetCompare1(TIM5,angle);
	__HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, angle);
}
