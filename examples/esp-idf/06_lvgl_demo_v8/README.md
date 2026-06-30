# LVGL v8 Demo / LVGL v8 演示

[中文](#中文) | [English](#english)

## 中文

本示例用于验证 ESP32-S3-Touch-LCD-4 的 BSP 显示初始化、触摸初始化和 LVGL v8 渲染流程。程序启动后调用 Waveshare BSP 初始化屏幕，然后运行 `lv_demo_widgets()`。

### 依赖组件

| 组件 | 版本 |
| --- | --- |
| `waveshare/esp32_s3_touch_lcd_4` | `3.0.0` |
| `lvgl/lvgl` | `8.4.*` |

### 快速复位处理

示例在启动显示前会对 `GPIO15`/`GPIO7` 上的 I2C 总线执行恢复流程。这样可以减少快速复位后 CH32 或同一 I2C 总线设备未同步复位导致的显示初始化异常。

### 编译和运行

```bash
cd examples/esp-idf/06_lvgl_demo_v8
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

首次构建会下载 BSP 和 LVGL managed components，耗时较长。

### 期望现象

- LCD 背光点亮。
- 屏幕显示 LVGL widgets demo。
- 触摸可以操作 LVGL 控件。

### 常见问题

- 如果屏幕不亮，先运行 [../ioexpander](../ioexpander/) 确认 CH32、背光 PWM 和 LCD/触摸复位正常。
- 如果构建失败，确认使用 ESP-IDF v5.5.4 或 v6.0.2，并清理旧的 `managed_components/` 后重试。
- 如果触摸方向不正确，检查 BSP 显示旋转和触摸配置。

## English

This example verifies BSP display startup, touch initialization, and LVGL v8 rendering on ESP32-S3-Touch-LCD-4. It starts the Waveshare BSP display stack and runs `lv_demo_widgets()`.

### Dependencies

| Component | Version |
| --- | --- |
| `waveshare/esp32_s3_touch_lcd_4` | `3.0.0` |
| `lvgl/lvgl` | `8.4.*` |

### Quick-Reset Handling

Before starting the display, the example recovers the I2C bus on `GPIO15`/`GPIO7`. This reduces display startup failures after quick resets when CH32 or another I2C device has not reset together with ESP32-S3.

### Build And Run

```bash
cd examples/esp-idf/06_lvgl_demo_v8
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

The first build downloads BSP and LVGL managed components, so it may take longer.

### Expected Behavior

- LCD backlight turns on.
- The screen shows the LVGL widgets demo.
- Touch can operate LVGL controls.

### Troubleshooting

- If the screen is dark, run [../ioexpander](../ioexpander/) first to verify CH32, backlight PWM, and LCD/touch reset.
- If build fails, confirm ESP-IDF v5.5.4 or v6.0.2 is used, remove stale `managed_components/`, and retry.
- If touch orientation is wrong, check BSP display rotation and touch configuration.
