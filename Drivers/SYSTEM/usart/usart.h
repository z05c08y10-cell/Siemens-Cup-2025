/**
 ****************************************************************************************************					 
 * @file        usart.h
 * @version     V1.1
 * @brief       串口初始化代码，支持printf            
 ****************************************************************************************************
 *
 * V1.1
 * 修改SYS_SUPPORT_OS部分代码, 包含头文件改成:"os.h"
 *
 ****************************************************************************************************
 */	

#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "./SYSTEM/sys/sys.h"


/*******************************************************************************************************/
/* 引脚和串口 定义 */

#define USART_TX_GPIO_PORT              GPIOA
#define USART_TX_GPIO_PIN               GPIO_PIN_9
#define USART_TX_GPIO_AF                GPIO_AF_7
#define USART_TX_GPIO_CLK               RCU_GPIOA   /* GPIOA时钟使能 */

#define USART_RX_GPIO_PORT              GPIOA
#define USART_RX_GPIO_PIN               GPIO_PIN_10
#define USART_RX_GPIO_AF                GPIO_AF_7
#define USART_RX_GPIO_CLK               RCU_GPIOA   /* GPIOA时钟使能 */

#define USART_UX                        USART0
#define USART_UX_IRQn                   USART0_IRQn
#define USART_UX_IRQHandler             USART0_IRQHandler
#define USART_UX_CLK                    RCU_USART0  /* USART0 时钟使能 */

#define USART_REC_LEN               200             /* 定义最大接收字节数 200 */
#define USART_EN_RX                 1               /* 使能（1）/禁止（0）串口接收 */


extern uint8_t  g_usart_rx_buf[USART_REC_LEN];      /* 接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 */
extern uint16_t g_usart_rx_sta;                     /* 接收状态标记 */

/*******************************************************************************************************/

void usart_init(uint32_t baudrate);                 /* 串口初始化函数 */

#endif


