#include "./BSP/TIMER/timer.h"
#include "./BSP/LED/led.h"

//volatile uint32_t timer6_counter = 0;     // 定时器计数器
//volatile uint8_t timer6_sample_flag = 0;  // 采样标志
//volatile uint8_t timer6_led_flag = 0;     // LED闪烁标志

/**
 * @brief       定时器TIMER6定时中断初始化函数
 */
void timerx_int_init(uint32_t arr, uint16_t psc)
{
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(TIMERX_INT_CLK);
    timer_deinit(TIMERX_INT);
    timer_struct_para_init(&timer_initpara);

    timer_initpara.prescaler         = psc;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = arr;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMERX_INT, &timer_initpara);

    timer_interrupt_flag_clear(TIMERX_INT, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(TIMERX_INT, TIMER_INT_UP);
    nvic_irq_enable(TIMERX_INT_IRQn, 1, 3);
}

/**
 * @brief       设置定时器周期
 * @param       period_ms: 周期(毫秒)
 * @retval      无
 */
void timerx_set_period(uint32_t period_ms)
{
    /* 计算预分频和重载值 */
    uint32_t clock_freq = 240000000;  // 系统时钟240MHz
    uint32_t psc = 239;               // 预分频值 (240MHz/(239+1)=1MHz)
    uint32_t arr = (period_ms * 1000) - 1;  // 重载值 (1MHz -> 1us)
    
    timer_autoreload_value_config(TIMER6, arr);
}

/**
 * @brief       使能定时器
 * @param       无
 * @retval      无
 */
void timerx_enable(void)
{
    timer_enable(TIMER6);
}

/**
 * @brief       禁用定时器
 * @param       无
 * @retval      无
 */
void timerx_disable(void)
{
    timer_disable(TIMER6);
}

