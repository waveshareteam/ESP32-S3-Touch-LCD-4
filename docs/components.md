# Components

ESP-IDF examples prefer managed components where possible:

- `waveshare/esp32_s3_touch_lcd_4` provides the board display, GT911 touch, and BSP integration used by LVGL and ESP-Brookesia examples. ESP32-S3-LCD-4 shares the display/peripheral path but does not populate GT911, so touch input should be treated as optional or disabled on that variant.
- `waveshare/pcf85063a` `2.0.0` provides the RTC driver used by the RTC example.
- `waveshare/custom_io_expander_ch32v003` `2.0.0` provides the board helper-controller integration used by RTC, SD, battery, and IO-expander examples.
- `lvgl/lvgl` is selected per example according to the LVGL major version being demonstrated.
- `espressif/esp-brookesia` is used by the ESP-Brookesia example.

Small local components remain only where they are board-example glue:

- `components/can` is used by TWAI/CAN receive, transmit, and Brookesia examples.
- `components/apps` contains local ESP-Brookesia demo applications and assets.

Arduino examples use bundled libraries from `examples/arduino/libraries` so CI and customer builds use the same known library set.

When a local reusable driver becomes available as a maintained Waveshare or Espressif component, prefer migrating the example manifest to the managed component and keep local code only for board-specific glue.
