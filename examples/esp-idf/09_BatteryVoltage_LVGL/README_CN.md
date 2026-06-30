# 电池电压 LVGL 示例

[English](README.md)

本示例演示如何通过 CH32V003 读取板载电池分压 ADC，并把换算后的电池电压显示到 LVGL 界面上。它适合客户在自己的 UI 工程中参考电池电压采样、平均滤波和 LVGL 标签更新方式。

### 原理

ESP32-S3 通过 I2C 访问 CH32V003，CH32 ADC 读取电池分压后的电压。根据原理图中的分压比例，示例使用以下公式换算电池电压：

```c
voltage = raw * 3.3f / 1023.0f * 3.0f;
```

| 项目 | 值 |
| --- | --- |
| CH32 I2C SDA | `GPIO15` |
| CH32 I2C SCL | `GPIO7` |
| CH32 I2C 地址 | `0x24` |
| ADC 原始范围 | 0 到 1023 |
| ADC 参考电压 | 3.3 V |
| 分压比例 | 3.0 |
| LVGL 版本 | `9.5.0` |

### 示例流程

1. 启动前恢复 I2C 总线，避免快速复位后总线卡住。
2. 初始化 Waveshare BSP 显示和触摸。
3. 获取 BSP 初始化好的 CH32 IO expander 句柄。
4. 创建居中的 LVGL label。
5. 后台任务每 2 秒读取 8 次 CH32 ADC，取平均值并换算电压。
6. 使用 `bsp_display_lock()`/`bsp_display_unlock()` 安全更新 LVGL 标签。

### 编译和运行

```bash
cd examples/esp-idf/09_BatteryVoltage_LVGL
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### 期望现象

屏幕中央显示类似内容：

```text
Battery
4.12 V
ADC 426
```

串口会同步打印 ADC 和电压日志。实际数值会随供电方式、电池状态和硬件差异变化。

### 移植到客户工程

客户工程中通常只需要复用以下步骤：

- 在显示或 CH32 初始化前执行 I2C 总线恢复。
- 使用 `bsp_io_expander_init()` 或 BSP 显示流程获得 `esp_io_expander_handle_t`。
- 调用 `custom_io_expander_get_adc()` 读取 ADC。
- 使用原理图对应的分压比例换算电压。
- 更新 LVGL UI 时使用 BSP/LVGL 的锁机制。

### 常见问题

- 如果读取 CH32 ADC 失败，先运行 [../ioexpander](../ioexpander/README_CN.md) 验证 CH32 是否可访问。
- 如果电压偏差大，确认硬件分压比例是否仍为 `3.0`，并检查 ADC 参考电压假设。
- 如果 UI 偶发崩溃，确认所有 LVGL 对象更新都在 `bsp_display_lock()` 和 `bsp_display_unlock()` 之间执行。
