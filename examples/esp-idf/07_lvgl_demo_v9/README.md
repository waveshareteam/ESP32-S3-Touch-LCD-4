# LVGL v9 Demo / LVGL v9 演示

[中文](#中文) | [English](#english)

## 中文

本示例用于验证 ESP32-S3-Touch-LCD-4 在 LVGL v9 下的显示初始化和渲染性能。程序启动 Waveshare BSP 显示栈后运行 `lv_demo_benchmark()`。

### 依赖组件

| 组件 | 版本 |
| --- | --- |
| `waveshare/esp32_s3_touch_lcd_4` | `3.0.0` |
| `lvgl/lvgl` | `9.5.0` |

### 快速复位处理

示例在启动显示前会对 `GPIO15`/`GPIO7` 上的 I2C 总线执行恢复流程。该流程用于解决快速复位时 CH32 或同一 I2C 总线设备未完全复位，导致后续 CH32 寄存器写入或显示初始化失败的问题。

### 编译和运行

```bash
cd examples/esp-idf/07_lvgl_demo_v9
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### 期望现象

- LCD 背光点亮。
- 屏幕运行 LVGL v9 benchmark demo。
- 串口输出 LVGL/BSP 初始化日志。

### 常见问题

- 如果背光不亮，先运行 [../ioexpander](../ioexpander/) 验证 CH32 PWM 控制是否正常。
- 如果画面异常或触摸方向不对，检查 BSP 版本和显示旋转配置。
- 如果客户需要普通控件演示，可在 `main/main.c` 中启用 `lv_demo_widgets()`，并注释 `lv_demo_benchmark()`。

## English

This example verifies display startup and rendering behavior with LVGL v9 on ESP32-S3-Touch-LCD-4. It starts the Waveshare BSP display stack and runs `lv_demo_benchmark()`.

### Dependencies

| Component | Version |
| --- | --- |
| `waveshare/esp32_s3_touch_lcd_4` | `3.0.0` |
| `lvgl/lvgl` | `9.5.0` |

### Quick-Reset Handling

Before starting the display, the example recovers the I2C bus on `GPIO15`/`GPIO7`. This flow addresses quick-reset cases where CH32 or another I2C device has not fully reset, causing later CH32 register writes or display startup to fail.

### Build And Run

```bash
cd examples/esp-idf/07_lvgl_demo_v9
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### Expected Behavior

- LCD backlight turns on.
- The screen runs the LVGL v9 benchmark demo.
- Serial monitor shows LVGL/BSP startup logs.

### Troubleshooting

- If the backlight is dark, run [../ioexpander](../ioexpander/) first to verify CH32 PWM control.
- If the image or touch orientation is wrong, check BSP version and display rotation configuration.
- If a normal widget demo is preferred, enable `lv_demo_widgets()` in `main/main.c` and comment out `lv_demo_benchmark()`.
