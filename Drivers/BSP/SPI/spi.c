/**
 ****************************************************************************************************
 * @file        spi.c
 * @version     V1.0
 * @brief       SPI 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */
 
#include "./BSP/SPI/spi.h"


/**
 * @brief       SPI初始化代码
 * @note        主机模式,8位数据,禁止硬件片选
 * @param       无
 * @retval      无
 */
void spi1_init(void)
{
	  spi_parameter_struct spi_init_struct;

    rcu_periph_clock_enable(SPI1_SCK_GPIO_CLK);        /* 使能SPI1_SCK IO口时钟 */
    rcu_periph_clock_enable(SPI1_MISO_GPIO_CLK);       /* 使能SPI1_MISO IO口时钟 */
    rcu_periph_clock_enable(SPI1_MOSI_GPIO_CLK);       /* 使能SPI1_MOSI IO口时钟 */
    rcu_periph_clock_enable(SPI1_SPI_CLK);             /* 使能SPI1时钟 */  

    /* 配置使用的SPI1引脚:
             SPI1_SCK->PB13
             SPI1_MISO->PB14
             SPI1_MOSI->PB15 */

    /* 配置SPI1引脚的复用功能 */
    gpio_af_set(SPI1_SCK_GPIO_PORT, SPI1_SCK_GPIO_AF, SPI1_SCK_GPIO_PIN);  
    gpio_af_set(SPI1_MISO_GPIO_PORT, SPI1_MISO_GPIO_AF, SPI1_MISO_GPIO_PIN);
    gpio_af_set(SPI1_MOSI_GPIO_PORT, SPI1_MOSI_GPIO_AF, SPI1_MOSI_GPIO_PIN);

    /* SPI1_SCK引脚模式设置 复用推挽输出 */
    gpio_mode_set(SPI1_SCK_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI1_SCK_GPIO_PIN);
    gpio_output_options_set(SPI1_SCK_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI1_SCK_GPIO_PIN);

    /* SPI1_MISO引脚模式设置 复用推挽输出 */
    gpio_mode_set(SPI1_MISO_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI1_MISO_GPIO_PIN);
    gpio_output_options_set(SPI1_MISO_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI1_MISO_GPIO_PIN);

    /* SPI1_MOSI引脚模式设置 复用推挽输出 */
    gpio_mode_set(SPI1_MOSI_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, SPI1_MOSI_GPIO_PIN);
    gpio_output_options_set(SPI1_MOSI_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_MAX, SPI1_MOSI_GPIO_PIN);

    spi_i2s_deinit(SPI1_SPI);                                          /* 复位SPI1 */
    spi_struct_para_init(&spi_init_struct);                            /* 初始化SPI结构体中所有参数为默认值 */
    
	  spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;   /* 传输模式配置:设置为全双工模式 */
    spi_init_struct.device_mode          = SPI_MASTER;                 /* 配置为主机模式 */
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;         /* 数据帧格式配置;8位数据帧格式 */ 
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;    /* 空闲状态下,CLK保持高电平,在第二个时钟跳变沿采集第一个数据 */
    spi_init_struct.nss                  = SPI_NSS_SOFT;               /* NSS软件模式,NSS电平取决于SWNSS位 */
    spi_init_struct.prescale             = SPI_PSC_256;                /* 预分频器配置:默认使用256分频,速度最低 */
    spi_init_struct.endian               = SPI_ENDIAN_MSB;             /* 大端或小端模式配置;先发送最高有效位 */
    spi_init(SPI1_SPI, &spi_init_struct);                              /* 初始化SPI1 */
	  
    spi_enable(SPI1_SPI);	                                             /* 使能SPI1 */	
}

/**
 * @brief       SPI1速度设置函数
 * @note        SPI1时钟选择来自APB1, 即PCLK1, 为60Mhz
 *              SPI速度 = PCLK1 / 2^(speed + 1)
 * @param       speed   : SPI时钟分频系数
 * @retval      无
 */
void spi1_set_speed(uint8_t speed)
{
	  speed &= 0X07;                      /* 限制范围 */
	  spi_disable(SPI1_SPI);	            /* SPI失能 */
	  SPI_CTL0(SPI1_SPI) &= ~(7 << 3);    /* 先清零 */
	  SPI_CTL0(SPI1_SPI) |= speed << 3;   /* 设置分频系数 */
	  spi_enable(SPI1_SPI);	              /* SPI使能 */
}

/**
 * @brief       SPI1读写一个字节数据
 * @param       txdata  : 要发送的数据(1字节)
 * @retval      接收到的数据(1字节)
 */
uint8_t spi1_read_write_byte(uint8_t txdata)
{   
	  while(RESET == spi_i2s_flag_get(SPI1_SPI, SPI_FLAG_TBE));    /* 等待发送缓冲区空 */
	
	  spi_i2s_data_transmit(SPI1_SPI, txdata);                     /* 发送一个字节 */
	
	  while(RESET == spi_i2s_flag_get(SPI1_SPI, SPI_FLAG_RBNE));   /* 等待接收缓冲区非空 */
	
	  return spi_i2s_data_receive(SPI1_SPI);                       /* 返回收到的数据 */
}






