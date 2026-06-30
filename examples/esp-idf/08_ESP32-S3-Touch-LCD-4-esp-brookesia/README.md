# ESP-Brookesia UI Example / ESP-Brookesia UI 示例

[English](#english) | [中文](#中文)

## English

This example demonstrates a larger ESP-Brookesia Phone UI application structure on ESP32-S3-Touch-LCD-4. After starting BSP display and touch, it creates a 480 x 480 dark Phone UI and registers calculator and draw-panel apps. It also includes a CAN transmit task, making it a useful reference for a more complex project layout.

### Dependencies

| Component | Version |
| --- | --- |
| `waveshare/esp32_s3_touch_lcd_4` | `3.0.0` |
| `espressif/esp_lvgl_port` | `^2` |
| `espressif/esp-brookesia` | `0.4.2` |
| ESP-IDF | `>=5.1.0` |

### Example Structure

| Path | Description |
| --- | --- |
| `main/main.cpp` | Display startup, I2C recovery, Brookesia Phone creation, app registration, and CAN task startup |
| `components/apps/calculator` | Calculator app |
| `components/apps/draw` | Draw-panel app |
| `components/apps/*/assets` | App icon assets |
| `components/can` | Small TWAI/CAN wrapper |

### Quick-Reset Handling

`main/main.cpp` recovers the I2C bus on `GPIO15`/`GPIO7` before calling `bsp_display_start()`. This reduces startup failures after quick resets when CH32 or related I2C display/touch devices retain stale state.

### Build And Run

```bash
cd examples/esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### Expected Behavior

- The screen shows the ESP-Brookesia Phone UI.
- Touch can open the calculator and draw-panel apps.
- Serial monitor shows display startup, app registration, and CAN task logs.

### Troubleshooting

- If the screen is dark, run [../ioexpander](../ioexpander/) first to verify CH32 backlight and reset control.
- If Brookesia components fail to download or resolve, remove this example's `managed_components/` and build again.
- If CAN bus errors appear, confirm that `GPIO_NUM_6`/`GPIO_NUM_0` in `components/can/can.hpp` match the hardware wiring and that another node is present to provide ACK.

## 中文

本示例展示 ESP32-S3-Touch-LCD-4 上较完整的 ESP-Brookesia Phone UI 应用结构。程序启动 BSP 显示和触摸后，创建 480 x 480 深色主题 Phone UI，并注册计算器和画板应用。同时示例还包含 CAN 发送任务，可作为较复杂工程组织方式的参考。

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
- 可以通过触摸打开计算器和画板应用。
- 串口显示显示初始化、应用注册和 CAN 任务相关日志。

### 常见问题

- 如果屏幕不亮，先运行 [../ioexpander](../ioexpander/) 验证 CH32 背光和复位控制。
- 如果 Brookesia 相关组件下载失败，清理本示例的 `managed_components/` 后重新构建。
- 如果 CAN 总线报错，确认 `components/can/can.hpp` 中的 `GPIO_NUM_6`/`GPIO_NUM_0` 与硬件连接一致，并确认总线上有其它节点提供 ACK。
