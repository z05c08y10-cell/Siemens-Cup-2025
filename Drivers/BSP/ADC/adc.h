/**
 ****************************************************************************************************
 * @file        adc.h
 * @version     V1.0
 * @brief       ADC 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */

#ifndef __ADC_H
#define __ADC_H

#include "./SYSTEM/sys/sys.h"


/******************************************************************************************/
/* ADC及引脚 定义 */

#define ADC_ADCX_CHY_GPIO_PORT              GPIOC
#define ADC_ADCX_CHY_GPIO_PIN               GPIO_PIN_0
#define ADC_ADCX_CHY_GPIO_CLK               RCU_GPIOC       /* GPIOC时钟使能 */

#define ADC_ADCX                            ADC0
#define ADC_ADCX_CHY                        ADC_CHANNEL_10   /* ADC_CHANNEL_x(x=0..18) */        
#define ADC_ADCX_CHY_CLK                    RCU_ADC0        /* ADC0 时钟使能 */

/******************************************************************************************/

void adc_init(void);                                             /* ADC初始化 */
uint16_t adc_get_result(uint8_t channel);                        /* 获得某个通道的ADC转换结果 */
uint32_t adc_get_result_average(uint8_t channel, uint8_t times); /* 得到某个通道给定次数采样的平均值 */

#endif 















