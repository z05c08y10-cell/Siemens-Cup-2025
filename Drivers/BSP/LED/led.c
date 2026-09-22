/**
 ****************************************************************************************************
 * @file        led.c
 * @version     V1.0
 * @brief       LED 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */
 
#include "./BSP/LED/led.h"


/**
 * @brief       初始化LED相关IO口, 并使能时钟
 * @param       无
 * @retval      无
 */
void led_init(void)
{
    rcu_periph_clock_enable(LED1_GPIO_CLK);     /* 使能LED1时钟 */
    rcu_periph_clock_enable(LED2_GPIO_CLK);     /* 使能LED2时钟 */
	  rcu_periph_clock_enable(LED3_GPIO_CLK);     /* 使能LED3时钟 */
    rcu_periph_clock_enable(LED4_GPIO_CLK);     /* 使能LED4时钟 */
	  rcu_periph_clock_enable(LED5_GPIO_CLK);     /* 使能LED5时钟 */
    rcu_periph_clock_enable(LED6_GPIO_CLK);     /* 使能LED6时钟 */
    
    /* LED1引脚模式设置 */
    gpio_mode_set(LED1_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, LED1_GPIO_PIN);
    gpio_output_options_set(LED1_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED1_GPIO_PIN);
  
    /* LED2引脚模式设置 */
    gpio_mode_set(LED2_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, LED2_GPIO_PIN);
    gpio_output_options_set(LED2_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED2_GPIO_PIN);
	
	  /* LED3引脚模式设置 */
    gpio_mode_set(LED3_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, LED3_GPIO_PIN);
    gpio_output_options_set(LED3_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED3_GPIO_PIN);
  
    /* LED4引脚模式设置 */
    gpio_mode_set(LED4_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, LED4_GPIO_PIN);
    gpio_output_options_set(LED4_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED4_GPIO_PIN);
	
	  /* LED5引脚模式设置 */
    gpio_mode_set(LED5_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, LED5_GPIO_PIN);
    gpio_output_options_set(LED5_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED5_GPIO_PIN);
  
    /* LED6引脚模式设置 */
    gpio_mode_set(LED6_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, LED6_GPIO_PIN);
    gpio_output_options_set(LED6_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED6_GPIO_PIN);
	  
	  LED1(0);                                    /* 关闭 LED1 */ 
	  LED2(0);                                    /* 关闭 LED2 */
		LED3(0);                                    /* 关闭 LED3 */ 
	  LED4(0);                                    /* 关闭 LED4 */
		LED5(0);                                    /* 关闭 LED5 */ 
	  LED6(0);                                    /* 关闭 LED6 */
}

