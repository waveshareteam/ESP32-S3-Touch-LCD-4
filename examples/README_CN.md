# 示例

[English](README.md)

本目录包含 ESP32-S3-Touch-LCD-4 和 ESP32-S3-LCD-4 的 ESP-IDF 示例、Arduino 示例和随附库。两款板共用显示和主要外设；只有 ESP32-S3-Touch-LCD-4 带 GT911 触摸。

建议客户优先查看 ESP-IDF 示例，因为这些示例已经整理为从简单到复杂的学习路径。使用 ESP32-S3-LCD-4 时，建议先烧录显示和外设示例，再尝试依赖触摸输入的 UI 固件。ESP-IDF 工程和 Arduino sketches 在相关文件改动时都会由 GitHub Actions 检查。

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
| [ioexpander](esp-idf/ioexpander/README_CN.md) | CH32V003 IO 扩展测试和客户使用说明 | 基础硬件确认 |
| [01_RS485_Test](esp-idf/01_RS485_Test/README_CN.md) | UART/RS485 收发回环测试 | 外设点亮 |
| [02_SD_Test](esp-idf/02_SD_Test/README_CN.md) | SD 卡挂载、读写、格式化和 CH32 上电/复位流程 | 外设点亮 |
| [03_RTC_Test](esp-idf/03_RTC_Test/README_CN.md) | PCF85063A RTC I2C 读写和闹钟中断 | 外设点亮 |
| [04_TWAIreceive](esp-idf/04_TWAIreceive/README_CN.md) | TWAI/CAN 接收并回传收到的帧 | 外设点亮 |
| [05_TWAItransmit](esp-idf/05_TWAItransmit/README_CN.md) | TWAI/CAN 周期发送测试帧 | 外设点亮 |
| [06_lvgl_demo_v8](esp-idf/06_lvgl_demo_v8/README_CN.md) | BSP 显示初始化和 LVGL v8 widgets demo | 显示/UI |
| [07_lvgl_demo_v9](esp-idf/07_lvgl_demo_v9/README_CN.md) | BSP 显示初始化和 LVGL v9 benchmark demo | 显示/UI |
| [09_BatteryVoltage_LVGL](esp-idf/09_BatteryVoltage_LVGL/README_CN.md) | CH32 ADC 电池电压采样并显示到 LVGL | 显示/板级监控 |
| [08_ESP32-S3-Touch-LCD-4-esp-brookesia](esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia/README_CN.md) | ESP-Brookesia Phone UI、计算器、画板和 CAN 任务 | 进阶 UI |

ESP-IDF 和 Arduino 示例 CI 规则见 [../docs/CI_CN.md](../docs/CI_CN.md)，固件 artifact 打包说明见 [../releases/README_CN.md](../releases/README_CN.md)。

### Arduino 示例

Arduino sketches 和随附库位于 [arduino](arduino/)。`03_Drawing_points` 和 `09_LVGL_Widgets` 会在运行时探测 GT911；烧录到 ESP32-S3-LCD-4 时会跳过触摸输入并继续运行。当前 `Build Examples` 工作流使用 Arduino ESP32 core `3.3.11` 以及 [arduino/libraries](arduino/libraries/) 中当前仓库自带的库编译第一方产品示例。客户本地使用时，建议与 CI 保持一致：ESP32-S3 Dev Module、16 MB Flash、OPI PSRAM、USB CDC on boot，以及 `app3M_fat9M_16MB` 分区。
