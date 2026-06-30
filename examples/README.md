# Examples / 示例

[中文](#中文) | [English](#english)

## 中文

本目录包含 ESP32-S3-Touch-LCD-4 的 ESP-IDF 示例、Arduino 示例和随附库。

建议客户优先查看 ESP-IDF 示例，因为这些示例已经整理为从简单到复杂的学习路径，并接入 GitHub Actions 自动构建。Arduino 示例保留用于 Arduino ESP32 开发流程，但当前不会被 ESP-IDF CI 构建。

### ESP-IDF 示例

每个 [esp-idf](esp-idf/) 下的一级目录都是独立 ESP-IDF 工程。进入选定示例目录后执行构建命令：

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

示例从外设点亮到图形界面逐步组织：

| 目录 | 用途 | 难度/阶段 |
| --- | --- | --- |
| [ioexpander](esp-idf/ioexpander/) | CH32V003 IO 扩展测试和客户使用说明 | 基础硬件确认 |
| [01_RS485_Test](esp-idf/01_RS485_Test/) | UART/RS485 收发回环测试 | 外设点亮 |
| [02_SD_Test](esp-idf/02_SD_Test/) | SD 卡挂载、读写、格式化和 CH32 上电/复位流程 | 外设点亮 |
| [03_RTC_Test](esp-idf/03_RTC_Test/) | PCF85063A RTC I2C 读写和闹钟中断 | 外设点亮 |
| [04_TWAIreceive](esp-idf/04_TWAIreceive/) | TWAI/CAN 接收并回传收到的帧 | 外设点亮 |
| [05_TWAItransmit](esp-idf/05_TWAItransmit/) | TWAI/CAN 周期发送测试帧 | 外设点亮 |
| [06_lvgl_demo_v8](esp-idf/06_lvgl_demo_v8/) | BSP 显示初始化和 LVGL v8 widgets demo | 显示/UI |
| [07_lvgl_demo_v9](esp-idf/07_lvgl_demo_v9/) | BSP 显示初始化和 LVGL v9 benchmark demo | 显示/UI |
| [09_BatteryVoltage_LVGL](esp-idf/09_BatteryVoltage_LVGL/) | CH32 ADC 电池电压采样并显示到 LVGL | 显示/板级监控 |
| [08_ESP32-S3-Touch-LCD-4-esp-brookesia](esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia/) | ESP-Brookesia Phone UI、计算器、画板和 CAN 任务 | 进阶 UI |

ESP-IDF 示例 CI 规则见 [../docs/CI.md](../docs/CI.md)。

### Arduino 示例

Arduino sketches 和随附库位于 [Arduino-v3.3.2](Arduino-v3.3.2/)。它们不参与 `ESP-IDF examples` 工作流构建。使用 Arduino 示例时，请确认 Arduino ESP32 core、板卡选项、分区和库版本与示例要求一致。

## English

This directory contains ESP-IDF examples, Arduino sketches, and bundled libraries for ESP32-S3-Touch-LCD-4.

Customers are encouraged to start with the ESP-IDF examples because they are organized as a learning path from simple peripheral bring-up to larger UI applications, and they are automatically built by GitHub Actions. Arduino examples remain available for the Arduino ESP32 workflow, but they are not built by the ESP-IDF CI workflow.

### ESP-IDF Examples

Each first-level directory under [esp-idf](esp-idf/) is a standalone ESP-IDF project. Run build commands from the selected example directory:

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

The examples are ordered from board-level bring-up to graphical UI:

| Directory | Purpose | Level |
| --- | --- | --- |
| [ioexpander](esp-idf/ioexpander/) | CH32V003 IO expander test and customer guide | Basic hardware check |
| [01_RS485_Test](esp-idf/01_RS485_Test/) | UART/RS485 receive and echo test | Peripheral bring-up |
| [02_SD_Test](esp-idf/02_SD_Test/) | SD card mount, read/write, format, and CH32 power/reset flow | Peripheral bring-up |
| [03_RTC_Test](esp-idf/03_RTC_Test/) | PCF85063A RTC I2C access and alarm interrupt | Peripheral bring-up |
| [04_TWAIreceive](esp-idf/04_TWAIreceive/) | TWAI/CAN receive path that echoes received frames | Peripheral bring-up |
| [05_TWAItransmit](esp-idf/05_TWAItransmit/) | TWAI/CAN periodic test-frame transmitter | Peripheral bring-up |
| [06_lvgl_demo_v8](esp-idf/06_lvgl_demo_v8/) | BSP display startup and LVGL v8 widgets demo | Display/UI |
| [07_lvgl_demo_v9](esp-idf/07_lvgl_demo_v9/) | BSP display startup and LVGL v9 benchmark demo | Display/UI |
| [09_BatteryVoltage_LVGL](esp-idf/09_BatteryVoltage_LVGL/) | CH32 ADC battery-voltage sampling displayed in LVGL | Display/board monitor |
| [08_ESP32-S3-Touch-LCD-4-esp-brookesia](esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia/) | ESP-Brookesia Phone UI with calculator, draw panel, and CAN task | Advanced UI |

See [../docs/CI.md](../docs/CI.md) for ESP-IDF example CI rules.

### Arduino Examples

Arduino sketches and bundled libraries are under [Arduino-v3.3.2](Arduino-v3.3.2/). They are not built by the `ESP-IDF examples` workflow. When using Arduino examples, verify the Arduino ESP32 core, board options, partition scheme, and library versions required by the selected sketch.
