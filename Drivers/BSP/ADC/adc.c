/**
 ****************************************************************************************************
 * @file        adc.c
 * @version     V1.0
 * @brief       ADC 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */
 
#include "./BSP/ADC/adc.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       ADC初始化函数
 *              我们使用12位分辨率, ADC采样时钟=20M, 转换时间为: 采样时间 + 12个ADC周期
 *              设置最大采样周期: 480, 则转换时间 = 492个ADC周期 = 24.6us
 * @param       无
 * @retval      无
 */
void adc_init(void)
{
    rcu_periph_clock_enable(ADC_ADCX_CHY_CLK);           /* 使能ADCX时钟 */
    rcu_periph_clock_enable(ADC_ADCX_CHY_GPIO_CLK);      /* 使能ADC IO口时钟 */
    gpio_mode_set(ADC_ADCX_CHY_GPIO_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, ADC_ADCX_CHY_GPIO_PIN);  /* ADC采集引脚模式设置,模拟输入 */    

    adc_deinit();   /* 复位所有的ADC外设 */

    /* ADC时钟选择来自 APB2, 即PCLK2, 频率为120Mhz, ADC最大时钟频率为40Mhz
     * 在120M PCLK2条件下, 我们使用6分频, 得到PCLK2 / 6 = 20Mhz 的ADC时钟
     */    
    adc_clock_config(ADC_ADCCK_PCLK2_DIV6);                                  /* 配置所有ADC时钟为PCLK2时钟6分频 */
  
    adc_sync_mode_config(ADC_SYNC_MODE_INDEPENDENT);                         /* ADC同步模式失能，所有ADC都独立工作 */
          
    adc_special_function_config(ADC_ADCX, ADC_SCAN_MODE, DISABLE);           /* 非扫描模式 */	
  
    adc_special_function_config(ADC_ADCX, ADC_CONTINUOUS_MODE, DISABLE);     /* 非连续模式 */	 
 
    adc_resolution_config(ADC_ADCX, ADC_RESOLUTION_12B);                     /* 12位分辨率 */
  
    adc_data_alignment_config(ADC_ADCX, ADC_DATAALIGN_RIGHT);                /* 数据最低有效位对齐(右对齐) */

    adc_external_trigger_config(ADC_ADCX, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_DISABLE);  /* 常规序列外部触发失能 */
       
    adc_channel_length_config(ADC_ADCX, ADC_ROUTINE_CHANNEL, 1);             /* 配置常规序列长度为1，1个转换在常规通道转换序列中 也就是只转换常规序列0 */

    adc_enable(ADC_ADCX);                                                    /* 使能ADCX */
    delay_ms(10);                                                            /* 等待ADC稳定 */
    
    adc_calibration_enable(ADC_ADCX);                                        /* ADCX校准复位 */    
}

/**
 * @brief       获得ADC转换后的结果
 * @param       channel: 通道编号 0~18，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_18 
 * @retval      转换结果
 */
uint16_t adc_get_result(uint8_t channel)
{
    adc_routine_channel_config(ADC_ADCX, 0U, channel, ADC_SAMPLETIME_480);   /* 配置ADCX常规通道组，序列0对应通道采样时间为480个时钟周期 */

    adc_software_trigger_enable(ADC_ADCX, ADC_ROUTINE_CHANNEL);  /* 软件触发常规序列启动转换 */
 
    while (!adc_flag_get(ADC_ADCX, ADC_FLAG_EOC));               /* 等待常规序列转换结束 */

    adc_flag_clear(ADC_ADCX, ADC_FLAG_EOC);                      /* 清除常规序列转换结束标志位 */
 
    return (adc_routine_data_read(ADC_ADCX));                    /* 返回ADCX常规通道的转换结果 */
}

/**
 * @brief       获取通道ch的转换值，取times次,然后平均
 * @param       channel: 通道编号 0~18，取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_18 
 * @param       times  : 获取次数
 * @retval      通道channel的times次转换结果平均值
 */
uint32_t adc_get_result_average(uint8_t channel, uint8_t times)
{
    uint32_t temp_val = 0;
    uint8_t t;

    for (t = 0; t < times; t++)             /* 获取times次数据 */
    {
        temp_val += adc_get_result(channel);
        delay_ms(5);
    }

    return temp_val / times;                /* 返回平均值 */
}









