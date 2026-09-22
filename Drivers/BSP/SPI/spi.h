/**
 ****************************************************************************************************
 * @file        spi.h
 * @version     V1.0
 * @brief       SPI 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */

#ifndef __SPI_H
#define __SPI_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* SPI1 引脚 定义 */

#define SPI1_SCK_GPIO_PORT           GPIOB
#define SPI1_SCK_GPIO_PIN            GPIO_PIN_13
#define SPI1_SCK_GPIO_AF             GPIO_AF_5
#define SPI1_SCK_GPIO_CLK            RCU_GPIOB     /* GPIOB时钟使能 */

#define SPI1_MISO_GPIO_PORT          GPIOB
#define SPI1_MISO_GPIO_PIN           GPIO_PIN_14
#define SPI1_MISO_GPIO_AF            GPIO_AF_5
#define SPI1_MISO_GPIO_CLK           RCU_GPIOB     /* GPIOB时钟使能 */

#define SPI1_MOSI_GPIO_PORT          GPIOB
#define SPI1_MOSI_GPIO_PIN           GPIO_PIN_15
#define SPI1_MOSI_GPIO_AF            GPIO_AF_5
#define SPI1_MOSI_GPIO_CLK           RCU_GPIOB     /* GPIOB时钟使能 */

/* SPI1相关定义 */
#define SPI1_SPI                     SPI1
#define SPI1_SPI_CLK                 RCU_SPI1      /* SPI1时钟使能 */
  
/******************************************************************************************/

/* SPI总线速度设置 */
#define SPI_SPEED_2         0
#define SPI_SPEED_4         1
#define SPI_SPEED_8         2
#define SPI_SPEED_16        3
#define SPI_SPEED_32        4
#define SPI_SPEED_64        5
#define SPI_SPEED_128       6
#define SPI_SPEED_256       7


void spi1_init(void);                              /* SPI1初始化 */               
void spi1_set_speed(uint8_t speed);                /* 设置SPI1速度 */
uint8_t spi1_read_write_byte(uint8_t txdata);      /* SPI1读写一个字节 */

#endif
























