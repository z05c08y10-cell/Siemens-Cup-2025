/**
 ****************************************************************************************************
 * @file        led.h
 * @version     V1.0
 * @brief       LED 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */	

#ifndef __LED_H
#define __LED_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 引脚 定义 */

#define LED1_GPIO_PORT                  GPIOD
#define LED1_GPIO_PIN                   GPIO_PIN_1
#define LED1_GPIO_CLK                   RCU_GPIOD     /* GPIOD时钟使能 */

#define LED2_GPIO_PORT                  GPIOD
#define LED2_GPIO_PIN                   GPIO_PIN_3
#define LED2_GPIO_CLK                   RCU_GPIOD     /* GPIOD时钟使能 */

#define LED3_GPIO_PORT                  GPIOD
#define LED3_GPIO_PIN                   GPIO_PIN_5
#define LED3_GPIO_CLK                   RCU_GPIOD     /* GPIOD时钟使能 */

#define LED4_GPIO_PORT                  GPIOD
#define LED4_GPIO_PIN                   GPIO_PIN_7
#define LED4_GPIO_CLK                   RCU_GPIOD     /* GPIOD时钟使能 */

#define LED5_GPIO_PORT                  GPIOB
#define LED5_GPIO_PIN                   GPIO_PIN_4
#define LED5_GPIO_CLK                   RCU_GPIOB     /* GPIOB时钟使能 */

#define LED6_GPIO_PORT                  GPIOB
#define LED6_GPIO_PIN                   GPIO_PIN_6
#define LED6_GPIO_CLK                   RCU_GPIOB     /* GPIOB时钟使能 */


/* LED端口定义 */
#define LED1(x)   do{ x ? \
                      gpio_bit_write(LED1_GPIO_PORT, LED1_GPIO_PIN, SET) : \
                      gpio_bit_write(LED1_GPIO_PORT, LED1_GPIO_PIN, RESET); \
                  }while(0)      

#define LED2(x)   do{ x ? \
                      gpio_bit_write(LED2_GPIO_PORT, LED2_GPIO_PIN, SET) : \
                      gpio_bit_write(LED2_GPIO_PORT, LED2_GPIO_PIN, RESET); \
                  }while(0)     

#define LED3(x)   do{ x ? \
                      gpio_bit_write(LED3_GPIO_PORT, LED3_GPIO_PIN, SET) : \
                      gpio_bit_write(LED3_GPIO_PORT, LED3_GPIO_PIN, RESET); \
                  }while(0)      

#define LED4(x)   do{ x ? \
                      gpio_bit_write(LED4_GPIO_PORT, LED4_GPIO_PIN, SET) : \
                      gpio_bit_write(LED4_GPIO_PORT, LED4_GPIO_PIN, RESET); \
                  }while(0)    

#define LED5(x)   do{ x ? \
                      gpio_bit_write(LED5_GPIO_PORT, LED5_GPIO_PIN, SET) : \
                      gpio_bit_write(LED5_GPIO_PORT, LED5_GPIO_PIN, RESET); \
                  }while(0)      

#define LED6(x)   do{ x ? \
                      gpio_bit_write(LED6_GPIO_PORT, LED6_GPIO_PIN, SET) : \
                      gpio_bit_write(LED6_GPIO_PORT, LED6_GPIO_PIN, RESET); \
                  }while(0)     

                  
/* LED取反定义 */
#define LED1_TOGGLE()   do{ gpio_bit_toggle(LED1_GPIO_PORT, LED1_GPIO_PIN); }while(0)      /* 翻转LED1 */
#define LED2_TOGGLE()   do{ gpio_bit_toggle(LED2_GPIO_PORT, LED2_GPIO_PIN); }while(0)      /* 翻转LED2 */
#define LED3_TOGGLE()   do{ gpio_bit_toggle(LED3_GPIO_PORT, LED3_GPIO_PIN); }while(0)      /* 翻转LED3 */
#define LED4_TOGGLE()   do{ gpio_bit_toggle(LED4_GPIO_PORT, LED4_GPIO_PIN); }while(0)      /* 翻转LED4 */
#define LED5_TOGGLE()   do{ gpio_bit_toggle(LED5_GPIO_PORT, LED5_GPIO_PIN); }while(0)      /* 翻转LED5 */
#define LED6_TOGGLE()   do{ gpio_bit_toggle(LED6_GPIO_PORT, LED6_GPIO_PIN); }while(0)      /* 翻转LED6 */

/******************************************************************************************/

void led_init(void);             /* 初始化LED */

#endif





