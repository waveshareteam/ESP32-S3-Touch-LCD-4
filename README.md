<div align="center">
  <h1>ESP32-S3-Touch-LCD-4</h1>
  <p><strong>ESP32-S3 4-inch 480 x 480 RGB LCD development board with optional GT911 capacitive touch</strong></p>
  <p>
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/actions/workflows/examples.yml"><img alt="Build Examples" src="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/actions/workflows/examples.yml/badge.svg"></a>
    <a href="LICENSE.txt"><img alt="License" src="https://img.shields.io/github/license/waveshareteam/ESP32-S3-Touch-LCD-4"></a>
  </p>
  <p>
    <a href="README_CN.md">中文</a> ·
    <a href="https://www.waveshare.com/esp32-s3-touch-lcd-4.htm">Product Page</a> ·
    <a href="https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4">Documentation</a> ·
    <a href="https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/actions/workflows/examples.yml">CI Firmware</a> ·
    <a href="examples/esp-idf/">ESP-IDF Examples</a> ·
    <a href="examples/arduino/">Arduino Examples</a>
  </p>
  <a href="https://www.waveshare.com/esp32-s3-touch-lcd-4.htm">
    <img src="https://www.waveshare.com/w/upload/7/7b/ESP32-S3-Touch-LCD-4-P.jpg" alt="Waveshare ESP32-S3-Touch-LCD-4" width="500">
  </a>
</div>

---

## Overview

This repository provides first-party ESP-IDF and Arduino examples, source-built
flashable CI firmware, factory recovery firmware, and V4.0 hardware references
for the Waveshare ESP32-S3-Touch-LCD-4 and ESP32-S3-LCD-4.

Both boards combine an ESP32-S3 with a 4-inch round RGB display, high-capacity
Flash and PSRAM, RTC, microSD, RS485, TWAI/CAN, battery support, and a
CH32V003 helper controller. The Touch variant also includes a GT911 capacitive
touch panel.

| Board | Touch | Recommended use |
| --- | --- | --- |
| ESP32-S3-Touch-LCD-4 | GT911 capacitive touch | All display, touch, LVGL, and ESP-Brookesia examples |
| ESP32-S3-LCD-4 | Not populated | Display and peripheral examples; adapt workflows that require pointer input |

## Hardware Overview

