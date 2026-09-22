/**
 ****************************************************************************************************
 * @file        fmc.h
 * @version     V1.0
 * @brief       GD32内部FLASH读写 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */

#ifndef __FMC_H
#define __FMC_H

#include "./SYSTEM/sys/sys.h"


/* FMC 扇区的起始地址 */
#define ADDR_FMC_SECTOR_0     ((uint32_t )0x08000000)     /* 扇区0起始地址, 16 Kbytes */
#define ADDR_FMC_SECTOR_1     ((uint32_t )0x08004000)     /* 扇区1起始地址, 16 Kbytes */
#define ADDR_FMC_SECTOR_2     ((uint32_t )0x08008000)     /* 扇区2起始地址, 16 Kbytes */
#define ADDR_FMC_SECTOR_3     ((uint32_t )0x0800C000)     /* 扇区3起始地址, 16 Kbytes */
#define ADDR_FMC_SECTOR_4     ((uint32_t )0x08010000)     /* 扇区4起始地址, 64 Kbytes */
#define ADDR_FMC_SECTOR_5     ((uint32_t )0x08020000)     /* 扇区5起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_6     ((uint32_t )0x08040000)     /* 扇区6起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_7     ((uint32_t )0x08060000)     /* 扇区7起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_8     ((uint32_t )0x08080000)     /* 扇区8起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_9     ((uint32_t )0x080A0000)     /* 扇区9起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_10    ((uint32_t )0x080C0000)     /* 扇区10起始地址,128 Kbytes */
#define ADDR_FMC_SECTOR_11    ((uint32_t )0x080E0000)     /* 扇区11起始地址,128 Kbytes */
#define ADDR_FMC_SECTOR_12    ((uint32_t )0x08100000)     /* 扇区12起始地址, 16 Kbytes */
#define ADDR_FMC_SECTOR_13    ((uint32_t )0x08104000)     /* 扇区13起始地址, 16 Kbytes */
#define ADDR_FMC_SECTOR_14    ((uint32_t )0x08108000)     /* 扇区14起始地址, 16 Kbytes */
#define ADDR_FMC_SECTOR_15    ((uint32_t )0x0810C000)     /* 扇区15起始地址, 16 Kbytes */
#define ADDR_FMC_SECTOR_16    ((uint32_t )0x08110000)     /* 扇区16起始地址, 64 Kbytes */
#define ADDR_FMC_SECTOR_17    ((uint32_t )0x08120000)     /* 扇区17起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_18    ((uint32_t )0x08140000)     /* 扇区18起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_19    ((uint32_t )0x08160000)     /* 扇区19起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_20    ((uint32_t )0x08180000)     /* 扇区20起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_21    ((uint32_t )0x081A0000)     /* 扇区21起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_22    ((uint32_t )0x081C0000)     /* 扇区22起始地址, 128 Kbytes */
#define ADDR_FMC_SECTOR_23    ((uint32_t )0x081E0000)     /* 扇区23起始地址, 128 Kbytes */

#define FMC_START_ADDRESS          FLASH_BASE                                    /* FMC起始地址 */
#define FMC_SIZE                   (*(uint16_t *)0x1FFF7A22U)                    /* FMC容量 */
#define FMC_END_ADDRESS            (FLASH_BASE + (FMC_SIZE * 1024) - 1)          /* FMC结束地址 */

/******************************************************************************************/

uint8_t fmc_sector_get(uint32_t address);                                        /* 获取给定地址所在的fmc扇区 */
uint32_t sector_name_to_number(uint8_t sector_name);                             /* 获取给定扇区的扇区编号 */
uint8_t fmc_erase_sector(uint32_t fmc_sector);                                   /* 擦除扇区 */
void fmc_write_32bit_data(uint32_t address, uint32_t *data_32, uint32_t length); /* 从指定地址开始,写入指定长度的数据 */
void fmc_read_32bit_data(uint32_t address, uint32_t *data_32, uint32_t length);  /* 从指定地址开始,读出指定长度的数据 */

void test_write(uint32_t waddr, uint32_t wdata);                                 /* 测试写数据 */
uint32_t test_read(uint32_t raddr);                                              /* 测试读数据 */

#endif

















