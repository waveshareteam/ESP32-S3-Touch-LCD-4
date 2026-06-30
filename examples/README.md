# Examples

[中文](README_CN.md)

This directory contains ESP-IDF examples, Arduino sketches, and bundled libraries for ESP32-S3-Touch-LCD-4.

Customers are encouraged to start with the ESP-IDF examples because they are organized as a learning path from simple peripheral bring-up to larger UI applications, and they are automatically built by GitHub Actions. Arduino examples remain available for the Arduino ESP32 workflow, but they are not built by the ESP-IDF CI workflow.

### ESP-IDF Examples

Each first-level directory under [esp-idf](esp-idf/) is a standalone ESP-IDF project. Run build commands from the selected example directory:

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

The examples are ordered from board-level bring-up to graphical UI:

| Directory | Purpose | Level |
| --- | --- | --- |
| [ioexpander](esp-idf/ioexpander/) | CH32V003 IO expander test and customer guide | Basic hardware check |
| [01_RS485_Test](esp-idf/01_RS485_Test/) | UART/RS485 receive and echo test | Peripheral bring-up |
| [02_SD_Test](esp-idf/02_SD_Test/) | SD card mount, read/write, format, and CH32 power/reset flow | Peripheral bring-up |
| [03_RTC_Test](esp-idf/03_RTC_Test/) | PCF85063A RTC I2C access and alarm interrupt | Peripheral bring-up |
| [04_TWAIreceive](esp-idf/04_TWAIreceive/) | TWAI/CAN receive path that echoes received frames | Peripheral bring-up |
| [05_TWAItransmit](esp-idf/05_TWAItransmit/) | TWAI/CAN periodic test-frame transmitter | Peripheral bring-up |
| [06_lvgl_demo_v8](esp-idf/06_lvgl_demo_v8/) | BSP display startup and LVGL v8 widgets demo | Display/UI |
| [07_lvgl_demo_v9](esp-idf/07_lvgl_demo_v9/) | BSP display startup and LVGL v9 benchmark demo | Display/UI |
| [09_BatteryVoltage_LVGL](esp-idf/09_BatteryVoltage_LVGL/) | CH32 ADC battery-voltage sampling displayed in LVGL | Display/board monitor |
| [08_ESP32-S3-Touch-LCD-4-esp-brookesia](esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia/) | ESP-Brookesia Phone UI with calculator, draw panel, and CAN task | Advanced UI |

See [../docs/CI.md](../docs/CI.md) for ESP-IDF example CI rules.

### Arduino Examples

Arduino sketches and bundled libraries are under [Arduino-v3.3.2](Arduino-v3.3.2/). They are not built by the `ESP-IDF examples` workflow. When using Arduino examples, verify the Arduino ESP32 core, board options, partition scheme, and library versions required by the selected sketch.