| Feature | Device / interface |
| --- | --- |
| MCU | ESP32-S3-WROOM-1-N16R8, dual-core Xtensa LX7 at up to 240 MHz |
| Memory | 16 MB Flash and 8 MB octal PSRAM |
| Wireless | 2.4 GHz Wi-Fi and Bluetooth 5 LE |
| Display | 4-inch 480 x 480 RGB LCD with serial control |
| Touch | GT911 capacitive touch over I2C on ESP32-S3-Touch-LCD-4 |
| Helper controller | CH32V003F4U6 over I2C for backlight, resets, buzzer, power control, and battery ADC |
| Real-time clock | PCF85063ATL at `0x51`; the ESP-IDF example uses [`waveshare/pcf85063a`](https://components.espressif.com/components/waveshare/pcf85063a) |
| Fieldbus | TJA1051 TWAI/CAN transceiver and SP3485 RS485 transceiver |
| Storage | microSD card slot using 1-bit SDMMC |
| Power | USB Type-C, external DC and battery inputs, charging, and battery-voltage sensing |
| Board support | Managed BSP: [`waveshare/esp32_s3_touch_lcd_4`](https://components.espressif.com/components/waveshare/esp32_s3_touch_lcd_4) |
| Hardware files | [V4.0 hardware reference](hardware/HARDWARE_REFERENCE.md) and [schematic](hardware/schematics/) |

> [!IMPORTANT]
> The bundled schematic covers ESP32-S3-Touch-LCD-4 V4.0. Use it together
> with the physical board revision as the source of truth for electrical work.
> Confirm the product documentation before relying on revision-specific details
> for ESP32-S3-LCD-4. Some online material describes earlier revisions with a
> different IO expander or pin routing.

## Firmware Artifacts

The fastest way to try a source-built example is to download a flashable
artifact from the
[Build Examples workflow](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/actions/workflows/examples.yml).

1. Open a successful workflow run for the required branch or tag.
2. Download the artifact matching the framework, example, and framework version.
3. Extract the archive and install esptool with `python -m pip install esptool`.
4. Connect the board over USB and run `flash.bat COMx` on Windows or
   `./flash.sh /dev/ttyACM0` on Linux.

Each archive contains a firmware manifest, flash arguments, helper scripts, and
the required binaries. Maintainers can also download the latest successful
artifacts for the current branch with:

```bash
python3 releases/download_artifacts.py --clean
```

The checked-in [V4.0 factory/recovery image](firmware/ESP32-S3-Touch-LCD-4-V4.0-FactoryOnly-251122.bin)
is a released binary for the touch-equipped board and is not rebuilt by CI.
Keep factory/recovery firmware separate from source-built CI artifacts. See
[Firmware Artifacts](docs/firmware.md) for the distinction and flashing notes.

## Build From Source

### ESP-IDF

The ESP-IDF projects are independent applications. Start with the board-level
IO expander example using one of the versions configured in CI:

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

Replace `PORT` with the board serial port, such as `COM8` on Windows or
`/dev/ttyACM0` on Linux. For Arduino board options, bundled libraries, and the
recommended learning order, see the [Examples Guide](examples/README.md).

### Arduino

After configuring Arduino CLI and the Espressif board package, install the core
version shown in the current CI matrix and compile with the bundled libraries:

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

## Examples

### ESP-IDF

| Example | Focus | Board use |
| --- | --- | --- |
| [ioexpander](examples/esp-idf/ioexpander/) | CH32V003 bring-up, I2C recovery, resets, backlight, buzzer, and battery ADC | Both |
| [01_RS485_Test](examples/esp-idf/01_RS485_Test/) | RS485 receive and echo | Both |
| [02_SD_Test](examples/esp-idf/02_SD_Test/) | microSD mount, read/write, format, and power/reset flow | Both |
| [03_RTC_Test](examples/esp-idf/03_RTC_Test/) | PCF85063A time/date access and alarm interrupt | Both |
| [04_TWAIreceive](examples/esp-idf/04_TWAIreceive/) | TWAI/CAN receive and frame echo | Both |
| [05_TWAItransmit](examples/esp-idf/05_TWAItransmit/) | TWAI/CAN periodic frame transmission | Both |
| [06_lvgl_demo_v8](examples/esp-idf/06_lvgl_demo_v8/) | BSP display startup and LVGL v8 widgets | Touch direct; adapt LCD-only BSP startup |
| [07_lvgl_demo_v9](examples/esp-idf/07_lvgl_demo_v9/) | BSP display startup and LVGL v9 benchmark | Touch direct; adapt LCD-only BSP startup |
| [08_ESP32-S3-Touch-LCD-4-esp-brookesia](examples/esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia/) | ESP-Brookesia Phone UI, calculator, drawing, and CAN task | Touch recommended |
| [09_BatteryVoltage_LVGL](examples/esp-idf/09_BatteryVoltage_LVGL/) | Battery-voltage sampling displayed with LVGL | Both |

### Arduino

| Example | Focus | Board use |
| --- | --- | --- |
| [01_HelloWorld](examples/arduino/01_HelloWorld/) | Arduino GFX display bring-up | Both |
| [02_AsciiTable](examples/arduino/02_AsciiTable/) | GFX text and ASCII character rendering | Both |
| [03_Drawing_points](examples/arduino/03_Drawing_points/) | GT911 touch drawing board | Touch for interaction |
| [05_GFX_PCF85063_simpleTime](examples/arduino/05_GFX_PCF85063_simpleTime/) | PCF85063 RTC with GFX output | Both |
| [06_GFX_ESPWiFiAnalyzer](examples/arduino/06_GFX_ESPWiFiAnalyzer/) | Wi-Fi scanning and channel visualization | Both |
| [07_GFX_Clock](examples/arduino/07_GFX_Clock/) | Graphical clock rendering | Both |
| [08_LVGL_PCF85063_simpleTime](examples/arduino/08_LVGL_PCF85063_simpleTime/) | PCF85063 RTC with an LVGL interface | Both |
| [09_LVGL_Widgets](examples/arduino/09_LVGL_Widgets/) | LVGL widget demonstration | Both; touch optional |
| [10_LVGL_SD](examples/arduino/10_LVGL_SD/) | microSD access with an LVGL interface | Both |
| [11_TWAItransmit](examples/arduino/11_TWAItransmit/) | TWAI/CAN periodic frame transmission | Both |
| [12_TWAIreceive](examples/arduino/12_TWAIreceive/) | TWAI/CAN receive and frame echo | Both |
| [13_RS485](examples/arduino/13_RS485/) | RS485 communication | Both |
| [14_LVGL_BatteryVoltage](examples/arduino/14_LVGL_BatteryVoltage/) | Battery-voltage monitor with LVGL | Both |

Bundled Arduino libraries live under
[`examples/arduino/libraries/`](examples/arduino/libraries/). Their upstream
library examples are intentionally excluded from the product CI matrix.

## Supported Toolchains

| Surface | Version | First-party firmware builds |
| --- | --- | ---: |
| ESP-IDF | `v5.5.5` | 10 |
| ESP-IDF | `v6.0.2` | 10 |
| Arduino-ESP32 | `3.3.11` | 13 |

The
[Build Examples workflow](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/actions/workflows/examples.yml)
runs two discovery jobs, selector validation, and 33 firmware build jobs for
the full matrix. Every successful build is packaged as a flashable artifact.
The companion
[Test Repository Tools workflow](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/actions/workflows/repository-tools.yml)
checks example discovery, selectors, and release tooling.

These versions describe the current workflow configuration rather than a
permanent compatibility promise. The workflow files remain the source of truth.

## Board Bring-Up Notes

- CH32V003 uses I2C address `0x24`. If the screen is dark or brightness
  does not change, confirm CH32 initialization. Backlight uses CH32 PWM through
  `custom_io_expander_set_pwm()` with a range of 0 to 255, not an ESP32 LEDC pin.
- After a quick reset, recover the shared I2C bus before initializing CH32V003
  and pulse the LCD/touch reset lines low. The maintained board examples already
  implement this flow.
- Battery examples use the V4.0 schematic divider calculation
  `raw * 3.3 / 1023 * 3.0`.
- The shared control bus uses SDA `GPIO15` and SCL `GPIO7`; RTC alarm routing
  reaches CH32V003 `EXIO7`.

## Repository Layout

| Path | Purpose |
| --- | --- |
| [`examples/esp-idf/`](examples/esp-idf/) | First-party ESP-IDF projects |
| [`examples/arduino/`](examples/arduino/) | First-party Arduino sketches and bundled libraries |
| [`firmware/`](firmware/) | Factory flashing and recovery binary |
| [`releases/`](releases/) | Firmware packaging and artifact download tools |
| [`hardware/`](hardware/) | V4.0 hardware reference and schematic |
| [`config/`](config/) | Reserved for reusable shared ESP-IDF overlays; none are active yet |
| [`docs/`](docs/) | Repository, CI, component, firmware, and compatibility notes |

## Documentation

- [Product Documentation](https://www.waveshare.com/wiki/ESP32-S3-Touch-LCD-4)
- [Hardware Reference](hardware/HARDWARE_REFERENCE.md)
- [Examples Guide](examples/README.md)
- [Repository Structure](docs/repository-structure.md)
- [Continuous Integration](docs/CI.md)
- [Managed Components](docs/components.md)
- [Firmware and Factory Recovery](docs/firmware.md)
- [Release Tools](releases/README.md)
- [ESP-Brookesia Notes](docs/brookesia.md)

## Support and Contributions

Contributions and reproducible issue reports are welcome. Include the board
model and revision, example path, framework version, reproduction steps,
expected behavior, actual behavior, and relevant serial logs.

- [Contributing Guide](CONTRIBUTING.md)
- [Support](SUPPORT.md)
- [Security Policy](SECURITY.md)
- [Open an Issue](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/issues/new/choose)

## License

This repository is licensed under Apache License 2.0. See
[LICENSE.txt](LICENSE.txt).
