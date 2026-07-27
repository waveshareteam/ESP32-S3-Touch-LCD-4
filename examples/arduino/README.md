# Arduino Examples

[中文](README_CN.md)

This directory contains 13 first-party Arduino sketches for ESP32-S3-Touch-LCD-4 and ESP32-S3-LCD-4. The current CI matrix compiles every sketch with Arduino-ESP32 core `3.3.11`, the `esp32s3` board target, and the bundled libraries under [`libraries/`](libraries/).

Directory number `04` is intentionally absent because the repository preserves the published sketch numbering. Bundled-library examples are useful upstream samples but are not first-party product CI targets.

## Sketches

| Sketch | Focus |
| --- | --- |
| [01_HelloWorld](01_HelloWorld/) | Basic LCD and Arduino GFX bring-up |
| [02_AsciiTable](02_AsciiTable/) | Character rendering and font output |
| [03_Drawing_points](03_Drawing_points/) | Drawing demo with optional GT911 pointer input |
| [05_GFX_PCF85063_simpleTime](05_GFX_PCF85063_simpleTime/) | PCF85063 RTC displayed with Arduino GFX |
| [06_GFX_ESPWiFiAnalyzer](06_GFX_ESPWiFiAnalyzer/) | Wi-Fi scan and channel visualization |
| [07_GFX_Clock](07_GFX_Clock/) | Graphical clock rendering |
| [08_LVGL_PCF85063_simpleTime](08_LVGL_PCF85063_simpleTime/) | PCF85063 RTC displayed with LVGL |
| [09_LVGL_Widgets](09_LVGL_Widgets/) | LVGL widgets with optional GT911 pointer input |
| [10_LVGL_SD](10_LVGL_SD/) | LVGL and SD card integration |
| [11_TWAItransmit](11_TWAItransmit/) | TWAI/CAN periodic transmit path |
| [12_TWAIreceive](12_TWAIreceive/) | TWAI/CAN receive path |
| [13_RS485](13_RS485/) | UART/RS485 communication |
| [14_LVGL_BatteryVoltage](14_LVGL_BatteryVoltage/) | CH32 battery-voltage sampling displayed with LVGL |

## Compile With Arduino CLI

Install the Arduino-ESP32 core configured by the current workflow, then compile from the repository root:

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

The workflow file is the source of truth for the core version and FQBN. The selected options configure ESP32-S3, hardware USB CDC, 16 MB Flash, OPI PSRAM, and the `app3M_fat9M_16MB` partition scheme.

## Board Variants

Only ESP32-S3-Touch-LCD-4 populates the GT911 touch controller. `03_Drawing_points` and `09_LVGL_Widgets` probe GT911 at runtime and can continue without pointer input. Treat other touch-dependent firmware as Touch-variant firmware unless its source or documentation explicitly supports display-only operation.

See [the combined example map](../README.md), [CI rules](../../docs/CI.md), and [firmware artifact downloads](../../releases/README.md).
