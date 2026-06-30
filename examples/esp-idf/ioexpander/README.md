# CH32V003 IO 扩展测试与使用说明

本示例是 ESP32-S3-Touch-LCD-4 上 CH32V003 IO 扩展芯片的独立 ESP-IDF
测试程序。它面向第一次使用这块板子的客户，目标是把 CH32 负责什么、
怎么初始化、怎么控制背光、怎么复位屏幕/触摸、怎么读电池电压讲清楚。

这个示例不会启动 LVGL，也不会初始化完整显示驱动。它只测试 CH32，并通过
串口日志打印每一步操作。因此当客户遇到屏幕不亮、背光不可控、快速复位后
外设异常、I2C 初始化失败等问题时，可以先运行这个示例确认 CH32 是否正常。

## CH32 在板子上的作用

ESP32-S3-Touch-LCD-4 使用 CH32V003 作为板载 IO 扩展/辅助控制芯片。
ESP32-S3 通过 I2C 写 CH32 寄存器，间接控制部分板载功能：

- LCD 背光 PWM
- LCD 复位
- 触摸复位
- 系统电源使能
- 蜂鸣器使能
- RTC 中断输入读取
- 电池电压 ADC 读取

也就是说，客户不能把这些功能都当成普通 ESP32 GPIO 使用。例如背光不是
ESP32-S3 的 LEDC 引脚，而是 CH32 的 PWM 寄存器控制。

## 硬件连接

ESP32-S3 与 CH32V003 的通信接口如下：

| 项目 | 值 |
| --- | --- |
| I2C 端口 | `I2C_NUM_0` |
| SDA | `GPIO15` |
| SCL | `GPIO7` |
| CH32 I2C 地址 | `0x24` |
| I2C 速率 | 400 kHz |

本示例使用的 ESP-IDF 组件是：

```yaml
dependencies:
  waveshare/custom_io_expander_ch32v003:
    version: "*"
```

## CH32 IO 映射表

| CH32 信号 | ESP-IDF mask | 板载功能 | 方向 | 常用电平/用法 |
| --- | --- | --- | --- | --- |
| `EXIO1` | `IO_EXPANDER_PIN_NUM_1` | 触摸复位 `TP_RST` | 输出 | `1` 释放复位，`0` 进入复位 |
| `EXIO3` | `IO_EXPANDER_PIN_NUM_3` | LCD 复位 `LCD_RST` | 输出 | `1` 释放复位，`0` 进入复位 |
| `EXIO5` | `IO_EXPANDER_PIN_NUM_5` | 系统电源使能 `SYS_EN` | 输出 | `1` 使能相关电源 |
| `EXIO6` | `IO_EXPANDER_PIN_NUM_6` | 蜂鸣器 `BEE_EN` | 输出 | `1` 响，`0` 关闭 |
| `EXIO7` | `IO_EXPANDER_PIN_NUM_7` | RTC 中断 `RTC_INT` | 输入 | 只读 |
| `EXIO_PWM` | `custom_io_expander_set_pwm()` | LCD 背光 `BL_EN` | PWM | `0` 关闭，`255` 最亮 |
| `EXIO_ADC` | `custom_io_expander_get_adc()` | 电池分压 ADC | ADC | 原始值 0 到 1023 |

LCD 正常工作时，建议保持 `SYS_EN`、`LCD_RST`、`TP_RST` 为高电平。
不使用蜂鸣器时，保持 `BEE_EN` 为低电平。

## 示例会做什么

程序启动后会依次执行：

1. 在安装 I2C 驱动前恢复 I2C 总线。
2. 在地址 `0x24` 创建 CH32V003 IO 扩展对象。
3. 设置 `EXIO1`、`EXIO3`、`EXIO5`、`EXIO6` 为输出。
4. 设置 `EXIO7` 为输入。
5. 将 LCD、触摸、系统使能、蜂鸣器相关输出全部拉低 200 ms。
6. 拉高 `SYS_EN`、`LCD_RST`、`TP_RST`，关闭 `BEE_EN`。
7. 打印 CH32 IO 状态、RTC 中断状态、电池 ADC 和换算电压。
8. 蜂鸣器短响一次。
9. 对 LCD 和触摸复位脚做一次低脉冲复位。
10. 依次设置背光亮度为 10%、40%、80%、100%、60%、20%、100%。
11. 每 2 秒继续打印一次 CH32 状态和电池电压。

运行时即使没有显示画面，背光亮度也应随 PWM 演示明显变化。

## 编译和运行

支持 ESP-IDF v5.5.4 和 v6.0.2。

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

把 `PORT` 替换成实际串口，例如 Windows 下的 `COM8` 或 Linux 下的
`/dev/ttyACM0`。

## 期望串口日志

电池 ADC 原始值会随供电方式和电池电压变化，下面只展示关键格式：

```text
I (...) ch32_ioexpander: Enable SYS_EN, release LCD_RST and TP_RST
I (...) ch32_ioexpander: CH32 output mask=0x6A SYS_EN=1 LCD_RST=1 TP_RST=1 BEE_EN=0 RTC_INT pin=...
I (...) ch32_ioexpander: Battery ADC raw=..., voltage=... V
I (...) ch32_ioexpander: Buzzer on for 80 ms
I (...) ch32_ioexpander: Set LCD backlight to 10% (PWM duty 26/255)
I (...) ch32_ioexpander: Set LCD backlight to 100% (PWM duty 255/255)
```

如果能看到这些日志，说明 ESP32-S3 可以正常访问 CH32，CH32 的基本寄存器
读写、PWM、ADC 都工作正常。

## 常用代码片段

### 1. 安全初始化 CH32

