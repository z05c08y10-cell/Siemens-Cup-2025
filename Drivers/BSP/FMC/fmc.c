/**
 ****************************************************************************************************
 * @file        fmc.c
 * @version     V1.0
 * @brief       GD32内部FLASH读写 驱动代码
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */
 
#include "./BSP/FMC/fmc.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"


/**
 * @brief       获取给定地址所在的fmc扇区(GD32F470VI)
 * @param       address   : fmc地址(0x08000000~0x081FFFFF)
 * @retval      address所在的扇区
 */
uint8_t fmc_sector_get(uint32_t address)
{
    uint8_t sector_name = 0;
  
    if ((FMC_START_ADDRESS <= address) && (FMC_END_ADDRESS >= address)) 
    {
        if (address < ADDR_FMC_SECTOR_1)       sector_name = 0;            
        else if (address < ADDR_FMC_SECTOR_2)  sector_name = 1;            
        else if (address < ADDR_FMC_SECTOR_3)  sector_name = 2;            
        else if (address < ADDR_FMC_SECTOR_4)  sector_name = 3;              
        else if (address < ADDR_FMC_SECTOR_5)  sector_name = 4;            
        else if (address < ADDR_FMC_SECTOR_6)  sector_name = 5;            
        else if (address < ADDR_FMC_SECTOR_7)  sector_name = 6;            
        else if (address < ADDR_FMC_SECTOR_8)  sector_name = 7;              
        else if (address < ADDR_FMC_SECTOR_9)  sector_name = 8;            
        else if (address < ADDR_FMC_SECTOR_10) sector_name = 9;           
        else if (address < ADDR_FMC_SECTOR_11) sector_name = 10;           
        else if (address < ADDR_FMC_SECTOR_12) sector_name = 11;         
        else if (address < ADDR_FMC_SECTOR_13) sector_name = 12;   
        else if (address < ADDR_FMC_SECTOR_14) sector_name = 13;   
        else if (address < ADDR_FMC_SECTOR_15) sector_name = 14;   
        else if (address < ADDR_FMC_SECTOR_16) sector_name = 15;   
        else if (address < ADDR_FMC_SECTOR_17) sector_name = 16;   
        else if (address < ADDR_FMC_SECTOR_18) sector_name = 17;   
        else if (address < ADDR_FMC_SECTOR_19) sector_name = 18;   
        else if (address < ADDR_FMC_SECTOR_20) sector_name = 19;   
        else if (address < ADDR_FMC_SECTOR_21) sector_name = 20;   
        else if (address < ADDR_FMC_SECTOR_22) sector_name = 21;   
        else if (address < ADDR_FMC_SECTOR_23) sector_name = 22; 
        else sector_name = 23;             
    }
    
    return sector_name;
}

/**
 * @brief       获取给定扇区的扇区编号(用于选择要擦除的扇区)
 * @param       sector_name   : 扇区名(0~23)
 * @retval      扇区的扇区编号
 */
uint32_t sector_name_to_number(uint8_t sector_name)
{
    if (11 >= sector_name)
    {
        return CTL_SN(sector_name);
    }
    else if (23 >= sector_name)
    {
        return CTL_SN(sector_name + 4);
    }
    else if (27 >= sector_name)
    {
        return CTL_SN(sector_name - 12);
    }
    else
    {
        return CTL_SN(15);
    }
}

/**
 * @brief       擦除扇区
 * @param       fmc_sector   : 扇区编号，范围为：CTL_SECTOR_NUMBER_0~CTL_SECTOR_NUMBER_23(sector 0~sector 23)
 *              CTL_SECTOR_NUMBER_0~3, 16K扇区;  CTL_SECTOR_NUMBER_4,64K扇区;  CTL_SECTOR_NUMBER_5~11, 128K扇区.
 *              CTL_SECTOR_NUMBER_12~15, 16K扇区; CTL_SECTOR_NUMBER_16,64K扇区; CTL_SECTOR_NUMBER_17~23, 128K扇区.
 * @retval      执行结果
 *   @arg       FMC_READY : 执行成功
 *   @arg       FMC_BUSY  : 闪存忙标志
 *   @arg       其他      : 错误编号
 */
