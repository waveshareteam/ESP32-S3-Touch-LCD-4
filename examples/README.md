# Examples

This directory contains both ESP-IDF projects and Arduino sketches for the
Waveshare ESP32-S3-Touch-LCD-4 board.

## ESP-IDF

Each directory under [ESP-IDF-v5.3.1](ESP-IDF-v5.3.1/) is a standalone ESP-IDF
project. Run ESP-IDF commands from inside the selected example directory unless
the example README says otherwise.

```bash
cd examples/ESP-IDF-v5.3.1/02_SD_Test
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

The current layout keeps the historical `ESP-IDF-v5.3.1` directory name for
compatibility, while the examples are indexed from simpler peripheral checks to
more advanced LVGL and ESP-Brookesia applications.

| Directory | Purpose | Level |
| --- | --- | --- |
| [01_RS485_Test](ESP-IDF-v5.3.1/01_RS485_Test/) | UART/RS485 receive and echo test | Peripheral bring-up |
| [02_SD_Test](ESP-IDF-v5.3.1/02_SD_Test/) | SD card access through the onboard socket | Peripheral bring-up |
| [03_RTC_Test](ESP-IDF-v5.3.1/03_RTC_Test/) | PCF85063 RTC access over I2C | Peripheral bring-up |
| [04_TWAIreceive](ESP-IDF-v5.3.1/04_TWAIreceive/) | TWAI/CAN receive path | Peripheral bring-up |
| [05_TWAItransmit](ESP-IDF-v5.3.1/05_TWAItransmit/) | TWAI/CAN transmit path | Peripheral bring-up |
| [06_lvgl_demo_v8](ESP-IDF-v5.3.1/06_lvgl_demo_v8/) | LVGL v8 display demo | Display and UI |
| [07_lvgl_demo_v9](ESP-IDF-v5.3.1/07_lvgl_demo_v9/) | LVGL v9 display demo | Display and UI |
| [08_ESP32-S3-Touch-LCD-4-esp-brookesia](ESP-IDF-v5.3.1/08_ESP32-S3-Touch-LCD-4-esp-brookesia/) | ESP-Brookesia UI application | Advanced UI |
| [09_BatteryVoltage_LVGL](ESP-IDF-v5.3.1/09_BatteryVoltage_LVGL/) | CH32 ADC battery voltage display with LVGL | Display and board monitor |

See [../docs/CI.md](../docs/CI.md) for the ESP-IDF example build checks.

## Arduino

Arduino sketches and bundled libraries are under [Arduino-v3.3.2](Arduino-v3.3.2/).
They are not built by the ESP-IDF CI workflow.