推荐客户自己的 ESP-IDF 工程也采用这个流程：

```c
board_i2c_recover();
i2c_new_master_bus(&i2c_bus_conf, &i2c_bus);
custom_io_expander_new_i2c_ch32v003(i2c_bus, 0x24, &io_expander);
esp_io_expander_set_dir(io_expander, output_mask, IO_EXPANDER_OUTPUT);
esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_7, IO_EXPANDER_INPUT);
```

`board_i2c_recover()` 很重要。快速复位时，ESP32-S3 会复位，但 CH32 或同一
I2C 总线上的其他器件不一定同步复位。如果某个器件仍把 SDA 拉低，下一次
启动就可能出现 I2C 初始化失败或 CH32 寄存器写入失败。恢复流程会手动输出
SCL 脉冲并产生 STOP 条件，让总线回到空闲状态。

### 2. 释放 LCD 和触摸复位

```c
#define TP_RST   IO_EXPANDER_PIN_NUM_1
#define LCD_RST  IO_EXPANDER_PIN_NUM_3
#define SYS_EN   IO_EXPANDER_PIN_NUM_5
#define BEE_EN   IO_EXPANDER_PIN_NUM_6

esp_io_expander_set_dir(io_expander, TP_RST | LCD_RST | SYS_EN | BEE_EN,
                        IO_EXPANDER_OUTPUT);
esp_io_expander_set_level(io_expander, TP_RST | LCD_RST | SYS_EN | BEE_EN, 0);
vTaskDelay(pdMS_TO_TICKS(200));
esp_io_expander_set_level(io_expander, SYS_EN | LCD_RST | TP_RST, 1);
esp_io_expander_set_level(io_expander, BEE_EN, 0);
```

如果客户遇到屏幕或触摸在快速复位后偶发不正常，建议在应用启动时明确执行
一次上面的低电平复位再释放。

### 3. 控制 LCD 背光

LCD 背光由 CH32 的 PWM 寄存器控制，不能用普通 ESP32 GPIO 或 LEDC 直接
控制。使用：

```c
custom_io_expander_set_pwm(io_expander, 0);    // 关闭背光
custom_io_expander_set_pwm(io_expander, 128);  // 约 50% 亮度
custom_io_expander_set_pwm(io_expander, 255);  // 最高亮度
```

如果希望用百分比：

```c
uint8_t duty = percent * 255 / 100;
custom_io_expander_set_pwm(io_expander, duty);
```

如果客户使用 Waveshare BSP 的显示接口，也可以在显示初始化后调用：

```c
bsp_display_brightness_set(80);  // 80% 亮度
```

BSP 底层同样会写 CH32 PWM。

### 4. 读取电池电压

CH32 ADC 返回 0 到 1023 的原始值。根据当前板载分压，电池电压换算公式为：

```c
voltage = raw * 3.3f / 1023.0f * 3.0f;
```

基础读取方式：

```c
uint16_t raw = 0;
custom_io_expander_get_adc(io_expander, &raw);
float voltage = raw * 3.3f / 1023.0f * 3.0f;
```

实际产品里建议多次采样后取平均值，减少 ADC 抖动。本示例的
`read_battery_voltage()` 已经做了 8 次采样平均。

### 5. 控制蜂鸣器

```c
esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_6, 1);
vTaskDelay(pdMS_TO_TICKS(80));
esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_6, 0);
```

注意蜂鸣器用完后一定要拉低 `BEE_EN`，否则可能持续发声。

### 6. 读取 RTC 中断状态

`EXIO7` 连接到 `RTC_INT`，配置为输入。

```c
uint32_t level = 0;
uint8_t int_reg = 0;

esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_7, IO_EXPANDER_INPUT);
esp_io_expander_get_level(io_expander, IO_EXPANDER_PIN_NUM_7, &level);
custom_io_expander_get_int(io_expander, &int_reg);
```

## 如果工程已经使用 BSP

如果客户工程已经依赖 Waveshare BSP，可以直接取得 BSP 初始化好的 CH32 句柄：

```c
#include "bsp/esp-bsp.h"

esp_io_expander_handle_t io_expander = bsp_io_expander_init();
```

显示示例通常会在 `bsp_display_start()` 里初始化 CH32 和显示相关电源/复位。
本示例展示的是更底层的 CH32 调用，方便客户理解 BSP 背后做了什么。

## 常见问题

### 找不到 CH32：`CH32V003 IO expander not found at 0x24`

优先检查：

- I2C SDA 是否为 `GPIO15`。
- I2C SCL 是否为 `GPIO7`。
- 是否有其他代码已经占用了同一个 I2C 端口。
- 是否在 `i2c_new_master_bus()` 前执行了 I2C 总线恢复。

### 背光不变化

这块板子的背光不由普通 ESP32 GPIO 控制。请使用
`custom_io_expander_set_pwm()`，或者在 BSP 显示流程里使用
`bsp_display_brightness_set()`。

### 快速复位后 LCD 或触摸异常

建议启动时做三件事：

1. 先恢复 I2C 总线。
2. 初始化 CH32。
3. 将 `LCD_RST` 和 `TP_RST` 拉低 100 到 200 ms，再拉高释放。

本示例已经包含完整流程，可直接参考 `ch32_init()` 和
`ch32_reset_lcd_and_touch()`。

### 蜂鸣器一直响

确认 `EXIO6` 被设置为输出，并在蜂鸣结束后写低电平：

```c
esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_6, 0);
```

### 电池电压读数不符合预期

当前换算使用 3.3 V ADC 参考电压和 3.0 分压比例。如果客户修改了硬件分压，
需要同步修改 `main/main.c` 里的 `BATTERY_DIVIDER_RATIO`。
