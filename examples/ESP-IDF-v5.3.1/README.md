# ESP-IDF Examples

These examples target ESP-IDF v5.3.1 and the `esp32s3` target. Each numbered
directory is an independent ESP-IDF project with its own `CMakeLists.txt` and
`main/` directory.

## Learning Path

Start with the peripheral checks, then move to display and UI examples:

| Order | Example | What It Checks |
| --- | --- | --- |
| 01 | [RS485_Test](01_RS485_Test/) | UART/RS485 wiring and serial data path |
| 02 | [SD_Test](02_SD_Test/) | SD card mount, file write, file read, and CH32 SD power setup |
| 03 | [RTC_Test](03_RTC_Test/) | PCF85063 RTC access over I2C |
| 04 | [TWAIreceive](04_TWAIreceive/) | CAN receive path and transceiver control |
| 05 | [TWAItransmit](05_TWAItransmit/) | CAN transmit path and transceiver control |
| 06 | [lvgl_demo_v8](06_lvgl_demo_v8/) | BSP display startup and LVGL v8 rendering |
| 07 | [lvgl_demo_v9](07_lvgl_demo_v9/) | BSP display startup and LVGL v9 rendering |
| 08 | [ESP32-S3-Touch-LCD-4-esp-brookesia](08_ESP32-S3-Touch-LCD-4-esp-brookesia/) | Advanced ESP-Brookesia phone UI |
| 09 | [BatteryVoltage_LVGL](09_BatteryVoltage_LVGL/) | CH32 ADC battery-voltage sampling and LVGL display |

## Build

```bash
cd examples/ESP-IDF-v5.3.1/09_BatteryVoltage_LVGL
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

Run `idf.py menuconfig` before building examples that need board-specific
settings, network credentials, display options, or peripheral tuning.

## Adding Examples

Keep new examples small and focused:

- Use a numbered ESP-IDF directory for each standalone example.
- Include `README.md`, `CMakeLists.txt`, `main/`, and `sdkconfig.defaults`
  when applicable.
- Document required hardware, menuconfig options, and expected serial output.
- Keep generated build artifacts, `managed_components/`, dependency lock files,
  and local `sdkconfig` files out of commits unless they are intentionally
  curated.
