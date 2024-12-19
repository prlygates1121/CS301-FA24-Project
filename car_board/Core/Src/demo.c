/**
 ****************************************************************************************************
 * @file        demo.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MS6050模块测试实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 MiniSTM32 V4开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "demo.h"
#include "atk_ms6050.h"
#include "inv_mpu.h"
#include "usart.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

///**
// * @brief       例程演示入口函数
// * @param       无
// * @retval      无
// */
void demo_run(void)
{
    uint8_t ret;
    float pit, rol, yaw;
    int16_t acc_x, acc_y, acc_z;
    int16_t gyr_x, gyr_y, gyr_z;
    int16_t temp;

    /* 初始化ATK-MS6050 */
    ret = atk_ms6050_init();
    if (ret != 0)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)"ATK-MS6050 init failed!\r\n", strlen("ATK-MS6050 init failed!\r\n"), HAL_MAX_DELAY);
        while (1)
        {
        	TOGGLE_LED1;
            HAL_Delay(200);
        }
    }

    /* 初始化ATK-MS6050 DMP */
    ret = atk_ms6050_dmp_init();
    if (ret != 0)
    {
        HAL_UART_Transmit(&huart1, (uint8_t*)"ATK-MS6050 DMP init failed!\r\n", strlen("ATK-MS6050 DMP init failed!\r\n"), HAL_MAX_DELAY);
        while (1)
        {
        	TOGGLE_LED2;
            HAL_Delay(200);
        }
    }

    /* LCD UI初始化 */

    while (1)
    {

        /* 获取ATK-MS6050 DMP处理后的数据 */
        ret  = atk_ms6050_dmp_get_data(&yaw);
        /* 获取ATK-MS6050加速度值 */
        ret += atk_ms6050_get_accelerometer(&acc_x, &acc_y, &acc_z);
        /* 获取ATK-MS6050陀螺仪值 */
        ret += atk_ms6050_get_gyroscope(&gyr_x, &gyr_y, &gyr_z);
        /* 获取ATK-MS6050温度值 */
        ret += atk_ms6050_get_temperature(&temp);
        if (ret == 0)
        {
            	char info[50];

                sprintf(info, "yaw: %.2f, ", yaw);
                HAL_UART_Transmit(&huart1, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);

                sprintf(info, "acc_x: %d, acc_y: %d, acc_z: %d, ", acc_x, acc_y, acc_z);
                HAL_UART_Transmit(&huart1, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);

                sprintf(info, "gyr_x: %d, gyr_y: %d, gyr_z: %d, ", gyr_x, gyr_y, gyr_z);
                HAL_UART_Transmit(&huart1, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);

                sprintf(info, "temp: %d\r\n", temp);
                HAL_UART_Transmit(&huart1, (uint8_t*)info, strlen(info), HAL_MAX_DELAY);

        }

    }
}