uint8_t fmc_erase_sector(uint32_t fmc_sector)
{
    fmc_state_enum fmc_state = FMC_READY;
  
    fmc_unlock();   /* 解锁FMC主编程块操作 */
  
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);  /* 清除FMC相关标志位 */
  
    fmc_state = fmc_sector_erase(fmc_sector);   /* FMC扇区擦除 */
  
    fmc_lock();     /* 锁定FMC主编程块操作 */
  
    return fmc_state;
}

/**
 * @brief       从FLASH指定地址开始,写入指定长度的数据(自动擦除)
 * @note        因为GD32F4的扇区实在太大,没办法本地保存扇区数据,所以本函数写地址如果非0XFF
 *              ,那么会先擦除整个扇区且不保存扇区数据.所以写非0XFF的地址,将导致整个扇区数据丢失.
 *              建议写之前确保扇区里没有重要数据,最好是整个扇区先擦除了,然后慢慢往后写.
 * @param       address    : 起始地址(此地址必须为4的倍数!!否则写入出错!)
 * @param       data_32    : 数据指针
 * @param       length     : 要写入的字(32位)数
 * @retval      无
 */
void fmc_write_32bit_data(uint32_t address, uint32_t *data_32, uint32_t length)
{
    fmc_state_enum fmc_state = FMC_READY;
    uint32_t addrx = 0;
    uint32_t endaddr = 0;
    uint32_t i;
    uint32_t sector_num;

    if (address < FMC_START_ADDRESS || address % 4 ||       /* 写入地址小于 FMC_START_ADDRESS, 或不是4的整数倍, 非法. */
        address > (FMC_END_ADDRESS))                        /* 写入地址大于 FMC_END_ADDRESS, 非法. */
    {
        return;
    }
    
    fmc_unlock();                     /* 解锁FMC主编程块操作 */
  
    fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OPERR | FMC_FLAG_WPERR | FMC_FLAG_PGMERR | FMC_FLAG_PGSERR);  /* 清除FMC相关标志位 */

    addrx = address;                  /* 写入的起始地址 */
    endaddr = address + length * 4;   /* 写入的结束地址 */

    if (addrx < 0X1FFF0000)           /* 只有主存储区,才需要执行擦除操作!! */
    {
        while (addrx < endaddr)       /* 扫清一切障碍.(对非FFFFFFFF的地方,先擦除) */
        {
            if (*(volatile uint32_t *)addrx != 0XFFFFFFFF)    /* 有非0XFFFFFFFF的地方,要擦除这个扇区 */
            {
                sector_num = sector_name_to_number(fmc_sector_get(addrx)); /* 获取扇区编号 */ 
              
                fmc_state = fmc_sector_erase(sector_num);     /* 擦除扇区 */

                if (FMC_READY != fmc_state)             
                {
                    break;                                    /* 发生错误 */
                }               
            }
            else
            {
                addrx += 4;
            }
        }
    }

    if (FMC_READY == fmc_state)
    {    
        for (i = 0; i < length; i++)
        {
            if (FMC_READY != fmc_word_program(address, data_32[i])) /* 写入数据 */
            {
                 break;        /* 写入异常 */  
            }
            
            address += 4;
        }
    }

    fmc_lock();                /* 锁定FMC主编程块操作 */
}

/**
 * @brief       从指定地址开始,读出指定长度的数据
 * @param       address    : 起始地址
 * @param       data_32    : 数据指针
 * @param       length     : 要读取的字(32位)数
 * @retval      无
 */
void fmc_read_32bit_data(uint32_t address, uint32_t *data_32, uint32_t length)
{ 
    uint32_t i;
  
    for (i = 0; i < length; i++)
    {
        data_32[i] = *(volatile uint32_t *)address;     /* 读取4个字节(1个字) */
        address += 4;                                   /* 偏移4个字节 */
    }
}

/******************************************************************************************/
/* 测试用代码 */

/**
 * @brief       测试写数据(写1个字)
 * @param       waddr : 起始地址
 * @param       wdata : 要写入的数据
 * @retval      无
 */
void test_write(uint32_t waddr, uint32_t wdata)
{
    fmc_write_32bit_data(waddr, &wdata, 1);       /* 写入一个字 */
}

/**
 * @brief       测试读数据(读1个字)
 * @param       raddr : 读取地址
 * @retval      读取到的数据 (32位)
 */
uint32_t test_read(uint32_t raddr)
{
    return *(volatile uint32_t *)raddr;           /* 读取一个字 */
}











