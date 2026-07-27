# Waveshare ESP32-S3-Touch-LCD-4 / ESP32-S3-LCD-4

[English](README.md)

本仓库同时适用于 Waveshare ESP32-S3-Touch-LCD-4 和 ESP32-S3-LCD-4 两款 4 英寸圆形 ESP32-S3 板卡。两者共用 480 x 480 LCD、16 MB Flash、8 MB PSRAM、RS485、TWAI/CAN、RTC、SD 卡、电池电压检测和 CH32V003 辅助控制器；Touch 版本额外带 GT911 电容触摸。

本仓库提供工厂固件、原理图、10 个第一方 ESP-IDF 工程和 13 个第一方 Arduino sketches。示例已按从简单外设到图形界面的顺序整理；两类开发框架都会在相关文件改动时由 GitHub Actions 自动检查。

仓库内原理图和本文引用的硬件参数覆盖 ESP32-S3-Touch-LCD-4 V4.0。对于 ESP32-S3-LCD-4，请先核对产品文档和实物板卡版本，再使用触摸相关或特定硬件版本的信息。

### 支持的板卡版本

| 板卡 | 触摸控制器 | 说明 |
| --- | --- | --- |
| ESP32-S3-Touch-LCD-4 | GT911 电容触摸 | 可直接使用显示、触摸、LVGL 和 ESP-Brookesia 示例。 |
| ESP32-S3-LCD-4 | 不带 GT911 触摸 | 显示、CH32、电池、SD、RS485、TWAI/CAN、RTC 和非触摸 UI 路径共用。只有示例明确说明支持时才能使用无触摸模式；其他依赖触摸的固件必须先适配再烧录。 |

### 主要特性

| 项目 | 说明 |
| --- | --- |
| 主控 | ESP32-S3，2.4 GHz Wi-Fi，Bluetooth LE 5 |
| 存储 | 16 MB Flash，8 MB PSRAM |
| 显示 | 4 英寸 480 x 480 LCD，支持 LVGL 和 ESP-Brookesia 示例 |
| 触摸 | ESP32-S3-Touch-LCD-4 带 GT911 电容触摸；ESP32-S3-LCD-4 不带触摸 |
| IO 扩展 | CH32V003，通过 I2C 控制背光、LCD/触摸复位、蜂鸣器、电源使能并读取电池 ADC |
| 常用外设 | RS485、TWAI/CAN、RTC、SD 卡、电池电压检测 |
| 主要开发方式 | ESP-IDF、Arduino ESP32 |

### 仓库目录

| 路径 | 内容 |
| --- | --- |
| [examples/esp-idf](examples/esp-idf/README_CN.md) | 10 个第一方 ESP-IDF 工程，从外设测试到 LVGL/ESP-Brookesia UI |
| [examples/arduino](examples/arduino/README_CN.md) | 13 个第一方 Arduino sketches 和随附库 |
| [config](config/) | 预留的共享配置片段目录；当前尚未启用共享 overlay |
| [examples/esp-idf/ioexpander](examples/esp-idf/ioexpander/README_CN.md) | CH32V003 IO 扩展独立测试程序和详细客户使用说明 |
| [docs/CI_CN.md](docs/CI_CN.md) | ESP-IDF 和 Arduino 示例 CI 规则 |
| [hardware](hardware/HARDWARE_REFERENCE_CN.md) | V4.0 硬件参考；原理图 PDF 位于 `hardware/schematics/` |
| [firmware](firmware/) | 工厂/恢复固件和烧录说明；[下载 V4.0 固件](firmware/ESP32-S3-Touch-LCD-4-V4.0-FactoryOnly-251122.bin) |
| [docs/firmware_CN.md](docs/firmware_CN.md) | 固件产物策略和 CI 边界 |
| [releases](releases/) | 固件打包和 artifact 下载工具 |
| [docs/components.md](docs/components.md) | 托管组件和本地 glue code 策略 |
| [docs/repository-structure.md](docs/repository-structure.md) | 仓库目录职责和边界 |

### 快速开始 ESP-IDF

如需复现当前 CI 矩阵，请安装 ESP-IDF v5.5.5 或 v6.0.2，然后从简单示例开始：

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

把 `PORT` 替换成实际串口，例如 Windows 下的 `COM8` 或 Linux 下的 `/dev/ttyACM0`。

推荐学习顺序：

1. [ioexpander](examples/esp-idf/ioexpander/README_CN.md)：先确认 CH32、背光、复位、蜂鸣器和电池 ADC 正常。
2. [01_RS485_Test](examples/esp-idf/01_RS485_Test/README_CN.md) 到 [05_TWAItransmit](examples/esp-idf/05_TWAItransmit/README_CN.md)：逐个验证板载外设。
3. [06_lvgl_demo_v8](examples/esp-idf/06_lvgl_demo_v8/README_CN.md) 或 [07_lvgl_demo_v9](examples/esp-idf/07_lvgl_demo_v9/README_CN.md)：验证显示、带触摸 BSP 启动流程和 LVGL。ESP32-S3-LCD-4 无 GT911，请将触摸输入视为不可用，除非示例明确支持无触摸模式。
4. [09_BatteryVoltage_LVGL](examples/esp-idf/09_BatteryVoltage_LVGL/README_CN.md)：学习在 LVGL 界面中显示电池电压。
5. [08_ESP32-S3-Touch-LCD-4-esp-brookesia](examples/esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia/README_CN.md)：查看较完整的 ESP-Brookesia UI 应用结构。

