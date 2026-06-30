# ESP-Brookesia UI Example

[中文](README_CN.md)

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
