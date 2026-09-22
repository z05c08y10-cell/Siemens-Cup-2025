/**
 ****************************************************************************************************
 * @file        key.c
 * @version     V1.0
 * @brief       按键输入 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */
 
#include "./BSP/KEY/key.h"
#include "./SYSTEM/delay/delay.h"


/**
 * @brief       按键初始化函数
 * @param       无
 * @retval      无
 */
void key_init(void)
{
    rcu_periph_clock_enable(KEY1_GPIO_CLK);     /* 使能KEY1时钟 */
		rcu_periph_clock_enable(KEY2_GPIO_CLK);     /* 使能KEY2时钟 */
		rcu_periph_clock_enable(KEY3_GPIO_CLK);     /* 使能KEY3时钟 */
		rcu_periph_clock_enable(KEY4_GPIO_CLK);     /* 使能KEY4时钟 */
		rcu_periph_clock_enable(KEY5_GPIO_CLK);     /* 使能KEY5时钟 */
		rcu_periph_clock_enable(KEY6_GPIO_CLK);     /* 使能KEY6时钟 */
	  rcu_periph_clock_enable(WKUP_GPIO_CLK);     /* 使能WKUP时钟 */
    
    gpio_mode_set(KEY1_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY1_GPIO_PIN);   /* KEY1引脚模式设置,上拉输入 */
		gpio_mode_set(KEY2_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY2_GPIO_PIN);   /* KEY2引脚模式设置,上拉输入 */
		gpio_mode_set(KEY3_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY3_GPIO_PIN);   /* KEY3引脚模式设置,上拉输入 */
		gpio_mode_set(KEY4_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY4_GPIO_PIN);   /* KEY4引脚模式设置,上拉输入 */
		gpio_mode_set(KEY5_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY5_GPIO_PIN);   /* KEY5引脚模式设置,上拉输入 */
		gpio_mode_set(KEY6_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY6_GPIO_PIN);   /* KEY6引脚模式设置,上拉输入 */
	  gpio_mode_set(WKUP_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, WKUP_GPIO_PIN);   /* WKUP引脚模式设置,上拉输入 */  	
}

/**
 * @brief       按键扫描函数
 * @note        该函数有响应优先级(同时按下多个按键): WK_UP > KEY6 > KEY5 > KEY4 > KEY3 > KEY2 > KEY1!!
 * @param       mode:0 / 1, 具体含义如下:
 *   @arg       0,  不支持连续按(当按键按下不放时, 只有第一次调用会返回键值,
 *                  必须松开以后, 再次按下才会返回其他键值)
 *   @arg       1,  支持连续按(当按键按下不放时, 每次调用该函数都会返回键值)
 * @retval      键值, 定义如下:
 *              KEY1_PRES, 1, KEY1按下
 *              KEY2_PRES, 2, KEY2按下
 *              KEY3_PRES, 3, KEY3按下
 *              KEY4_PRES, 4, KEY4按下
 *              KEY5_PRES, 5, KEY5按下
 *              KEY6_PRES, 6, KEY6按下
 *              WKUP_PRES, 7, WKUP按下
 */
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;  /* 按键按松开标志 */
    uint8_t keyval = 0;

    if (mode) key_up = 1;       /* 支持连按 */
  
    if (key_up && (KEY1 == 0 || KEY2 == 0 || KEY3 == 0 || KEY4 == 0 || KEY5 == 0 || KEY6 == 0 || WK_UP == 0))     /* 按键松开标志为1, 且有任意一个按键按下了 */
    {
        delay_ms(10);           /* 去抖动 */
        key_up = 0;

        if (KEY1 == 0)  keyval = KEY1_PRES;
			  if (KEY2 == 0)  keyval = KEY2_PRES;
				if (KEY3 == 0)  keyval = KEY3_PRES;
				if (KEY4 == 0)  keyval = KEY4_PRES;
				if (KEY5 == 0)  keyval = KEY5_PRES;
				if (KEY6 == 0)  keyval = KEY6_PRES;
			  if (WK_UP == 0) keyval = WKUP_PRES;
    }
    else if (KEY1 == 1 && KEY2 == 1 && KEY3 == 1 && KEY4 == 1 && KEY5 == 1 && KEY6 == 1 && WK_UP == 1)            /* 没有任何按键按下, 标记按键松开 */
    {
        key_up = 1;
    }

    return keyval;              /* 返回键值 */
}


