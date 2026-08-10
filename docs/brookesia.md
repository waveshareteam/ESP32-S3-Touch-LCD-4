# ESP-Brookesia Notes

[中文](brookesia_CN.md)

`examples/esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia` is a source-maintained rich UI example. Brookesia `0.4.2` is currently validated only with ESP-IDF `v5.5.5` and `v6.0.2` through the shared `Build Examples` workflow. Do not widen that compatibility statement until the complete matrix has passed after the upgrade.

The example depends on managed BSP, LVGL, ESP LVGL port, and ESP-Brookesia components, plus local demo applications under `components/apps`.

ESP-Brookesia Phone UI expects a valid LVGL input device. Use this example directly on ESP32-S3-Touch-LCD-4; ESP32-S3-LCD-4 does not populate GT911, so the example needs a no-touch navigation/input adaptation before it is treated as LCD-only firmware.

Keep Brookesia compatibility changes local only when they are specific to this board or demo composition. Reusable display, touch, audio, storage, or UI framework fixes should be moved into the shared component repository when practical.

If a future ESP-Brookesia or LVGL release requires source changes, update this note with the component versions validated by CI.
