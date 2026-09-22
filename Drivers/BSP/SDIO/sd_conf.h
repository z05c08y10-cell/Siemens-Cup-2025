/**
 ****************************************************************************************************
 * @file        sd_conf.h
 * @version     V1.0
 * @brief       SD卡初始化 驱动代码
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 *
 ****************************************************************************************************
 */
 
#ifndef __SD_CONF_H
#define __SD_CONF_H

#include "./SYSTEM/sys/sys.h"
#include "./BSP/SDIO/sdio_sdcard.h"


extern sd_card_info_struct sd_cardinfo;   /* SD卡信息 */

/******************************************************************************************/

sd_error_enum sdio_sd_init(void);         /* 初始化SD卡 */
void card_info_get(void);                 /* 获取SD卡相关信息 */

sd_error_enum sd_read_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);    /* 读SD卡(fatfs调用) */
sd_error_enum sd_write_disk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);   /* 写SD卡(fatfs调用) */
sd_error_enum sd_read_udisk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);   /* 读SD卡(usb调用) */
sd_error_enum sd_write_udisk(uint8_t *pbuf, uint32_t saddr, uint32_t cnt);  /* 写SD卡(usb调用) */

#endif







