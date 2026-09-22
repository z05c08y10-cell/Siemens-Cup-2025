/**
 ****************************************************************************************************
 * @file        timer.h
 * @version     V1.0
 * @brief       定时器中断 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6小系统板
 *
 ****************************************************************************************************
 */

#ifndef __TIMER_H
#define __TIMER_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* 定时器 定义 */

/* TIMERX 中断定义 
 * 注意: 通过修改这4个宏定义,可以支持任意一个定时器.
 */
 
#define TIMERX_INT                       TIMER6
#define TIMERX_INT_IRQn                  TIMER6_IRQn
#define TIMERX_INT_IRQHandler            TIMER6_IRQHandler
#define TIMERX_INT_CLK                   RCU_TIMER6   /* TIMER6 时钟使能 */

/******************************************************************************************/

void timerx_int_init(uint32_t arr, uint16_t psc);     /* 定时器TIMERX 定时中断初始化函数 */
void timerx_set_period(uint32_t period_ms);           /* 新增：设置定时器周期 */
void timerx_enable(void);                             /* 新增：使能定时器 */
void timerx_disable(void);                            /* 新增：禁用定时器 */

extern volatile uint8_t timer6_sample_flag;           /* 采样标志 */
extern volatile uint8_t timer6_led_flag;              /* LED闪烁标志 */

#endif

