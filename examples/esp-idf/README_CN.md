# ESP-IDF 示例

[English](README.md)

这些示例面向 ESP32-S3-Touch-LCD-4 和 ESP32-S3-LCD-4 的 `esp32s3` 目标芯片，并在 CI 中使用 ESP-IDF `v5.5.4` 和 `v6.0.2` 构建。每个可构建示例目录都包含自己的 `CMakeLists.txt` 和 `main/` 目录。

### 推荐学习路径

建议先运行 `ioexpander`，确认 CH32、背光、复位和电池 ADC 工作正常，再逐步验证其它外设和 UI 示例。ESP32-S3-LCD-4 不带 GT911，因此依赖触摸输入的 UI 示例应视为 Touch 版本固件，除非对应 README 或源码明确说明可以无触摸运行。

| 顺序 | 示例 | 主要验证内容 |
| --- | --- | --- |
| 00 | [ioexpander](ioexpander/README_CN.md) | CH32V003 IO 扩展、LCD/触摸复位、背光 PWM、蜂鸣器、RTC_INT、电池 ADC |
| 01 | [RS485_Test](01_RS485_Test/README_CN.md) | UART/RS485 引脚和串口数据路径 |
| 02 | [SD_Test](02_SD_Test/README_CN.md) | SD 卡挂载、文件写入/读取、格式化和 CH32 上电/复位流程 |
| 03 | [RTC_Test](03_RTC_Test/README_CN.md) | PCF85063A RTC I2C 访问、时间设置、闹钟中断 |
| 04 | [TWAIreceive](04_TWAIreceive/README_CN.md) | TWAI/CAN 接收路径和收到帧回传 |
| 05 | [TWAItransmit](05_TWAItransmit/README_CN.md) | TWAI/CAN 周期发送路径 |
| 06 | [lvgl_demo_v8](06_lvgl_demo_v8/README_CN.md) | BSP 显示启动、GT911 触摸初始化和 LVGL v8 widgets demo |
| 07 | [lvgl_demo_v9](07_lvgl_demo_v9/README_CN.md) | BSP 显示启动、GT911 触摸相关启动和 LVGL v9 benchmark demo |
| 08 | [BatteryVoltage_LVGL](09_BatteryVoltage_LVGL/README_CN.md) | CH32 ADC 电池电压采样并显示到 LVGL |
| 09 | [ESP32-S3-Touch-LCD-4-esp-brookesia](08_ESP32-S3-Touch-LCD-4-esp-brookesia/README_CN.md) | 进阶触摸驱动 ESP-Brookesia Phone UI、应用注册和外设任务 |

### 通用构建命令

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

把 `PORT` 替换成实际串口。第一次构建会下载 managed components，耗时会更长。

### 添加新示例

新增 ESP-IDF 示例时建议保持小而聚焦：

- 每个独立示例使用一个一级目录。
- 示例目录应包含 `README.md`、`CMakeLists.txt`、`main/`，需要默认配置时添加 `sdkconfig.defaults`。
- README 必须提供中英文说明，包含硬件连接、构建步骤、期望现象和常见问题。
- 如果使用板载 CH32，请说明相关 EXIO 映射、I2C 地址和快速复位恢复流程。
- 不要提交 `build/`、`managed_components/`、`dependencies.lock`、本地 `sdkconfig` 或 `sdkconfig.old`，除非这些文件被明确设计为可维护配置。
