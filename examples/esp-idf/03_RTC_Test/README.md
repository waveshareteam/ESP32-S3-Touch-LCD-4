# RTC Test / RTC 测试

[中文](#中文) | [English](#english)

## 中文

本示例用于验证板载 PCF85063A RTC。程序通过 I2C 初始化 RTC，设置一个固定时间，设置 2 秒后的闹钟，并通过 ESP32-S3 中断输入检测闹钟触发。

### 硬件和默认配置

| 项目 | 默认值 | 说明 |
| --- | --- | --- |
| RTC 芯片 | PCF85063A | I2C RTC |
| RTC I2C 地址 | `0x51` | 定义在 `main/PCF85063A.h` |
| I2C SCL | `GPIO19` | 可通过 `idf.py menuconfig` 修改 |
| I2C SDA | `GPIO18` | 可通过 `idf.py menuconfig` 修改 |
| RTC 中断输入 | `GPIO6` | `main/main.c` 中调用 `DEV_GPIO_INT(6, ...)` |

如果客户的硬件版本或连接方式不同，请在 `Example Configuration` 中修改 I2C 引脚，并同步检查 RTC 中断脚。

### 示例流程

1. 初始化 PCF85063A。
2. 设置时间为 `2024-02-02 09:00:00`。
3. 设置闹钟为 `2024-02-02 09:00:02`。
4. 使能 RTC 闹钟中断。
5. 每秒读取并打印当前 RTC 时间。
6. 闹钟触发后打印提示，并重新使能闹钟。

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
I (...) RTC: The alarm clock goes off.
```

### 常见问题

- 如果 I2C 读写失败，确认 RTC SDA/SCL 引脚和 menuconfig 一致。
- 如果时间一直不走，检查 RTC 晶振、供电和 PCF85063A 初始化返回值。
- 如果闹钟不触发，确认中断脚连接到当前代码使用的 `GPIO6`，并确认中断电平符合硬件设计。
- 如果只希望闹钟触发一次，可参考 `main/main.c` 注释，不要在触发后再次调用 `PCF85063A_Enable_Alarm()`。

## English

This example verifies the onboard PCF85063A RTC. It initializes the RTC over I2C, sets a fixed time, sets an alarm two seconds later, and uses an ESP32-S3 interrupt input to detect the alarm event.

### Hardware And Default Configuration

| Item | Default | Description |
| --- | --- | --- |
| RTC chip | PCF85063A | I2C RTC |
| RTC I2C address | `0x51` | Defined in `main/PCF85063A.h` |
| I2C SCL | `GPIO19` | Configurable with `idf.py menuconfig` |
| I2C SDA | `GPIO18` | Configurable with `idf.py menuconfig` |
| RTC interrupt input | `GPIO6` | `main/main.c` calls `DEV_GPIO_INT(6, ...)` |

If your hardware revision or wiring is different, update the I2C pins under `Example Configuration` and check the RTC interrupt pin as well.

### Example Flow

1. Initialize PCF85063A.
2. Set time to `2024-02-02 09:00:00`.
3. Set alarm to `2024-02-02 09:00:02`.
4. Enable RTC alarm interrupt.
5. Read and print the RTC time every second.
6. Print a message when the alarm triggers, then enable the alarm again.

### Build And Run

```bash
cd examples/esp-idf/03_RTC_Test
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### Expected Log

```text
I (...) RTC: Now_time is 2024-02-02 09:00:00
I (...) RTC: Now_time is 2024-02-02 09:00:01
I (...) RTC: The alarm clock goes off.
```

### Troubleshooting

- If I2C read/write fails, confirm that RTC SDA/SCL match menuconfig.
- If time does not advance, check the RTC crystal, power supply, and PCF85063A initialization result.
- If the alarm does not trigger, confirm that the interrupt line is connected to `GPIO6` as used by the current code, and that the interrupt level matches the hardware design.
- If the alarm should run only once, follow the comment in `main/main.c` and do not call `PCF85063A_Enable_Alarm()` again after the first trigger.
