# RTC Test

[中文](README_CN.md)

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