### 快速开始 Arduino

安装 Arduino CLI，配置 Espressif 开发板索引，并安装当前工作流固定的 core 版本。使用仓库随附库和 CI 板卡选项编译第一方 sketch：

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

全部 sketches、板卡选项和随附库边界见 [Arduino 示例索引](examples/arduino/README_CN.md)。core 版本或 FQBN 调整后，以工作流文件为准。

### CH32V003 IO 扩展说明

这块板子的部分功能不是 ESP32-S3 GPIO 直接控制，而是由 CH32V003 辅助芯片控制。常见例子包括 LCD 背光 PWM、LCD 复位、触摸复位、蜂鸣器、系统电源使能和电池 ADC。

关键硬件参数：

| 项目 | 值 |
| --- | --- |
| I2C SDA | `GPIO15` |
| I2C SCL | `GPIO7` |
| CH32 I2C 地址 | `0x24` |
| 背光控制 | `custom_io_expander_set_pwm()`，范围 0 到 255 |
| 电池电压换算 | `raw * 3.3 / 1023 * 3.0` |

客户如果遇到快速复位后屏幕、触摸或 CH32 寄存器写入异常，建议先运行 [examples/esp-idf/ioexpander](examples/esp-idf/ioexpander/README_CN.md)；该示例包含 I2C 总线恢复、CH32 重试初始化和 LCD/触摸复位流程。

### 示例和 CI

完整示例地图见 [examples/README_CN.md](examples/README_CN.md)、[10 个工程的 ESP-IDF 索引](examples/esp-idf/README_CN.md) 和 [13 个 sketches 的 Arduino 索引](examples/arduino/README_CN.md)。

CI 使用一个 `Build Examples` 工作流。只要源码、共享配置、发现脚本、工作流或发布打包等 CI 相关路径发生变化，工作流就会运行完整的第一方矩阵：ESP-IDF `v5.5.5` 和 `v6.0.2`（目标 `esp32s3`），以及 Arduino ESP32 core `3.3.11`。成功任务会上传由 `releases/package_firmware.py` 生成的可烧录 firmware artifacts。这些版本表示当前工作流配置，不是永久兼容承诺。

### 固件下载

- 仓库内的 [V4.0 工厂/恢复固件](firmware/ESP32-S3-Touch-LCD-4-V4.0-FactoryOnly-251122.bin) 是为带触摸板卡发布的二进制文件，不由 CI 重新构建。
- 从源码构建的示例包位于成功的 `Build Examples` 工作流运行中。可用 `python3 releases/download_artifacts.py --clean` 下载当前分支最新成功运行，也可用 `--run-id <run-id>` 指定运行。
- 工厂/恢复固件与 CI 生成的示例 artifacts 必须分开使用。详情见 [固件策略](docs/firmware_CN.md) 和 [发布工具](releases/README.md)。

### 文档

- [硬件参考](hardware/HARDWARE_REFERENCE_CN.md)
- [仓库结构](docs/repository-structure.md)
- [持续集成](docs/CI_CN.md)
- [托管组件](docs/components.md)
- [固件和工厂恢复](docs/firmware_CN.md)
- [贡献指南](CONTRIBUTING.md)、[支持](SUPPORT.md) 和 [安全策略](SECURITY.md)


### 常见问题

**屏幕不亮或背光不能调节**

请先确认 CH32 初始化成功。背光由 CH32 PWM 控制，不是普通 ESP32 LEDC 引脚。底层可用 `custom_io_expander_set_pwm()`，使用 BSP 时可调用 `bsp_display_brightness_set()`。

**快速复位后外设偶发异常**

CH32 或同一 I2C 总线上的器件不一定与 ESP32-S3 同步复位。应用启动时建议先恢复 I2C 总线，再初始化 CH32，并给 LCD/触摸复位脚一个低脉冲。`ioexpander`、SD、电池和 UI 示例已经包含相关恢复流程。

**电池电压显示不准确**

本仓库示例按照原理图分压比例使用 `3.0` 倍换算。如果客户修改了硬件分压或 ADC 参考，需要同步调整示例中的换算常量。

**Arduino CI 使用哪些库**

Arduino CI 会把 `examples/arduino/libraries` 传给 Arduino CLI，因此使用仓库内随附的 `GFX Library for Arduino`、`lvgl`、`SensorLib`、`WS_CH32_IO` 和 `EspSoftwareSerial`，不会从 Library Manager 下载替换版本。

### 支持

如果遇到问题，请先查看示例 README、串口日志和 [Issues](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/issues)。需要技术支持时，请通过 Waveshare 支持渠道联系，并提供产品版本、复现步骤、使用的 ESP-IDF/Arduino 版本和串口日志。

### 许可证

本仓库使用 Apache License 2.0。详情见 [LICENSE.txt](LICENSE.txt)。
