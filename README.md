# 西门子25赛题 — GD32F470 电压监测系统

基于 **GD32F470VET6** 的工业嵌入式电压采集、处理、显示与存储系统。

> 西门子杯中国智能制造挑战赛 · 2025 赛题工程

---

## 功能概览

| 模块 | 说明 |
| --- | --- |
| 电压采集 | ADC 采样，采样周期可配置（5 / 10 / 15） |
| 数据处理 | 比例系数 `ratio` 换算、超限阈值 `limit` 判定 |
| 显示 | OLED（IIC）实时显示状态与时间，LED 闪烁指示采样 |
| 时间 | RTC 实时时钟，支持串口校时 |
| 存储 | SD 卡（SDIO）与 SPI NOR Flash，FATFS 文件系统 |
| 配置管理 | 配置保存至 Flash / 从 Flash 读取 |
| 自检 | 上电系统自检 |
| 日志 | 采样数据、超限记录、操作日志落盘 |
| 安全 | 隐藏输出模式、加密模式开关 |

---

## 目录结构

```
西门子杯/
├── Drivers/
│   ├── BSP/                      # 板级驱动
│   │   ├── ADC/                  # ADC 采集
│   │   ├── FMC/                  # Flash 控制器
│   │   ├── IIC/  OLED/           # IIC 与 OLED 显示
│   │   ├── KEY/  LED/            # 按键与 LED
│   │   ├── NORFLASH/             # SPI NOR Flash
│   │   ├── RTC/                  # 实时时钟
│   │   ├── SDIO/                 # SD 卡驱动
│   │   ├── SPI/  TIMER/          # SPI 与定时器
│   │   └── CMSIS/                # Cortex-M4 内核支持
│   ├── GD32F4xx_standard_peripheral/   # GD32 标准外设库
│   └── SYSTEM/                   # sys / delay / usart
├── Middlewares/
│   ├── FATFS/                    # FatFs 文件系统
│   └── MALLOC/                   # 内存管理
├── User/                         # 应用层（main.c 等）
├── Projects/MDK-ARM/             # Keil MDK 工程
└── Output/                       # 编译输出（仅保留 GD32F470.hex）
```

---

## 硬件资源与引脚分配

**主控**：GD32F470VET6（Cortex-M4，512KB Flash / 192KB RAM）

### LED

| 名称 | 引脚 |
| --- | --- |
| LED1 | PD1 |
| LED2 | PD3 |
| LED3 | PD5 |
| LED4 | PD7 |
| LED5 | PB4 |
| LED6 | PB6 |

### 独立按键

| 名称 | 引脚 |
| --- | --- |
| KEY1 | PE15 |
| KEY2 | PE13 |
| KEY3 | PE11 |
| KEY4 | PE9 |
| KEY5 | PE7 |
| KEY6 | PB0 |
| WK_UP | PA0 |

### 串口与存储

| 资源 | 说明 |
| --- | --- |
| 串口0 | PA9 / PA10，板载 USB 转串口芯片 CH340 |
| GD25Q40 | SPI Flash，挂载于 SPI1：SCK=PB13、MISO=PB14、MOSI=PB15、CS=PD12 |
| SD 卡 | SDIO 接口 |
| OLED | IIC：SCL=PB8、SDA=PB9 |

---

## 串口命令

波特率 **115200 bps**，命令以回车换行结束。

| 命令 | 功能 |
| --- | --- |
| `test` | 系统自检 |
| `RTC Config <时间>` | 设置 RTC 时间 |
| `RTC now` | 读取当前时间 |
| `conf` | 读取配置文件 |
| `ratio` | 设置电压比例系数 |
| `limit` | 设置电压超限阈值 |
| `config save` | 配置保存至 Flash |
| `config read` | 从 Flash 读取配置 |
| `start` | 启动采样 |
| `stop` | 停止采样 |
| `hide` / `unhide` | 开启 / 关闭隐藏输出模式 |
| `encrypt` | 切换加密模式 |

---

## 编译与烧录

1. 使用 **Keil MDK (uVision5)** 打开工程：
   `Projects/MDK-ARM/GD32F470.uvprojx`
2. 需要安装器件包：`GigaDevice.GD32F4xx_DFP.3.0.3`
3. 编译器：**ARMCC V5.06 update 7 (build 960)**
4. 编译后固件输出于 `Output/GD32F470.hex`

> 说明：仓库已忽略编译中间产物（`.o` / `.crf` / `.d` / `.axf` 等），仅保留可直接烧录的
> `GD32F470.hex`。工程中的键盘布局文件（`*.uvguix.*`）与锁文件同样已忽略。

---

## 注意事项

1. 串口调试助手波特率需设置为 **115200 bps**。
2. 串口调试助手需勾选 **DTR**，否则 MCU 复位后程序不运行。
3. 请使用 USB 线连接板载的 USB_PORT 接口进行调试。
4. LED 与独立按键需用杜邦线连接至对应 IO 口。
