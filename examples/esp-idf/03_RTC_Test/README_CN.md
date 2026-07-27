# RTC 测试

[English](README.md)

本示例用于验证板载 PCF85063A RTC。程序通过 I2C 初始化 RTC，设置一个固定时间和 2 秒后的闹钟，并通过 CH32V003 辅助控制器读取闹钟信号路径。

RTC 驱动由托管组件 `waveshare/pcf85063a` 提供；本示例只保留板级共享 I2C 和 CH32V003 `EXIO7` 集成代码。

### 硬件和默认配置

| 项目 | 默认值 | 说明 |
| --- | --- | --- |
| RTC 芯片 | PCF85063A | I2C RTC |
| RTC I2C 地址 | `0x51` | 由 `waveshare/pcf85063a` `2.0.0` 提供 |
| I2C SCL | `GPIO7` | 板级共享 I2C 总线，可通过 `idf.py menuconfig` 修改 |
| I2C SDA | `GPIO15` | 板级共享 I2C 总线，可通过 `idf.py menuconfig` 修改 |
| RTC 中断路径 | CH32V003 `EXIO7` | PCF85063A 的 `RTC_INT` 未直接连接 ESP32-S3 GPIO |

示例读取 CH32V003 的 `EXIO7` 路径中断寄存器，并检查 PCF85063A 闹钟标志确认事件。不要把该线路配置为 ESP32-S3 直连 GPIO 中断。

### 示例流程

1. 初始化 PCF85063A。
2. 设置时间为 `2024-02-02 09:00:00`。
3. 设置闹钟为 `2024-02-02 09:00:02`。
4. 使能 RTC 闹钟中断。
5. 每秒读取 RTC 时间、CH32V003 RTC 中断寄存器和 RTC 闹钟标志。
6. RTC 闹钟标志置位后打印提示，并重新使能闹钟。

### 编译和运行

```bash
cd examples/esp-idf/03_RTC_Test
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### 期望日志

```text
I (...) RTC: Now_time is 2024-02-02 09:00:00
I (...) RTC: Now_time is 2024-02-02 09:00:01
I (...) RTC: The alarm clock goes off (CH32 RTC_INT register=0).
```

### 常见问题

- 如果 I2C 读写失败，确认 RTC SDA/SCL 引脚和 menuconfig 一致。
- 如果时间一直不走，检查 RTC 晶振、供电和 PCF85063A 初始化返回值。
- 如果闹钟不触发，检查 I2C 地址 `0x24` 的 CH32V003、其 `EXIO7` 输入和 PCF85063A 闹钟标志；不要把 `RTC_INT` 分配给 ESP32-S3 GPIO。
- 如果只希望闹钟触发一次，可参考 `main/main.c` 注释，不要在触发后再次调用 `pcf85063a_enable_alarm()`。
