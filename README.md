# Waveshare ESP32-S3-Touch-LCD-4

[English](#english) | [中文](#中文)

## English

ESP32-S3-Touch-LCD-4 is a Waveshare 4-inch round touch development board based on ESP32-S3. It integrates a 480 x 480 LCD, capacitive touch, 16 MB Flash, 8 MB PSRAM, and common HMI peripherals such as RS485, TWAI/CAN, RTC, SD card, and battery-voltage sensing. It is suitable for smart control panels, industrial HMI projects, home gateways, dashboards, lighting controllers, and touch-enabled embedded products.

This repository provides factory firmware, schematic files, Arduino examples, and ESP-IDF examples. The ESP-IDF examples are organized from basic peripheral bring-up to graphical UI applications and are checked by GitHub Actions.

### Key Features

| Item | Description |
| --- | --- |
| MCU | ESP32-S3 with 2.4 GHz Wi-Fi and Bluetooth LE 5 |
| Memory | 16 MB Flash and 8 MB PSRAM |
| Display | 4-inch 480 x 480 LCD with LVGL and ESP-Brookesia examples |
| Touch | Capacitive touch initialized through the BSP display interface |
| IO expander | CH32V003 over I2C for backlight, LCD/touch reset, buzzer, power enable, and battery ADC |
| Peripherals | RS485, TWAI/CAN, RTC, SD card, battery-voltage monitor |
| Main frameworks | ESP-IDF and Arduino ESP32 |

### Repository Layout

| Path | Content |
| --- | --- |
| [examples/esp-idf](examples/esp-idf/) | ESP-IDF examples from peripheral tests to LVGL/ESP-Brookesia UI |
| [examples/Arduino-v3.3.2](examples/Arduino-v3.3.2/) | Arduino sketches and bundled libraries |
| [examples/esp-idf/ioexpander](examples/esp-idf/ioexpander/) | Standalone CH32V003 IO expander test and detailed customer guide |
| [docs/CI.md](docs/CI.md) | ESP-IDF example CI rules |
| [Schematic](Schematic/) | V4.0 schematic PDF |
| [Firmware](Firmware/) | Factory firmware image |

### ESP-IDF Quick Start

Install ESP-IDF v5.5.4 or v6.0.x, then start with a small board-level example:

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
3. [06_lvgl_demo_v8](examples/esp-idf/06_lvgl_demo_v8/) or [07_lvgl_demo_v9](examples/esp-idf/07_lvgl_demo_v9/): validate display, touch, and LVGL.
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

CI builds ESP-IDF examples only; Arduino sketches are intentionally ignored. Pull requests or pushes to `main` that change `examples/esp-idf/`, the example indexes, the root README, CI documentation, the workflow, or the example discovery script trigger the `ESP-IDF examples` workflow. The workflow builds target `esp32s3` with ESP-IDF `v5.5.4` and `v6.0.2`.

### FAQ

**The screen is dark or brightness does not change**

Check that CH32 initialization succeeds. The backlight is controlled by CH32 PWM, not a regular ESP32 LEDC pin. Use `custom_io_expander_set_pwm()` at the low level, or `bsp_display_brightness_set()` when using the BSP.

**Peripherals occasionally fail after quick reset**

CH32 or other devices on the same I2C bus may not reset at the same time as ESP32-S3. At application startup, recover the I2C bus, initialize CH32, and pulse the LCD/touch reset pins low before releasing them. The `ioexpander`, SD, battery, and UI examples already include this recovery flow.

**Battery voltage looks wrong**

The examples use a divider ratio of `3.0` according to the schematic. If the hardware divider or ADC reference is changed, update the conversion constants in the examples.

**Why is Arduino not built by CI?**

The current automated checks focus on ESP-IDF examples. Arduino sketches and bundled libraries remain in the repository for customers, but they are not built by the `ESP-IDF examples` workflow.

### Support

If you run into problems, first check the example README files, serial logs, and [Issues](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/issues). For technical support, contact Waveshare support and include the product version, reproduction steps, ESP-IDF/Arduino version, and serial log.

### License

This repository is licensed under Apache License 2.0. See [LICENSE](LICENSE) for details.

## 中文

ESP32-S3-Touch-LCD-4 是 Waveshare 的 4 英寸圆形触控开发板，板载 ESP32-S3、480 x 480 LCD、触摸、16 MB Flash、8 MB PSRAM，以及 RS485、TWAI/CAN、RTC、SD 卡、电池电压检测等常用 HMI 外设。它适合快速开发智能控制面板、工业人机界面、家居网关、仪表盘、照明控制器和带触摸交互的嵌入式应用。

本仓库提供工厂固件、原理图、Arduino 示例和 ESP-IDF 示例。ESP-IDF 示例已按从简单外设到图形界面的顺序整理，并接入 GitHub Actions 自动构建。

### 主要特性

| 项目 | 说明 |
| --- | --- |
| 主控 | ESP32-S3，2.4 GHz Wi-Fi，Bluetooth LE 5 |
| 存储 | 16 MB Flash，8 MB PSRAM |
| 显示 | 4 英寸 480 x 480 LCD，支持 LVGL 和 ESP-Brookesia 示例 |
| 触摸 | 电容触摸，由 BSP 显示接口初始化 |
| IO 扩展 | CH32V003，通过 I2C 控制背光、LCD/触摸复位、蜂鸣器、电源使能并读取电池 ADC |
| 常用外设 | RS485、TWAI/CAN、RTC、SD 卡、电池电压检测 |
| 主要开发方式 | ESP-IDF、Arduino ESP32 |

### 仓库目录

| 路径 | 内容 |
| --- | --- |
| [examples/esp-idf](examples/esp-idf/) | ESP-IDF 示例，从外设测试到 LVGL/ESP-Brookesia UI |
| [examples/Arduino-v3.3.2](examples/Arduino-v3.3.2/) | Arduino 示例和随附库 |
| [examples/esp-idf/ioexpander](examples/esp-idf/ioexpander/) | CH32V003 IO 扩展独立测试程序和详细客户使用说明 |
| [docs/CI.md](docs/CI.md) | ESP-IDF 示例 CI 规则 |
| [Schematic](Schematic/) | V4.0 原理图 PDF |
| [Firmware](Firmware/) | 工厂固件文件 |

### 快速开始 ESP-IDF

建议先确认已安装 ESP-IDF v5.5.4 或 v6.0.x，然后从简单示例开始：

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

把 `PORT` 替换成实际串口，例如 Windows 下的 `COM8` 或 Linux 下的 `/dev/ttyACM0`。

推荐学习顺序：

1. [ioexpander](examples/esp-idf/ioexpander/)：先确认 CH32、背光、复位、蜂鸣器和电池 ADC 正常。
2. [01_RS485_Test](examples/esp-idf/01_RS485_Test/) 到 [05_TWAItransmit](examples/esp-idf/05_TWAItransmit/)：逐个验证板载外设。
3. [06_lvgl_demo_v8](examples/esp-idf/06_lvgl_demo_v8/) 或 [07_lvgl_demo_v9](examples/esp-idf/07_lvgl_demo_v9/)：验证显示、触摸和 LVGL。
4. [09_BatteryVoltage_LVGL](examples/esp-idf/09_BatteryVoltage_LVGL/)：学习在 LVGL 界面中显示电池电压。
5. [08_ESP32-S3-Touch-LCD-4-esp-brookesia](examples/esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia/)：查看较完整的 ESP-Brookesia UI 应用结构。

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

客户如果遇到快速复位后屏幕、触摸或 CH32 寄存器写入异常，建议先运行 [examples/esp-idf/ioexpander](examples/esp-idf/ioexpander/)；该示例包含 I2C 总线恢复、CH32 重试初始化和 LCD/触摸复位流程。

### 示例和 CI

ESP-IDF 示例目录说明见 [examples/README.md](examples/README.md) 和 [examples/esp-idf/README.md](examples/esp-idf/README.md)。

CI 仅构建 ESP-IDF 示例，不构建 Arduino 示例。PR 或 main 分支中只要改动 `examples/esp-idf/`、示例索引、根 README、CI 文档、工作流或示例发现脚本，就会触发 `ESP-IDF examples` 工作流。该工作流使用 ESP-IDF `v5.5.4` 和 `v6.0.2` 两个版本构建目标 `esp32s3`。

### 常见问题

**屏幕不亮或背光不能调节**

请先确认 CH32 初始化成功。背光由 CH32 PWM 控制，不是普通 ESP32 LEDC 引脚。底层可用 `custom_io_expander_set_pwm()`，使用 BSP 时可调用 `bsp_display_brightness_set()`。

**快速复位后外设偶发异常**

CH32 或同一 I2C 总线上的器件不一定与 ESP32-S3 同步复位。应用启动时建议先恢复 I2C 总线，再初始化 CH32，并给 LCD/触摸复位脚一个低脉冲。`ioexpander`、SD、电池和 UI 示例已经包含相关恢复流程。

**电池电压显示不准确**

本仓库示例按照原理图分压比例使用 `3.0` 倍换算。如果客户修改了硬件分压或 ADC 参考，需要同步调整示例中的换算常量。

**Arduino 为什么没有 CI**

当前自动检查聚焦 ESP-IDF 示例。Arduino 示例和随附库保留在仓库中供客户参考，但不会被 `ESP-IDF examples` 工作流构建。

### 支持

如果遇到问题，请先查看示例 README、串口日志和 [Issues](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/issues)。需要技术支持时，请通过 Waveshare 支持渠道联系，并提供产品版本、复现步骤、使用的 ESP-IDF/Arduino 版本和串口日志。

### 许可证

本仓库使用 Apache License 2.0。详情见 [LICENSE](LICENSE)。
