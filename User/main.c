/****************************************************************************************************
 * @file        main.c
 * @version     V2.0
 * @brief       工业嵌入式系统 - 电压监测系统
 ****************************************************************************************************
 *
 * 实验平台:    GD32F470VET6
 * 功能说明:    电压采集、处理、显示、存储系统
 *             支持自检、时间设置、配置管理、采样控制、数据处理和存储功能
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/OLED/oled.h"
#include "./BSP/KEY/key.h"
#include "./BSP/RTC/rtc.h"
#include "./BSP/ADC/adc.h"
#include "./BSP/SDIO/sd_conf.h"
#include "./BSP/NORFLASH/norflash.h"
#include "./FATFS/exfuns/exfuns.h"  
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "./BSP/TIMER/timer.h"

/* 全局变量定义 */
typedef struct {
    float ratio;         // 变比系数
    float limit;         // 电压阈值
    uint8_t sample_cycle; // 采样周期(5/10/15)
    uint32_t log_file_id; // 日志文件ID
} SystemConfig;

SystemConfig sys_config = {1.0, 100.0, 5, 0}; // 默认配置
uint8_t sampling_enabled = 0;   // 采样状态标志
uint8_t sample_count = 0;       // 当前文件采样计数
uint8_t encrypt_mode = 0;       // 加密存储模式
FIL sample_file, overlimit_file, log_file, hide_file; // 文件对象
char current_filename[50];      // 当前文件名缓存
uint32_t seconds_counter = 0;   // 秒计数器
uint8_t led_blink_state = 0;    // LED闪烁状态
uint32_t last_sample_time = 0;  // 上次采样时间
/* 在全局变量定义部分添加 */
uint8_t hide_output_mode = 0;   // 隐藏输出模式标志

/* 文件打开状态标志 */
uint8_t sample_file_open = 0;
uint8_t overlimit_file_open = 0;
uint8_t log_file_open = 0;
uint8_t hide_file_open = 0;

/* 时间结构体 */
typedef struct {
    uint8_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    uint8_t week;
} rtc_time_t;

/* 函数声明 */
void system_self_test(void);
void set_rtc_time(char *datetime);
void read_config_file(void);
void set_ratio(void);
void set_limit(void);
void save_config_to_flash(void);
void read_config_from_flash(void);
void start_sampling(void);
void stop_sampling(void);
void adjust_sample_cycle(uint8_t cycle);
void process_adc_sample(void);
void store_sample_data(float voltage, uint8_t overlimit);
void store_overlimit_data(float voltage);
void store_log_entry(const char *action);
void store_hide_data(uint32_t timestamp, float voltage);
uint32_t datetime_to_unix(rtc_time_t *time);
void encode_voltage(float voltage, uint8_t *buffer);
void oled_display_status(void);
void get_current_time(rtc_time_t *time);
uint32_t get_system_time(void);

/* 添加以下代码到timer.c文件末尾 */

volatile uint32_t timer6_counter = 0;  // 定时器计数器
volatile uint8_t timer6_sample_flag = 0; // 采样标志
volatile uint8_t timer6_led_flag = 0;    // LED闪烁标志
/* 在全局变量定义部分添加 */
volatile uint8_t timer6_time_update_flag = 0; // 时间更新标志
/**
 * @brief       TIMER6中断服务函数
 * @param       无
 * @retval      无
 */
void TIMER6_IRQHandler(void)
{
    if(timer_interrupt_flag_get(TIMER6, TIMER_INT_FLAG_UP) != RESET)
    {
        timer_interrupt_flag_clear(TIMER6, TIMER_INT_FLAG_UP);
        
        timer6_counter++;
        
        /* 1秒LED闪烁控制 */
        if(timer6_counter % 1000 == 0) {
            timer6_led_flag = 1;
					timer6_time_update_flag = 1; // 新增时间更新标志
        }
        
        /* 采样周期控制 */
        if(sampling_enabled && (timer6_counter % (sys_config.sample_cycle * 1000) == 0)) {
            timer6_sample_flag = 1;
        }
    }
}
float oled_voltage;
/**
 * @brief       更新OLED时间显示
 * @param       无
 * @retval      无
 */
