/**
 ****************************************************************************************************
 * @file        key.h
 * @version     V1.0
 * @brief       按键输入 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */	

#ifndef __KEY_H
#define __KEY_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 定义 */

#define KEY1_GPIO_PORT                  GPIOB
#define KEY1_GPIO_PIN                   GPIO_PIN_0
#define KEY1_GPIO_CLK                   RCU_GPIOB     /* GPIOB时钟使能 */

#define KEY2_GPIO_PORT                  GPIOE
#define KEY2_GPIO_PIN                   GPIO_PIN_7
#define KEY2_GPIO_CLK                   RCU_GPIOE     /* GPIOE时钟使能 */

#define KEY3_GPIO_PORT                  GPIOE
#define KEY3_GPIO_PIN                   GPIO_PIN_9
#define KEY3_GPIO_CLK                   RCU_GPIOE     /* GPIOE时钟使能 */

#define KEY4_GPIO_PORT                  GPIOE
#define KEY4_GPIO_PIN                   GPIO_PIN_11
#define KEY4_GPIO_CLK                   RCU_GPIOE     /* GPIOE时钟使能 */

#define KEY5_GPIO_PORT                  GPIOE
#define KEY5_GPIO_PIN                   GPIO_PIN_13
#define KEY5_GPIO_CLK                   RCU_GPIOE     /* GPIOE时钟使能 */

#define KEY6_GPIO_PORT                  GPIOE
#define KEY6_GPIO_PIN                   GPIO_PIN_15
#define KEY6_GPIO_CLK                   RCU_GPIOE     /* GPIOE时钟使能 */

#define WKUP_GPIO_PORT                  GPIOA
#define WKUP_GPIO_PIN                   GPIO_PIN_0
#define WKUP_GPIO_CLK                   RCU_GPIOA     /* GPIOA时钟使能 */

#define KEY1        gpio_input_bit_get(KEY1_GPIO_PORT, KEY1_GPIO_PIN)   /* 读取KEY1引脚 */
#define KEY2        gpio_input_bit_get(KEY2_GPIO_PORT, KEY2_GPIO_PIN)   /* 读取KEY2引脚 */
#define KEY3        gpio_input_bit_get(KEY3_GPIO_PORT, KEY3_GPIO_PIN)   /* 读取KEY3引脚 */
#define KEY4        gpio_input_bit_get(KEY4_GPIO_PORT, KEY4_GPIO_PIN)   /* 读取KEY4引脚 */
#define KEY5        gpio_input_bit_get(KEY5_GPIO_PORT, KEY5_GPIO_PIN)   /* 读取KEY5引脚 */
#define KEY6        gpio_input_bit_get(KEY6_GPIO_PORT, KEY6_GPIO_PIN)   /* 读取KEY6引脚 */
#define WK_UP       gpio_input_bit_get(WKUP_GPIO_PORT, WKUP_GPIO_PIN)   /* 读取WKUP引脚 */

#define KEY1_PRES 	1             /* KEY1按下 */
#define KEY2_PRES 	2             /* KEY2按下 */
#define KEY3_PRES 	3             /* KEY3按下 */
#define KEY4_PRES 	4             /* KEY4按下 */
#define KEY5_PRES 	5             /* KEY5按下 */
#define KEY6_PRES 	6             /* KEY6按下 */
#define WKUP_PRES   7             /* WK_UP按下 */

/******************************************************************************************/

void key_init(void);              /* 按键初始化函数 */
uint8_t key_scan(uint8_t mode);   /* 按键扫描函数 */

#endif
