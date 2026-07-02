# Waveshare ESP32-S3-Touch-LCD-4

[English](README.md)

ESP32-S3-Touch-LCD-4 是 Waveshare 的 4 英寸圆形触控开发板，板载 ESP32-S3、480 x 480 LCD、触摸、16 MB Flash、8 MB PSRAM，以及 RS485、TWAI/CAN、RTC、SD 卡、电池电压检测等常用 HMI 外设。它适合快速开发智能控制面板、工业人机界面、家居网关、仪表盘、照明控制器和带触摸交互的嵌入式应用。

本仓库提供工厂固件、原理图、Arduino 示例和 ESP-IDF 示例。示例已按从简单外设到图形界面的顺序整理；ESP-IDF 示例和 Arduino sketches 会在相关文件改动时由 GitHub Actions 自动检查。

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
| [examples/esp-idf](examples/esp-idf/README_CN.md) | ESP-IDF 示例，从外设测试到 LVGL/ESP-Brookesia UI |
| [examples/Arduino-v3.3.2](examples/Arduino-v3.3.2/) | Arduino 示例和随附库 |
| [examples/esp-idf/ioexpander](examples/esp-idf/ioexpander/README_CN.md) | CH32V003 IO 扩展独立测试程序和详细客户使用说明 |
| [docs/CI_CN.md](docs/CI_CN.md) | ESP-IDF 和 Arduino 示例 CI 规则 |
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

1. [ioexpander](examples/esp-idf/ioexpander/README_CN.md)：先确认 CH32、背光、复位、蜂鸣器和电池 ADC 正常。
2. [01_RS485_Test](examples/esp-idf/01_RS485_Test/README_CN.md) 到 [05_TWAItransmit](examples/esp-idf/05_TWAItransmit/README_CN.md)：逐个验证板载外设。
3. [06_lvgl_demo_v8](examples/esp-idf/06_lvgl_demo_v8/README_CN.md) 或 [07_lvgl_demo_v9](examples/esp-idf/07_lvgl_demo_v9/README_CN.md)：验证显示、触摸和 LVGL。
4. [09_BatteryVoltage_LVGL](examples/esp-idf/09_BatteryVoltage_LVGL/README_CN.md)：学习在 LVGL 界面中显示电池电压。
5. [08_ESP32-S3-Touch-LCD-4-esp-brookesia](examples/esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia/README_CN.md)：查看较完整的 ESP-Brookesia UI 应用结构。

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

ESP-IDF 示例目录说明见 [examples/README_CN.md](examples/README_CN.md) 和 [examples/esp-idf/README_CN.md](examples/esp-idf/README_CN.md)。

CI 现在包含两个示例工作流。`ESP-IDF examples` 使用 ESP-IDF `v5.5.4` 和 `v6.0.2` 构建改动过的 ESP-IDF 工程，目标为 `esp32s3`。`Arduino examples` 使用 Arduino ESP32 core `3.3.8`、ESP32-S3 Dev Module FQBN、16 MB Flash、OPI PSRAM、USB CDC on boot，以及 `examples/Arduino-v3.3.2/libraries` 下的随附库编译改动过的 sketches。

### 常见问题

**屏幕不亮或背光不能调节**

请先确认 CH32 初始化成功。背光由 CH32 PWM 控制，不是普通 ESP32 LEDC 引脚。底层可用 `custom_io_expander_set_pwm()`，使用 BSP 时可调用 `bsp_display_brightness_set()`。

**快速复位后外设偶发异常**

CH32 或同一 I2C 总线上的器件不一定与 ESP32-S3 同步复位。应用启动时建议先恢复 I2C 总线，再初始化 CH32，并给 LCD/触摸复位脚一个低脉冲。`ioexpander`、SD、电池和 UI 示例已经包含相关恢复流程。

**电池电压显示不准确**

本仓库示例按照原理图分压比例使用 `3.0` 倍换算。如果客户修改了硬件分压或 ADC 参考，需要同步调整示例中的换算常量。

**Arduino CI 使用哪些库**

Arduino CI 会把 `examples/Arduino-v3.3.2/libraries` 传给 Arduino CLI，因此使用仓库内随附的 `GFX Library for Arduino`、`lvgl`、`SensorLib`、`WS_CH32_IO` 和 `EspSoftwareSerial`，不会从 Library Manager 下载替换版本。

### 支持

如果遇到问题，请先查看示例 README、串口日志和 [Issues](https://github.com/waveshareteam/ESP32-S3-Touch-LCD-4/issues)。需要技术支持时，请通过 Waveshare 支持渠道联系，并提供产品版本、复现步骤、使用的 ESP-IDF/Arduino 版本和串口日志。

### 许可证

本仓库使用 Apache License 2.0。详情见 [LICENSE](LICENSE)。
