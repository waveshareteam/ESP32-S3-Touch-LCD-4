# LVGL v8 Demo

[中文](README_CN.md)

This example verifies BSP display startup, touch initialization, and LVGL v8 rendering on ESP32-S3-Touch-LCD-4. It starts the Waveshare BSP display stack and runs `lv_demo_widgets()`.

ESP32-S3-LCD-4 shares the LCD and CH32 paths but does not populate GT911. Use this example directly on the Touch variant; for the LCD-only variant, use a display-only path or adapt BSP startup if touch probing blocks boot.

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
- Touch can operate LVGL controls on ESP32-S3-Touch-LCD-4.

### Troubleshooting

- If the screen is dark, run [../ioexpander](../ioexpander/) first to verify CH32, backlight PWM, and LCD/touch reset.
- If build fails, confirm ESP-IDF v5.5.4 or v6.0.2 is used, remove stale `managed_components/`, and retry.
- If touch orientation is wrong, check BSP display rotation and touch configuration.