void update_oled_time(void)
{
		uint16_t adcx;
		float temp;
		rtc_time_t current_time;
		get_current_time(&current_time);

		char time_str[12];
		sprintf(time_str, "%02d:%02d:%02d", 
						current_time.hour, current_time.minute, current_time.second);


		oled_show_string(0, 0, time_str, 12);


		adcx = adc_get_result_average(ADC_ADCX_CHY, 20);    /* 获取通道CHY的转换值，20次取平均 */
		oled_voltage = (float)adcx * (3.3 / 4096);         /* 计算实际电压值 */

		char volt_str[16];
		sprintf(volt_str, "%.2fV", oled_voltage);               /* 格式化为字符串，保留两位小数 */

		oled_show_string(0, 16, volt_str, 12);             /* 在指定位置(0,16)显示字符串，字体大小12 */
		oled_refresh_gram();                                /* 刷新显示 */

		oled_refresh_gram();
}

/**
 * @brief       获取系统时间（毫秒）
 * @param       无
 * @retval      当前系统时间（毫秒）
 */
uint32_t get_system_time(void) {
    static uint32_t counter = 0;
    static uint32_t last_tick = 0;
    uint32_t current_tick = sys_tick_get();
    
    counter += (current_tick - last_tick);
    last_tick = current_tick;
    return counter;
}

/**
 * @brief       获取当前时间
 * @param       time: 时间结构体指针
 * @retval      无
 */
void get_current_time(rtc_time_t *time)
{
    uint8_t ampm;
    rtc_get_date(&time->year, &time->month, &time->day, &time->week);
    rtc_get_time(&time->hour, &time->minute, &time->second, &ampm);
}

/**
 * @brief       系统自检
 * @param       无
 * @retval      无
 */
void system_self_test(void)
{
    uint16_t flash_id = norflash_read_id();
    uint8_t sd_status = (SD_OK == sdio_sd_init()) ? 1 : 0;
    rtc_time_t current_time;
    get_current_time(&current_time);
    
    printf("=====system selftest=====\r\n");
    printf("flash.............%s\r\n", (flash_id == GD25Q40) ? "ok" : "error");
    printf("TF card............%s\r\n", sd_status ? "ok" : "error");
    printf("flash ID:0x%04X\r\n", flash_id);
    
    if(sd_status) {
        uint32_t capacity = sd_card_capacity_get() >> 10; // KB
        printf("TF card memory:%lu KB\r\n", capacity);
    } else {
        printf("can not find TF card\r\n");
    }
    
    printf("RTC:20%02d-%02d-%02d %02d:%02d:%02d\r\n", 
           current_time.year, current_time.month, current_time.day,
           current_time.hour, current_time.minute, current_time.second);
    printf("=====system selftest=====\r\n");
}

/**
 * @brief       设置RTC时间
 * @param       datetime: 时间字符串(格式:2025-01-01 12:00:30)
 * @retval      无
 */
