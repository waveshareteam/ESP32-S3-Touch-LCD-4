# LVGL v9 演示

[English](README.md)

本示例用于验证 ESP32-S3-Touch-LCD-4 在 LVGL v9 下的显示初始化和渲染性能。程序启动 Waveshare BSP 显示栈后运行 `lv_demo_benchmark()`。

ESP32-S3-LCD-4 共用 LCD 和外设路径，但不带 GT911。benchmark 本身以显示为主，不过 BSP 启动路径具备触摸初始化能力；如果无触摸板需要跳过 GT911 探测，请先适配启动流程。

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
- ESP32-S3-LCD-4 没有触摸输入，即使 benchmark 显示路径本身可用也不能提供触摸交互。

### 常见问题

- 如果背光不亮，先运行 [../ioexpander](../ioexpander/README_CN.md) 验证 CH32 PWM 控制是否正常。
- 如果画面异常或触摸方向不对，检查 BSP 版本和显示旋转配置。
- 如果客户需要普通控件演示，可在 `main/main.c` 中启用 `lv_demo_widgets()`，并注释 `lv_demo_benchmark()`。
