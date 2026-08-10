# 组件

[English](components.md)

ESP-IDF 示例优先使用托管组件：`waveshare/esp32_s3_touch_lcd_4` 提供板级 BSP，`waveshare/pcf85063a` 提供 RTC，`waveshare/custom_io_expander_ch32v003` 提供辅助控制器，`lvgl/lvgl` 按示例选择主版本，`espressif/esp-brookesia` 用于 Brookesia 示例。

本地 `components/can` 和 `components/apps` 仅保留板级示例胶水代码。Arduino 示例使用 `examples/arduino/libraries` 中的随附库，以使 CI 与用户构建使用相同版本。
