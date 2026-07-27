# Waveshare ESP32-S3-Touch-LCD-4 / ESP32-S3-LCD-4

[中文](README_CN.md)

This repository supports the Waveshare ESP32-S3-Touch-LCD-4 and ESP32-S3-LCD-4 4-inch round ESP32-S3 boards. The two boards share the 480 x 480 LCD, 16 MB Flash, 8 MB PSRAM, RS485, TWAI/CAN, RTC, SD card, battery-voltage sensing, and CH32V003 helper controller; the Touch variant adds a GT911 capacitive touch panel.

This repository provides factory firmware, schematic files, Arduino examples, and ESP-IDF examples. The examples are organized from basic peripheral bring-up to graphical UI applications, and both ESP-IDF and Arduino sketches are checked by GitHub Actions when related files change.

### Supported Board Variants

| Board | Touch controller | Notes |
| --- | --- | --- |
| ESP32-S3-Touch-LCD-4 | GT911 capacitive touch | Use all display, touch, LVGL, and ESP-Brookesia examples. |
| ESP32-S3-LCD-4 | No GT911 touch panel | Display, CH32, battery, SD, RS485, TWAI/CAN, RTC, and non-touch UI paths are shared. Touch-specific examples compile in CI, but firmware that requires GT911 input should be used in display-only mode or adapted before flashing. |

### Key Features

| Item | Description |
| --- | --- |
| MCU | ESP32-S3 with 2.4 GHz Wi-Fi and Bluetooth LE 5 |
| Memory | 16 MB Flash and 8 MB PSRAM |
| Display | 4-inch 480 x 480 LCD with LVGL and ESP-Brookesia examples |
| Touch | GT911 capacitive touch on ESP32-S3-Touch-LCD-4; not populated on ESP32-S3-LCD-4 |
| IO expander | CH32V003 over I2C for backlight, LCD/touch reset, buzzer, power enable, and battery ADC |
| Peripherals | RS485, TWAI/CAN, RTC, SD card, battery-voltage monitor |
| Main frameworks | ESP-IDF and Arduino ESP32 |

### Repository Layout

| Path | Content |
| --- | --- |
| [examples/esp-idf](examples/esp-idf/) | ESP-IDF examples from peripheral tests to LVGL/ESP-Brookesia UI |
| [examples/arduino](examples/arduino/) | Arduino sketches and bundled libraries |
| [config](config/) | Shared configuration notes and future reusable overlays |
| [examples/esp-idf/ioexpander](examples/esp-idf/ioexpander/) | Standalone CH32V003 IO expander test and detailed customer guide |
| [docs/CI.md](docs/CI.md) | ESP-IDF and Arduino example CI rules |
| [hardware](hardware/HARDWARE_REFERENCE.md) | V4.0 hardware reference and schematic PDF under `hardware/schematics/` |
| [firmware](firmware/) | Factory/recovery firmware image and flashing notes |
| [docs/firmware.md](docs/firmware.md) | Firmware artifact policy and CI boundary |
| [releases](releases/) | Firmware packaging and artifact download helpers |

### ESP-IDF Quick Start

To reproduce the currently configured CI matrix, install ESP-IDF v5.5.5 or v6.0.2, then start with a small board-level example:

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

Replace `PORT` with the actual serial port, such as `COM8` on Windows or `/dev/ttyACM0` on Linux.

Recommended learning order:

1. [ioexpander](examples/esp-idf/ioexpander/): verify CH32, backlight, reset pins, buzzer, and battery ADC first.
2. [01_RS485_Test](examples/esp-idf/01_RS485_Test/) to [05_TWAItransmit](examples/esp-idf/05_TWAItransmit/): validate onboard peripherals one by one.
3. [06_lvgl_demo_v8](examples/esp-idf/06_lvgl_demo_v8/) or [07_lvgl_demo_v9](examples/esp-idf/07_lvgl_demo_v9/): validate display, touch-capable BSP startup, and LVGL. On ESP32-S3-LCD-4, treat GT911 input as unavailable unless the example explicitly supports display-only mode.
4. [09_BatteryVoltage_LVGL](examples/esp-idf/09_BatteryVoltage_LVGL/): learn how to show battery voltage in an LVGL screen.
5. [08_ESP32-S3-Touch-LCD-4-esp-brookesia](examples/esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia/): inspect a larger ESP-Brookesia UI application structure.

### CH32V003 IO Expander

Some board functions are controlled by the CH32V003 helper chip instead of direct ESP32-S3 GPIOs. Common examples are LCD backlight PWM, LCD reset, touch reset, buzzer, system power enable, and battery ADC.

Key hardware parameters:

| Item | Value |
| --- | --- |
| I2C SDA | `GPIO15` |
| I2C SCL | `GPIO7` |
| CH32 I2C address | `0x24` |
| Backlight API | `custom_io_expander_set_pwm()`, range 0 to 255 |
| Battery-voltage formula | `raw * 3.3 / 1023 * 3.0` |

If the display, touch, or CH32 register writes occasionally fail after a quick reset, run [examples/esp-idf/ioexpander](examples/esp-idf/ioexpander/) first. It includes I2C bus recovery, CH32 initialization retry, and LCD/touch reset handling.

### Examples And CI

See [examples/README.md](examples/README.md) and [examples/esp-idf/README.md](examples/esp-idf/README.md) for the example map.

CI uses one `Build Examples` workflow. When a CI-relevant source, configuration, discovery, workflow, or release-packaging path changes, the workflow runs the complete first-party matrix: ESP-IDF `v5.5.5` and `v6.0.2` for target `esp32s3`, plus Arduino ESP32 core `3.3.11`. Successful jobs upload flashable firmware artifacts generated by `releases/package_firmware.py`. Treat these versions as the current workflow configuration rather than a permanent compatibility promise.

### FAQ

**The screen is dark or brightness does not change**

Check that CH32 initialization succeeds. The backlight is controlled by CH32 PWM, not a regular ESP32 LEDC pin. Use `custom_io_expander_set_pwm()` at the low level, or `bsp_display_brightness_set()` when using the BSP.

**Peripherals occasionally fail after quick reset**

CH32 or other devices on the same I2C bus may not reset at the same time as ESP32-S3. At application startup, recover the I2C bus, initialize CH32, and pulse the LCD/touch reset pins low before releasing them. The `ioexpander`, SD, battery, and UI examples already include this recovery flow.

**Battery voltage looks wrong**

The examples use a divider ratio of `3.0` according to the schematic. If the hardware divider or ADC reference is changed, update the conversion constants in the examples.

**Which libraries does Arduino CI use?**

Arduino CI passes `examples/arduino/libraries` to Arduino CLI, so it uses the repository versions of `GFX Library for Arduino`, `lvgl`, `SensorLib`, `WS_CH32_IO`, and `EspSoftwareSerial` instead of downloading replacements from Library Manager.

### Support

If you run into problems, first check the example README files, serial logs, and [Issues](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/issues). For technical support, contact Waveshare support and include the product version, reproduction steps, ESP-IDF/Arduino version, and serial log.

### License

This repository is licensed under Apache License 2.0. See [LICENSE.txt](LICENSE.txt) for details.
