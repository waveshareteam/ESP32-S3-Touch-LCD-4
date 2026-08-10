# ESP-Brookesia 说明

[English](brookesia.md)

`examples/esp-idf/08_ESP32-S3-Touch-LCD-4-esp-brookesia` 是受维护的丰富 UI 示例。Brookesia `0.4.2` 当前仅在 ESP-IDF `v5.5.5` 和 `v6.0.2` 上通过共享工作流验证；升级后须完成完整矩阵验证，才可放宽该兼容范围。

该示例依赖托管 BSP、LVGL、ESP LVGL port、ESP-Brookesia 组件和本地 `components/apps`。GT911 触摸仅存在于 Touch 版本；LCD-only 板卡需先适配无触摸导航。