void set_rtc_time(char *datetime)
{
    int year, month, day, hour, min, sec;
    sscanf(datetime, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
    
    rtc_set_date(year, month, day, 0); // 星期设为0
    rtc_set_time(hour, min, sec, 0);   // AM/PM设为0 (24小时制)
    
    printf("RTC config success\r\n");
    printf("Time:20%02d-%02d-%02d %02d:%02d:%02d\r\n", year, month, day, hour, min, sec);
}

/**
 * @brief       读取配置文件
 * @param       无
 * @retval      无
 */
void read_config_file(void)
{
    FIL file;
    FRESULT res;
    char line[64];
    
    res = f_open(&file, "config.ini", FA_READ);
    if (res != FR_OK) {
        printf("config.ini file not found.\r\n");
        return;
    }
    
    while (f_gets(line, sizeof(line), &file)) {
        if (strstr(line, "Ratio")) {
            sscanf(line, "Ch0 = %f", &sys_config.ratio);
            printf("Ratio = %.1f\r\n", sys_config.ratio);
        } else if (strstr(line, "Limit")) {
            sscanf(line, "ch0 = %f", &sys_config.limit);
            printf("Limit = %.1f\r\n", sys_config.limit);
        }
    }
    
    f_close(&file);
    printf("config read success\r\n");
}

/**
 * @brief       设置变比
 * @param       无
 * @retval      无
 */
void set_ratio(void)
{
    printf("Ratio = %.1f\r\n", sys_config.ratio);
    printf("Input value(0-100):\r\n");
    
    /* 等待用户输入 */
    uint8_t key;
    char input[16] = {0};
    uint8_t index = 0;
    
    while (1) {
        key = key_scan(0);
        if (key) {
            if (key == KEY1_PRES) {  // 回车
                float new_ratio = atof(input);
                if (new_ratio >= 0 && new_ratio <= 100) {
                    sys_config.ratio = new_ratio;
                    printf("Ratio modified success\r\n");
                    printf("Ratio = %.1f\r\n", sys_config.ratio);
                    store_log_entry("Ratio modified");
                } else {
                    printf("Ratio invalid\r\n");
                    printf("Ratio = %.1f\r\n", sys_config.ratio);
                }
                break;
            } else if (key == KEY2_PRES) {  // 退格
                if (index > 0) input[--index] = '\0';
            } else {  // 数字输入
                if (index < 15) {
                    input[index++] = '0' + (key - 1);
                    input[index] = '\0';
                }
            }
        }
        delay_ms(10);
    }
}

/**
 * @brief       设置阈值
 * @param       无
 * @retval      无
 */
void set_limit(void)
{
    printf("Limit = %.1f\r\n", sys_config.limit);
    printf("Input value(0-500):\r\n");
    
    /* 等待用户输入 */
    uint8_t key;
    char input[16] = {0};
    uint8_t index = 0;
    
    while (1) {
        key = key_scan(0);
        if (key) {
            if (key == KEY1_PRES) {  // 回车
                float new_limit = atof(input);
                if (new_limit >= 0 && new_limit <= 500) {
                    sys_config.limit = new_limit;
                    printf("Limit modified success\r\n");
                    printf("Limit = %.1f\r\n", sys_config.limit);
                    store_log_entry("Limit modified");
                } else {
                    printf("Limit invalid\r\n");
                    printf("Limit = %.1f\r\n", sys_config.limit);
                }
                break;
            } else if (key == KEY2_PRES) {  // 退格
                if (index > 0) input[--index] = '\0';
            } else {  // 数字输入
                if (index < 15) {
                    input[index++] = '0' + (key - 1);
                    input[index] = '\0';
                }
            }
        }
        delay_ms(10);
    }
}

/**
 * @brief       保存配置到Flash
 * @param       无
 * @retval      无
 */
void save_config_to_flash(void)
{
    printf("ratio = %.1f\r\n", sys_config.ratio);
    printf("limit: %.2f\r\n", sys_config.limit);
    
    /* 擦除Flash扇区并写入配置 */
    norflash_erase_sector(0);
    norflash_write((uint8_t*)&sys_config, 0, sizeof(sys_config));
    
    printf("save parameters to flash\r\n");
    store_log_entry("Config saved");
}

/**
 * @brief       从Flash读取配置
 * @param       无
 * @retval      无
 */
void read_config_from_flash(void)
{
    SystemConfig config;
    norflash_read((uint8_t*)&config, 0, sizeof(config));
    
    /* 验证数据有效性 */
    if (config.ratio >= 0 && config.ratio <= 100 && 
        config.limit >= 0 && config.limit <= 500) {
        sys_config = config;
    }
    
    printf("read parameters from flash\r\n");
    printf("ratio:%.1f\r\n", sys_config.ratio);
    printf("limit:%.2f\r\n", sys_config.limit);
}

/**
 * @brief       开始采样
 * @param       无
 * @retval      无
 */
void start_sampling(void)
{
    sampling_enabled = 1;
    led_blink_state = 1;
    LED1(1);  // 初始点亮
    
    timer6_counter = 0;  // 重置定时器计数器
    timer6_sample_flag = 0;
    timer6_led_flag = 0;
    timer6_time_update_flag = 0;
    
    /* 初始化OLED显示 */
    rtc_time_t current_time;
    get_current_time(&current_time);
    
    char time_str[12], volt_str[16];
    sprintf(time_str, "%02d:%02d:%02d", 
            current_time.hour, current_time.minute, current_time.second);
    sprintf(volt_str, "0.00V");  // 初始电压值
    
    oled_clear();
    oled_show_string(0, 0, time_str, 12);
    oled_show_string(0, 16, volt_str, 12);
    oled_refresh_gram();
    
    /* 启用定时器 */
    timer_enable(TIMER6);
    
    printf("Periodic Sampling\r\n");
    printf("sample cycle: %ds\r\n", sys_config.sample_cycle);
    
    store_log_entry("Sampling started");
}

/**
 * @brief       停止采样
 * @param       无
 * @retval      无
 */
void stop_sampling(void)
{
    sampling_enabled = 0;
    LED1(0);
    LED2(0);
    
    /* 禁用定时器 */
    timer_disable(TIMER6);
    
    oled_clear();
    oled_show_string(0, 0, "system idle", 12);
    oled_refresh_gram();
    
    printf("Periodic Sampling STOP\r\n");
    
    /* 关闭所有打开的文件 */
    if (sample_file_open) {
        f_close(&sample_file);
        sample_file_open = 0;
    }
    if (overlimit_file_open) {
        f_close(&overlimit_file);
        overlimit_file_open = 0;
    }
    if (hide_file_open) {
        f_close(&hide_file);
        hide_file_open = 0;
    }
    
    store_log_entry("Sampling stopped");
}

/**
 * @brief       调整采样周期
 * @param       cycle: 采样周期(5/10/15)
 * @retval      无
 */
void adjust_sample_cycle(uint8_t cycle)
{
   sys_config.sample_cycle = cycle;
	
	/* 重新配置定时器 */
    timer_disable(TIMERX_INT);
    timer_deinit(TIMERX_INT);
	
	/* 根据不同周期设置定时器 */
//    switch(cycle) {
//        case 5:
//            timerx_int_init(2000-1, 120-1);  // 240MHz/(239+1)=1MHz, (999+1)/1MHz=1ms
//            break;
//        case 10:
//            timerx_int_init(1500-1, 120-1); // 10秒周期
//            break;
//        case 15:
//            timerx_int_init(1000-1, 120-1); // 15秒周期
//            break;
//    }
	switch(cycle) {
        case 5:
					  timer_disable(TIMERX_INT);
    timer_deinit(TIMERX_INT);
            timerx_int_init(1500-1, 120-1);  // 5秒周期
            break;
        case 10:
					timer_disable(TIMERX_INT);
    timer_deinit(TIMERX_INT);
            timerx_int_init(1000-1, 120-1); // 10秒周期
            break;
        case 15:
					timer_disable(TIMERX_INT);
    timer_deinit(TIMERX_INT);
            timerx_int_init(1000-1, 120-1); // 15秒周期
            break;
    }
    
    if(sampling_enabled) {
        timer_enable(TIMERX_INT);
    }
		
    printf("sample cycle adjust: %ds\r\n", cycle);
    store_log_entry("Sample cycle adjusted");
    
    /* 保存新周期到Flash */
    save_config_to_flash();
}


/**
 * @brief       处理ADC采样
 * @param       无
 * @retval      无
 */
void process_adc_sample(void)
{
    rtc_time_t current_time;
    get_current_time(&current_time);
    
    /* 读取ADC值并转换为电压 */
//    uint16_t adc_value = adc_get_result_average(ADC_ADCX_CHY, 20);
//    float voltage = (float)adc_value * (3.3 / 4096) * sys_config.ratio;
//    
//    /* OLED显示 - 只更新电压部分 */
//    char volt_str[16];
//    sprintf(volt_str, "%.2fV", voltage);
//    
//    oled_show_string(0, 16, volt_str, 12);
//    oled_refresh_gram();
    
    /* 串口输出 */
    printf("20%02d-%02d-%02d %02d:%02d:%02d ch0 = %.2fV", 
           current_time.year, current_time.month, current_time.day,
           current_time.hour, current_time.minute, current_time.second,
           oled_voltage);
    
    /* 超限检测 */
    uint8_t overlimit = 0;
    if (oled_voltage > sys_config.limit/10) {
        LED2(1);
        overlimit = 1;
        printf(" OverLimit(%.2f)!", sys_config.limit);
        
        /* 存储超限数据 */
        store_overlimit_data(oled_voltage);
    }
		else if(oled_voltage <= sys_config.limit/10)
		{
			LED2(0);
		}

    printf("\r\n");
    
    /* 存储采样数据 */
    store_sample_data(oled_voltage, overlimit);
    
    /* 存储加密数据 */
    if (encrypt_mode) {
        uint32_t unix_time = datetime_to_unix(&current_time);
        store_hide_data(unix_time, oled_voltage);
    }
		
		/* 隐藏模式输出 */
    if (hide_output_mode) {
        /* 转换为Unix时间戳 */
        uint32_t unix_time = datetime_to_unix(&current_time);
        
        /* 电压值分解 */
        uint16_t integer_part = (uint16_t)oled_voltage;
        uint16_t fractional_part = (uint16_t)((oled_voltage - integer_part) * 65536);
        
        /* 生成隐藏格式字符串 */
        char hide_str[20];
        sprintf(hide_str, "%08X%04X%04X", unix_time, integer_part, fractional_part);
        
        /* 添加超限标记 */
        if (overlimit) {
            printf("%s*\r\n", hide_str);
        } else {
            printf("%s\r\n", hide_str);
        }
    }
}

/**
 * @brief       存储采样数据
 * @param       voltage: 电压值
 * @param       overlimit: 是否超限
 * @retval      无
 */
void store_sample_data(float voltage, uint8_t overlimit)
{
    /* 每10条数据创建新文件 */
    if (sample_count == 0 || sample_count >= 10) {
        if (sample_file_open) {
            f_close(&sample_file);
            sample_file_open = 0;
        }
        
        rtc_time_t current_time;
        get_current_time(&current_time);
        
        sprintf(current_filename, "sample/sampleData20%02d%02d%02d%02d%02d%02d.txt",
                current_time.year, current_time.month, current_time.day,
                current_time.hour, current_time.minute, current_time.second);
        
        f_mkdir("sample");
        if (f_open(&sample_file, current_filename, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
            sample_file_open = 1;
        }
        sample_count = 0;
    }
    
    /* 写入数据 */
    rtc_time_t current_time;
    get_current_time(&current_time);
    
    char data_line[64];
    sprintf(data_line, "20%02d-%02d-%02d %02d:%02d:%02d,%.2f,%d\r\n",
            current_time.year, current_time.month, current_time.day,
            current_time.hour, current_time.minute, current_time.second,
            voltage, overlimit);
    
    UINT bw;
    if (sample_file_open) {
        f_write(&sample_file, data_line, strlen(data_line), &bw);
        f_sync(&sample_file);
    }
    
    sample_count++;
}

/**
 * @brief       存储超限数据
 * @param       voltage: 电压值
 * @retval      无
 */
void store_overlimit_data(float voltage)
{
    /* 每10条数据创建新文件 */
    static uint8_t overlimit_count = 0;
    
    if (overlimit_count == 0 || overlimit_count >= 10) {
        if (overlimit_file_open) {
            f_close(&overlimit_file);
            overlimit_file_open = 0;
        }
        
        rtc_time_t current_time;
        get_current_time(&current_time);
        
        sprintf(current_filename, "overLimit/overLimit%04d%02d%02d%02d%02d%02d.txt",
                current_time.year, current_time.month, current_time.day,
                current_time.hour, current_time.minute, current_time.second);
        
        f_mkdir("overLimit");
        if (f_open(&overlimit_file, current_filename, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
            overlimit_file_open = 1;
        }
        overlimit_count = 0;
    }
    
    /* 写入数据 */
    rtc_time_t current_time;
    get_current_time(&current_time);
    
    char data_line[64];
    sprintf(data_line, "20%02d-%02d-%02d %02d:%02d:%02d,%.2f\r\n",
            current_time.year, current_time.month, current_time.day,
            current_time.hour, current_time.minute, current_time.second,
            voltage);
    
    UINT bw;
    if (overlimit_file_open) {
        f_write(&overlimit_file, data_line, strlen(data_line), &bw);
        f_sync(&overlimit_file);
    }
    
    overlimit_count++;
}

/**
 * @brief       存储日志条目
 * @param       action: 操作描述
 * @retval      无
 */
void store_log_entry(const char *action)
{
    static uint8_t log_initialized = 0;
    
    /* 上电时创建新日志文件 */
    if (!log_initialized) {
        sprintf(current_filename, "log/log%lu.txt", sys_config.log_file_id);
        f_mkdir("log");
        if (f_open(&log_file, current_filename, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
            log_file_open = 1;
            log_initialized = 1;
        }
        
        /* 写入初始信息 */
        if (log_file_open) {
            rtc_time_t current_time;
            get_current_time(&current_time);
            
            char header[128];
            sprintf(header, "System started at 20%02d-%02d-%02d %02d:%02d:%02d\r\n",
                    current_time.year, current_time.month, current_time.day,
                    current_time.hour, current_time.minute, current_time.second);
            
            UINT bw;
            f_write(&log_file, header, strlen(header), &bw);
            f_sync(&log_file);
        }
    }
    
    /* 写入日志条目 */
    if (log_file_open) {
        rtc_time_t current_time;
        get_current_time(&current_time);
        
        char log_entry[128];
        sprintf(log_entry, "[20%02d-%02d-%02d %02d:%02d:%02d] %s\r\n",
                current_time.year, current_time.month, current_time.day,
                current_time.hour, current_time.minute, current_time.second,
                action);
        
        UINT bw;
        f_write(&log_file, log_entry, strlen(log_entry), &bw);
        f_sync(&log_file);
    }
}

/**
 * @brief       存储加密数据
 * @param       timestamp: Unix时间戳
 * @param       voltage: 电压值
 * @retval      无
 */
void store_hide_data(uint32_t timestamp, float voltage)
{
    /* 每10条数据创建新文件 */
    static uint8_t hide_count = 0;
    
    if (hide_count == 0 || hide_count >= 10) {
        if (hide_file_open) {
            f_close(&hide_file);
            hide_file_open = 0;
        }
        
        rtc_time_t current_time;
        get_current_time(&current_time);
        
        sprintf(current_filename, "hideData/hideData%04d%02d%02d%02d%02d%02d.txt",
                current_time.year, current_time.month, current_time.day,
                current_time.hour, current_time.minute, current_time.second);
        
        f_mkdir("hideData");
        if (f_open(&hide_file, current_filename, FA_CREATE_ALWAYS | FA_WRITE) == FR_OK) {
            hide_file_open = 1;
        }
        hide_count = 0;
    }
    
    /* 编码电压值 */
    uint8_t voltage_buf[8];
    encode_voltage(voltage, voltage_buf);
    
    /* 写入数据 */
    char data_line[32];
    sprintf(data_line, "%08X", timestamp);
    for (int i = 0; i < 8; i++) {
        sprintf(data_line + 8 + i*2, "%02X", voltage_buf[i]);
    }
    strcat(data_line, "\r\n");
    
    UINT bw;
    if (hide_file_open) {
        f_write(&hide_file, data_line, strlen(data_line), &bw);
        f_sync(&hide_file);
    }
    
    hide_count++;
}

/**
 * @brief       日期时间转Unix时间戳
 * @param       time: 日期时间结构体
 * @retval      Unix时间戳
 */
uint32_t datetime_to_unix(rtc_time_t *time)
{
    struct tm t;
    t.tm_year = 2000 + time->year - 1900;
    t.tm_mon = time->month - 1;
    t.tm_mday = time->day;
    t.tm_hour = time->hour;
    t.tm_min = time->minute;
    t.tm_sec = time->second;
    t.tm_isdst = -1;
    
    return mktime(&t);
}

/**
 * @brief       编码电压值
 * @param       voltage: 电压值
 * @param       buffer: 输出缓冲区(8字节)
 * @retval      无
 */
void encode_voltage(float voltage, uint8_t *buffer)
{
    uint16_t integer_part = (uint16_t)voltage;
    uint16_t fractional_part = (uint16_t)((voltage - integer_part) * 65536);
    
    buffer[0] = (integer_part >> 8) & 0xFF;
    buffer[1] = integer_part & 0xFF;
    buffer[2] = (fractional_part >> 8) & 0xFF;
    buffer[3] = fractional_part & 0xFF;
    
    /* 同时存储原始数据用于校验 */
    uint32_t raw = *(uint32_t*)&voltage;
    buffer[4] = (raw >> 24) & 0xFF;
    buffer[5] = (raw >> 16) & 0xFF;
    buffer[6] = (raw >> 8) & 0xFF;
    buffer[7] = raw & 0xFF;
}

/**
 * @brief       OLED状态显示
 * @param       无
 * @retval      无
 */
void oled_display_status(void)
{
    if (sampling_enabled) {
        rtc_time_t current_time;
        get_current_time(&current_time);
        
        char time_str[12], status[16];
        sprintf(time_str, "%02d:%02d:%02d", 
                current_time.hour, current_time.minute, current_time.second);
        
        sprintf(status, "CYC:%ds", sys_config.sample_cycle);
        
        oled_clear();
        oled_show_string(0, 0, time_str, 12);
        oled_show_string(0, 16, status, 12);
        oled_refresh_gram();
    } else {
        oled_clear();
        oled_show_string(0, 0, "system idle", 12);
        oled_refresh_gram();
    }
}


/* 添加全局变量 */
extern volatile uint8_t timer6_sample_flag;
extern volatile uint8_t timer6_led_flag;
int main(void)
{
    /* 硬件初始化 */
    delay_init(240);
    usart_init(115200);
    led_init();
    key_init();
    oled_init();
    adc_init();
    norflash_init();
    rtc_config();  // 修改为调用rtc_config
	
	/* 初始化定时器6 - 1ms中断 */
    timerx_int_init(1000-1, 120-1);  // 240MHz/(239+1)=1MHz, (999+1)/1MHz=1ms
    
    /* 挂载文件系统 */
    FATFS fs;
    f_mount(&fs, "0:", 1);
    
    /* 从Flash加载配置 */
    read_config_from_flash();
	
	  printf("=====system init=====\r\n");
    printf("Device_ID:2025-CIMC\r\n");
   printf("=====system ready=====\r\n");
    
    /* 初始化日志 */
    sys_config.log_file_id++;  // 每次上电递增
    save_config_to_flash();    // 保存新的文件ID
    store_log_entry("System started");
    
    /* 初始OLED显示 */
    oled_display_status();
		float last_voltage = 0.0;   // 保存上一次采样的电压值
    
    /* 主循环 */
    uint8_t key;
    
    while (1) {
        /* 按键处理 */
        key = key_scan(0);
        switch (key) {
            case KEY1_PRES:  // 采样启停
                if (sampling_enabled) stop_sampling();
                else start_sampling();
                break;
                
            case KEY2_PRES:  // 周期5s
                adjust_sample_cycle(5);
                break;
                
            case KEY3_PRES:  // 周期10s
                adjust_sample_cycle(10);
                break;
                
            case KEY4_PRES:  // 周期15s
                adjust_sample_cycle(15);
                break;
                
            case WKUP_PRES:  // 加密模式切换
                encrypt_mode = !encrypt_mode;
                store_log_entry(encrypt_mode ? "Encrypt enabled" : "Encrypt disabled");
                break;
        }
        
        /* 串口命令处理 */
        if (g_usart_rx_sta & 0x8000) {
            uint16_t len = g_usart_rx_sta & 0x3fff;
            g_usart_rx_buf[len] = '\0';
            g_usart_rx_sta = 0;
            
            char *cmd = (char*)g_usart_rx_buf;
            
            if (strcmp(cmd, "test") == 0) {
                system_self_test();
            } 
            else if (strncmp(cmd, "RTC Config", 10) == 0) {
                set_rtc_time(cmd + 11);
            } 
            else if (strcmp(cmd, "RTC now") == 0) {
                rtc_time_t current_time;
                get_current_time(&current_time);
                printf("Current Time:20%02d-%02d-%02d %02d:%02d:%02d\r\n", 
                       current_time.year, current_time.month, current_time.day,
                       current_time.hour, current_time.minute, current_time.second);
            } 
            else if (strcmp(cmd, "conf") == 0) {
                read_config_file();
            } 
            else if (strcmp(cmd, "ratio") == 0) {
                set_ratio();
            } 
            else if (strcmp(cmd, "limit") == 0) {
                set_limit();
            } 
            else if (strcmp(cmd, "config save") == 0) {
                save_config_to_flash();
            } 
            else if (strcmp(cmd, "config read") == 0) {
                read_config_from_flash();
            } 
            else if (strcmp(cmd, "start") == 0) {
                start_sampling();
            } 
            else if (strcmp(cmd, "stop") == 0) {
                stop_sampling();
            } 
            else if (strcmp(cmd, "hide") == 0) {
								hide_output_mode = 1;
								printf("Hide output mode enabled\r\n");
						} 
						else if (strcmp(cmd, "unhide") == 0) {
								hide_output_mode = 0;
								printf("Hide output mode disabled\r\n");
            }
						else if (strcmp(cmd, "encrypt") == 0) {  // 修改命令名避免冲突
    encrypt_mode = !encrypt_mode;
    printf("Encrypt mode %s\r\n", encrypt_mode ? "enabled" : "disabled");
    store_log_entry(encrypt_mode ? "Encrypt enabled" : "Encrypt disabled");
}
        }
       /* LED闪烁处理 */
        if (timer6_led_flag && sampling_enabled) {
            timer6_led_flag = 0;
            led_blink_state = !led_blink_state;
            LED1(led_blink_state);
        }
        
        /* 时间更新处理 */
        if (timer6_time_update_flag) {
            timer6_time_update_flag = 0;
            
            if (sampling_enabled) {
                update_oled_time();  // 每秒更新时间显示
            }
        }
        
        /* 采样处理 */
        if (timer6_sample_flag && sampling_enabled) {
            timer6_sample_flag = 0;
            process_adc_sample();
        }
//			/* 采样定时 */
//			if (sampling_enabled) {
//					uint32_t current_time = get_system_time();
//					if (current_time - last_sample_time >= sys_config.sample_cycle * 1000) {
//							last_sample_time = current_time;
//							process_adc_sample();
//							
//							/* LED闪烁控制 */
//							led_blink_state = !led_blink_state;
//							LED1(led_blink_state);
//					}
//			}
        
        delay_ms(1);
    }
	}