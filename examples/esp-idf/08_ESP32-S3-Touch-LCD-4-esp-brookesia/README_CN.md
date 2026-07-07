# ESP-Brookesia UI 示例

[English](README.md)

本示例展示 ESP32-S3-Touch-LCD-4 上较完整的 ESP-Brookesia Phone UI 应用结构。程序启动 BSP 显示和触摸后，创建 480 x 480 深色主题 Phone UI，并注册计算器和画板应用。同时示例还包含 CAN 发送任务，可作为较复杂工程组织方式的参考。

这是触摸驱动的 UI 示例。ESP32-S3-LCD-4 不带 GT911，只有在增加替代输入路径或无触摸导航模式后，才建议将该固件烧录到无触摸版本。

### 依赖组件

| 组件 | 版本 |
| --- | --- |
| `waveshare/esp32_s3_touch_lcd_4` | `3.0.0` |
| `espressif/esp_lvgl_port` | `^2` |
| `espressif/esp-brookesia` | `0.4.2` |
| ESP-IDF | `>=5.1.0` |

### 示例结构

| 路径 | 说明 |
| --- | --- |
| `main/main.cpp` | 显示启动、I2C 恢复、Brookesia Phone 创建、应用注册和 CAN 任务启动 |
| `components/apps/calculator` | 计算器应用 |
| `components/apps/draw` | 画板应用 |
| `components/apps/*/assets` | 应用图标资源 |
| `components/can` | TWAI/CAN 简单封装 |

### 快速复位处理

`main/main.cpp` 在调用 `bsp_display_start()` 前会先恢复 `GPIO15`/`GPIO7` I2C 总线。该处理用于减少快速复位后 CH32 或触摸/显示相关 I2C 设备状态残留导致的初始化失败。

### 编译和运行

```bash
cd examples/esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### 期望现象

- 屏幕显示 ESP-Brookesia Phone UI。
- ESP32-S3-Touch-LCD-4 上可以通过触摸打开计算器和画板应用。
- 串口显示显示初始化、应用注册和 CAN 任务相关日志。

### 常见问题

- 如果屏幕不亮，先运行 [../ioexpander](../ioexpander/README_CN.md) 验证 CH32 背光和复位控制。
- 如果 Brookesia 相关组件下载失败，清理本示例的 `managed_components/` 后重新构建。
- 如果 CAN 总线报错，确认 `components/can/can.hpp` 中的 `GPIO_NUM_6`/`GPIO_NUM_0` 与硬件连接一致，并确认总线上有其它节点提供 ACK。
