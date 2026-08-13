# 组件

[English](components.md)

ESP-IDF 示例优先使用托管组件：`waveshare/esp32_s3_touch_lcd_4` 提供板级 BSP，`waveshare/pcf85063a` 提供 RTC，`waveshare/custom_io_expander_ch32v003` 提供辅助控制器，`lvgl/lvgl` 按示例选择主版本，`espressif/esp-brookesia` 用于 Brookesia 示例。

本地 `components/can` 和 `components/apps` 仅保留板级示例胶水代码。Arduino 示例使用 `examples/arduino/libraries` 中的随附库，以使 CI 与用户构建使用相同版本。

manifest 中的精确组件固定值是当前仓库面向 ESP-IDF v5.5 和 v6 CI 的兼容组合。BSP/自定义 I/O/RTC 的固定值是维护时选定的 registry 发布版本。ESP-Brookesia `0.4.2` 有意保留而不升级至 `0.5.0`，直到两个 IDF 版本和板级硬件行为均已验证；完成该验证后再重新评估这些固定值。本说明记录当前维护策略，并不声称本次变更完成了硬件认证。
