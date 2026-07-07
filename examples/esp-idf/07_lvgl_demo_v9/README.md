# LVGL v9 Demo

[中文](README_CN.md)

This example verifies display startup and rendering behavior with LVGL v9 on ESP32-S3-Touch-LCD-4. It starts the Waveshare BSP display stack and runs `lv_demo_benchmark()`.

ESP32-S3-LCD-4 shares the LCD and peripheral paths but does not populate GT911. The benchmark itself is display-focused, but the BSP startup path is touch-capable; adapt startup if the LCD-only board must skip GT911 probing.

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
- ESP32-S3-LCD-4 has no touch input even when the benchmark display path is otherwise suitable.

### Troubleshooting

- If the backlight is dark, run [../ioexpander](../ioexpander/) first to verify CH32 PWM control.
- If the image or touch orientation is wrong, check BSP version and display rotation configuration.
- If a normal widget demo is preferred, enable `lv_demo_widgets()` in `main/main.c` and comment out `lv_demo_benchmark()`.
