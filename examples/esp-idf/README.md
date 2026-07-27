# ESP-IDF Examples

[中文](README_CN.md)

These examples target `esp32s3` for ESP32-S3-Touch-LCD-4 and ESP32-S3-LCD-4. The current CI matrix builds them with ESP-IDF `v5.5.5` and `v6.0.2`. Each buildable example directory has its own `CMakeLists.txt` and `main/` directory.

### Recommended Learning Path

Run `ioexpander` first to verify CH32, backlight, reset pins, and battery ADC, then move through the peripheral and UI examples. ESP32-S3-LCD-4 does not have GT911, so treat touch-driven UI examples as Touch-variant firmware unless their README or source explicitly says they can run without pointer input.

| Order | Example | What It Checks |
| --- | --- | --- |
| 00 | [ioexpander](ioexpander/) | CH32V003 IO expander, LCD/touch reset, backlight PWM, buzzer, RTC_INT, battery ADC |
| 01 | [RS485_Test](01_RS485_Test/) | UART/RS485 pins and serial data path |
| 02 | [SD_Test](02_SD_Test/) | SD card mount, file write/read, format, and CH32 power/reset flow |
| 03 | [RTC_Test](03_RTC_Test/) | PCF85063A RTC I2C access, time setting, alarm interrupt |
| 04 | [TWAIreceive](04_TWAIreceive/) | TWAI/CAN receive path and received-frame echo |
| 05 | [TWAItransmit](05_TWAItransmit/) | TWAI/CAN periodic transmit path |
| 06 | [lvgl_demo_v8](06_lvgl_demo_v8/) | BSP display startup, GT911 touch initialization, and LVGL v8 widgets demo |
| 07 | [lvgl_demo_v9](07_lvgl_demo_v9/) | BSP display startup, GT911 touch-capable startup, and LVGL v9 benchmark demo |
| 08 | [BatteryVoltage_LVGL](09_BatteryVoltage_LVGL/) | CH32 ADC battery-voltage sampling displayed in LVGL |
| 09 | [ESP32-S3-Touch-LCD-4-esp-brookesia](08_ESP32-S3-Touch-LCD-4-esp-brookesia/) | Advanced touch-driven ESP-Brookesia Phone UI, app registration, and peripheral tasks |

### Common Build Commands

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

Replace `PORT` with the actual serial port. The first build may take longer because managed components are downloaded.

### Adding New Examples

Keep new ESP-IDF examples small and focused:

- Use one first-level directory per standalone example.
- Include `README.md`, `CMakeLists.txt`, `main/`, and `sdkconfig.defaults` when default configuration is needed.
- README files must be bilingual and include hardware wiring, build steps, expected behavior, and troubleshooting notes.
- If CH32 is used, document the related EXIO mapping, I2C address, and quick-reset recovery flow.
- Do not commit `build/`, `managed_components/`, `dependencies.lock`, local `sdkconfig`, or `sdkconfig.old` unless they are intentionally maintained configuration files.
