# RTC Test

[中文](README_CN.md)

This example verifies the onboard PCF85063A RTC. It initializes the RTC over I2C, sets a fixed time, sets an alarm two seconds later, and reads the alarm path through the CH32V003 helper controller.

### Hardware And Default Configuration

| Item | Default | Description |
| --- | --- | --- |
| RTC chip | PCF85063A | I2C RTC |
| RTC I2C address | `0x51` | Defined in `main/PCF85063A.h` |
| I2C SCL | `GPIO7` | Shared board I2C bus; configurable with `idf.py menuconfig` |
| I2C SDA | `GPIO15` | Shared board I2C bus; configurable with `idf.py menuconfig` |
| RTC interrupt path | CH32V003 `EXIO7` | The PCF85063A `RTC_INT` signal is not connected directly to an ESP32-S3 GPIO |

The example reads the CH32V003 interrupt register for the `EXIO7` path and checks the PCF85063A alarm flag to confirm the event. The alarm path must not be configured as a direct ESP32-S3 GPIO interrupt.

### Example Flow

1. Initialize PCF85063A.
2. Set time to `2024-02-02 09:00:00`.
3. Set alarm to `2024-02-02 09:00:02`.
4. Enable RTC alarm interrupt.
5. Read the RTC time, CH32V003 RTC interrupt register, and RTC alarm flag every second.
6. Print a message when the RTC alarm flag is set, then enable the alarm again.

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
I (...) RTC: The alarm clock goes off (CH32 RTC_INT register=0).
```

### Troubleshooting

- If I2C read/write fails, confirm that RTC SDA/SCL match menuconfig.
- If time does not advance, check the RTC crystal, power supply, and PCF85063A initialization result.
- If the alarm does not trigger, verify the CH32V003 at I2C address `0x24`, its `EXIO7` input, and the PCF85063A alarm flag. Do not assign `RTC_INT` to an ESP32-S3 GPIO.
- If the alarm should run only once, follow the comment in `main/main.c` and do not call `PCF85063A_Enable_Alarm()` again after the first trigger.
