# Arduino 示例

[English](README.md)

本目录包含 ESP32-S3-Touch-LCD-4 和 ESP32-S3-LCD-4 的 13 个第一方 Arduino sketches。当前 CI 矩阵使用 Arduino-ESP32 core `3.3.11`、`esp32s3` 开发板目标和 [`libraries/`](libraries/) 下的随附库编译每个 sketch。

目录编号有意保留已发布的 sketches 编号，因此没有 `04`。随附库内部的 examples 可作为上游库参考，但不属于第一方产品 CI 目标。

## Sketches

| Sketch | 用途 |
| --- | --- |
| [01_HelloWorld](01_HelloWorld/) | LCD 和 Arduino GFX 基础点亮 |
| [02_AsciiTable](02_AsciiTable/) | 字符和字体渲染 |
| [03_Drawing_points](03_Drawing_points/) | 支持可选 GT911 指针输入的绘图 demo |
| [05_GFX_PCF85063_simpleTime](05_GFX_PCF85063_simpleTime/) | 使用 Arduino GFX 显示 PCF85063 RTC |
| [06_GFX_ESPWiFiAnalyzer](06_GFX_ESPWiFiAnalyzer/) | Wi-Fi 扫描和信道可视化 |
| [07_GFX_Clock](07_GFX_Clock/) | 图形时钟渲染 |
| [08_LVGL_PCF85063_simpleTime](08_LVGL_PCF85063_simpleTime/) | 使用 LVGL 显示 PCF85063 RTC |
| [09_LVGL_Widgets](09_LVGL_Widgets/) | 支持可选 GT911 指针输入的 LVGL widgets |
| [10_LVGL_SD](10_LVGL_SD/) | LVGL 和 SD 卡集成 |
| [11_TWAItransmit](11_TWAItransmit/) | TWAI/CAN 周期发送 |
| [12_TWAIreceive](12_TWAIreceive/) | TWAI/CAN 接收 |
| [13_RS485](13_RS485/) | UART/RS485 通信 |
| [14_LVGL_BatteryVoltage](14_LVGL_BatteryVoltage/) | 使用 LVGL 显示 CH32 电池电压采样 |

## 使用 Arduino CLI 编译

安装当前工作流配置的 Arduino-ESP32 core，然后从仓库根目录编译：

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

core 版本和 FQBN 以工作流文件为准。以上选项配置 ESP32-S3、硬件 USB CDC、16 MB Flash、OPI PSRAM 和 `app3M_fat9M_16MB` 分区。

## 板卡版本

只有 ESP32-S3-Touch-LCD-4 带 GT911 触摸控制器。`03_Drawing_points` 和 `09_LVGL_Widgets` 会在运行时探测 GT911，无指针输入时仍可继续运行。其他依赖触摸的固件应视为 Touch 版本固件，除非源码或文档明确说明支持无触摸模式。

其他说明见 [完整示例地图](../README_CN.md)、[CI 规则](../../docs/CI_CN.md) 和 [固件 artifact 下载](../../releases/README_CN.md)。
