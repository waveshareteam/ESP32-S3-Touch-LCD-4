# Waveshare ESP32-S3-Touch-LCD-4 / ESP32-S3-LCD-4

[中文](README_CN.md)

This repository supports the Waveshare ESP32-S3-Touch-LCD-4 and ESP32-S3-LCD-4 4-inch round ESP32-S3 boards. The two boards share the 480 x 480 LCD, 16 MB Flash, 8 MB PSRAM, RS485, TWAI/CAN, RTC, SD card, battery-voltage sensing, and CH32V003 helper controller; the Touch variant adds a GT911 capacitive touch panel.

This repository provides factory firmware, schematic files, 10 first-party ESP-IDF projects, and 13 first-party Arduino sketches. The examples are organized from basic peripheral bring-up to graphical UI applications, and both framework surfaces are checked by GitHub Actions when related files change.

The local schematic and the hardware values documented here cover ESP32-S3-Touch-LCD-4 V4.0. For ESP32-S3-LCD-4, confirm the product documentation and physical board revision before relying on touch-related or revision-specific details.

### Supported Board Variants

| Board | Touch controller | Notes |
| --- | --- | --- |
| ESP32-S3-Touch-LCD-4 | GT911 capacitive touch | Use all display, touch, LVGL, and ESP-Brookesia examples. |
| ESP32-S3-LCD-4 | No GT911 touch panel | Display, CH32, battery, SD, RS485, TWAI/CAN, RTC, and non-touch UI paths are shared. Use display-only mode only when an example explicitly documents that support; otherwise adapt touch-dependent firmware before flashing. |

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
| [examples/esp-idf](examples/esp-idf/) | 10 first-party ESP-IDF projects from peripheral tests to LVGL/ESP-Brookesia UI |
| [examples/arduino](examples/arduino/README.md) | 13 first-party Arduino sketches and bundled libraries |
| [config](config/) | Reserved location for shared configuration overlays; none are currently active |
| [examples/esp-idf/ioexpander](examples/esp-idf/ioexpander/) | Standalone CH32V003 IO expander test and detailed customer guide |
| [docs/CI.md](docs/CI.md) | ESP-IDF and Arduino example CI rules |
| [hardware](hardware/HARDWARE_REFERENCE.md) | V4.0 hardware reference and schematic PDF under `hardware/schematics/` |
| [firmware](firmware/) | Factory/recovery firmware image and flashing notes; [download the V4.0 image](firmware/ESP32-S3-Touch-LCD-4-V4.0-FactoryOnly-251122.bin) |
| [docs/firmware.md](docs/firmware.md) | Firmware artifact policy and CI boundary |
| [releases](releases/) | Firmware packaging and artifact download helpers |
| [docs/components.md](docs/components.md) | Managed-component and local-glue policy |
| [docs/repository-structure.md](docs/repository-structure.md) | Repository ownership and path boundaries |

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

### Arduino Quick Start

Install Arduino CLI, configure the Espressif board package index, and install the core version pinned by the current workflow. Compile a first-party sketch with the repository's bundled libraries and CI board options:

```bash
arduino-cli config init --overwrite
arduino-cli config add board_manager.additional_urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli core update-index
arduino-cli core install esp32:esp32@3.3.11
arduino-cli compile \
  --fqbn "esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB" \
  --libraries examples/arduino/libraries \
  examples/arduino/01_HelloWorld
```

See the [Arduino example index](examples/arduino/README.md) for all sketches, board options, and the bundled-library boundary. The workflow file remains the source of truth when the core version or FQBN changes.

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

See [examples/README.md](examples/README.md), the [10-project ESP-IDF index](examples/esp-idf/README.md), and the [13-sketch Arduino index](examples/arduino/README.md) for the complete example map.

CI uses one `Build Examples` workflow. When a CI-relevant source, configuration, discovery, workflow, or release-packaging path changes, the workflow runs the complete first-party matrix: ESP-IDF `v5.5.5` and `v6.0.2` for target `esp32s3`, plus Arduino ESP32 core `3.3.11`. Successful jobs upload flashable firmware artifacts generated by `releases/package_firmware.py`. Treat these versions as the current workflow configuration rather than a permanent compatibility promise.

### Firmware Downloads

- The checked-in [V4.0 factory/recovery image](firmware/ESP32-S3-Touch-LCD-4-V4.0-FactoryOnly-251122.bin) is a released binary for the touch-equipped board. It is not rebuilt by CI.
- Source-built example packages are attached to successful `Build Examples` workflow runs. Download the latest successful run for the current branch with `python3 releases/download_artifacts.py --clean`, or pass `--run-id <run-id>` for a specific run.
- Keep factory/recovery firmware separate from CI-generated example artifacts. See [Firmware Policy](docs/firmware.md) and [Release Tools](releases/README.md).

### Documentation

- [Hardware Reference](hardware/HARDWARE_REFERENCE.md)
- [Repository Structure](docs/repository-structure.md)
- [Continuous Integration](docs/CI.md)
- [Managed Components](docs/components.md)
- [Firmware and Factory Recovery](docs/firmware.md)
- [Contributing](CONTRIBUTING.md), [Support](SUPPORT.md), and [Security Policy](SECURITY.md)


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
