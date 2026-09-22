/**
 ****************************************************************************************************					 
 * @file        usart.c
 * @version     V1.1
 * @brief       串口初始化代码，支持printf            
 ****************************************************************************************************
 *
 * V1.1
 * 修改SYS_SUPPORT_OS部分代码, 包含头文件改成:"os.h"
 *
 ****************************************************************************************************
 */
 
#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"


/* 如果使用os,则包括下面的头文件即可. */
#if SYS_SUPPORT_OS
#include "os.h" /* os 使用 */
#endif

/******************************************************************************************/
/* 加入以下代码, 支持printf函数, 而不需要选择use MicroLIB */

#if 1

#if (__ARMCC_VERSION >= 6010050)            /* 使用AC6编译器时 */
__asm(".global __use_no_semihosting\n\t");  /* 声明不使用半主机模式 */
__asm(".global __ARM_use_no_argv \n\t");    /* AC6下需要声明main函数为无参数格式，否则部分例程可能出现半主机模式 */

#else
/* 使用AC5编译器时, 要在这里定义__FILE 和 不使用半主机模式 */
#pragma import(__use_no_semihosting)

struct __FILE
{
    int handle;
    /* Whatever you require here. If the only file you are using is */
    /* standard output using printf() for debugging, no file handling */
    /* is required. */
};

#endif

/* 不使用半主机模式，至少需要重定义_ttywrch\_sys_exit\_sys_command_string函数,以同时兼容AC6和AC5模式 */
int _ttywrch(int ch)
{
    ch = ch;
    return ch;
}

/* 定义_sys_exit()以避免使用半主机模式 */
void _sys_exit(int x)
{
    x = x;
}

char *_sys_command_string(char *cmd, int len)
{
    return NULL;
}

/* FILE 在 stdio.h里面定义 */
FILE __stdout;

/* 重定义fputc函数, printf函数最终会通过调用fputc输出字符串到串口 */
int fputc(int ch, FILE *f)
{
    while (RESET == usart_flag_get(USART_UX, USART_FLAG_TC));      /* 等待上一个字符发送完成 */
    
    usart_data_transmit(USART_UX, (uint8_t)ch);                    /* 将要发送的字符 ch 写入到数据寄存器 */  
    return ch;
}
#endif

/******************************************************************************************/

#if USART_EN_RX       /* 如果使能了接收 */

/* 接收缓冲, 最大USART_REC_LEN个字节. */
uint8_t g_usart_rx_buf[USART_REC_LEN];

/*  接收状态
 *  bit15，      接收完成标志
 *  bit14，      接收到0x0d
 *  bit13~0，    接收到的有效字节数目
 */
uint16_t g_usart_rx_sta = 0;

/**
 * @brief       串口X中断服务函数
 * @param       无
 * @retval      无
 */
void USART_UX_IRQHandler(void)
{
    uint8_t rxdata;
#if SYS_SUPPORT_OS  /* 如果SYS_SUPPORT_OS为真，则需要支持OS. */
    OSIntEnter();    
#endif
   
		if (usart_interrupt_flag_get(USART_UX, USART_INT_FLAG_RBNE) != RESET)   /* 接收到数据 */
		{
        rxdata = usart_data_receive(USART_UX);	         /* 读取接收到的数据 */
        
        if ((g_usart_rx_sta & 0x8000) == 0)              /* 接收未完成? */
        {
            if (g_usart_rx_sta & 0x4000)                 /* 接收到了0x0d?  */
            { 
                if (rxdata != 0x0a)                      /* 接收到了0x0a? (必须先接收到到0x0d,才检查0x0a) */
                { 
                    g_usart_rx_sta = 0;                  /* 接收错误,重新开始 */
                }
                else 
                {
                    g_usart_rx_sta |= 0x8000;	           /* 收到了0x0a,标记接收完成了 */ 
                }
            }
            else                                         /* 还没收到0X0d  */
            {	
                if (rxdata == 0x0d)
                {
                    g_usart_rx_sta |= 0x4000;            /* 标记接收到了0x0d  */                            
                } 
                else
                {
                    g_usart_rx_buf[g_usart_rx_sta & 0X3FFF] = rxdata;              /* 存储数据到 g_usart_rx_buf */
                    g_usart_rx_sta++;
                  
                    if (g_usart_rx_sta > (USART_REC_LEN - 1))g_usart_rx_sta = 0;   /* 接收数据溢出,重新开始接收 */ 
                }		 
            }
        }   		 
     } 
     
#if SYS_SUPPORT_OS  /* 如果SYS_SUPPORT_OS为真，则需要支持OS. */
    OSIntExit();  											 
#endif
}
#endif

/**
 * @brief       串口X初始化函数
 * @param       bound: 波特率, 根据自己需要设置波特率值
 * @retval      无
 */
void usart_init(uint32_t baudrate)
{
    /* IO 及 时钟配置 */
    rcu_periph_clock_enable(USART_TX_GPIO_CLK);     /* 使能串口TX脚时钟 */
    rcu_periph_clock_enable(USART_RX_GPIO_CLK);     /* 使能串口RX脚时钟 */
    rcu_periph_clock_enable(USART_UX_CLK);          /* 使能串口时钟 */

    /* 设置USARTx_TX引脚的复用功能选择 */
    gpio_af_set(USART_TX_GPIO_PORT, USART_TX_GPIO_AF, USART_TX_GPIO_PIN);

    /* 设置USARTx_RX引脚的复用功能选择 */
    gpio_af_set(USART_RX_GPIO_PORT, USART_RX_GPIO_AF, USART_RX_GPIO_PIN);

    /* USARTx_TX引脚的模式设置 */
    gpio_mode_set(USART_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART_TX_GPIO_PIN);
    gpio_output_options_set(USART_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USART_TX_GPIO_PIN);

    /* USARTx_RX引脚的模式设置 */
    gpio_mode_set(USART_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, USART_RX_GPIO_PIN);
    gpio_output_options_set(USART_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, USART_RX_GPIO_PIN);

    /* 配置USART的参数 */
    usart_deinit(USART_UX);                                 /* 复位USARTx */
    usart_baudrate_set(USART_UX, baudrate);                 /* 设置波特率 */
    usart_stop_bit_set(USART_UX, USART_STB_1BIT);           /* 一个停止位 */
    usart_word_length_set(USART_UX, USART_WL_8BIT);         /* 字长为8位数据格式 */
    usart_parity_config(USART_UX, USART_PM_NONE);           /* 无奇偶校验位 */
    usart_transmit_config(USART_UX, USART_TRANSMIT_ENABLE); /* 使能发送 */
    
#if USART_EN_RX                                             /* 如果使能了接收 */
    usart_receive_config(USART_UX, USART_RECEIVE_ENABLE);   /* 使能接收 */
    usart_interrupt_enable(USART_UX, USART_INT_RBNE);       /* 使能接收缓冲区非空中断 */    
    /* 配置NVIC，并设置中断优先级 */
    nvic_irq_enable(USART_UX_IRQn, 3, 3);                   /* 组2，抢占优先级3，子优先级3 */
#endif

    usart_enable(USART_UX);	                                /* 使能串口 */
}




