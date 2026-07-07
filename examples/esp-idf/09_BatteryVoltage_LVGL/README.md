# Battery Voltage LVGL Example

[中文](README_CN.md)

This example shows how to read the onboard battery-divider ADC through CH32V003 and display the converted battery voltage on an LVGL screen. It is intended as a reference for battery-voltage sampling, averaging, and LVGL label updates in customer UI projects.

The application does not require touch input, but its ESP-IDF display startup uses the shared BSP for the Touch board family. On ESP32-S3-LCD-4, use it as display/peripheral firmware only after confirming the BSP startup path tolerates the missing GT911 touch controller.

### Principle

ESP32-S3 accesses CH32V003 over I2C, and the CH32 ADC samples the divided battery voltage. According to the schematic divider ratio, the example converts raw ADC values with:

```c
voltage = raw * 3.3f / 1023.0f * 3.0f;
```

| Item | Value |
| --- | --- |
| CH32 I2C SDA | `GPIO15` |
| CH32 I2C SCL | `GPIO7` |
| CH32 I2C address | `0x24` |
| ADC raw range | 0 to 1023 |
| ADC reference voltage | 3.3 V |
| Divider ratio | 3.0 |
| LVGL version | `9.5.0` |

### Example Flow

1. Recover the I2C bus before startup to avoid a stuck bus after quick resets.
2. Initialize the Waveshare BSP display path.
3. Get the CH32 IO expander handle initialized by the BSP.
4. Create a centered LVGL label.
5. Every 2 seconds, a background task reads CH32 ADC 8 times, averages the result, and converts it to voltage.
6. Update the LVGL label safely with `bsp_display_lock()`/`bsp_display_unlock()`.

### Build And Run

```bash
cd examples/esp-idf/09_BatteryVoltage_LVGL
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### Expected Behavior

The center of the screen shows text similar to:

```text
Battery
4.12 V
ADC 426
```

The serial monitor also prints ADC and voltage logs. Actual values depend on power source, battery state, and hardware tolerance.

### Porting To Customer Projects

A customer project usually needs these steps:

- Recover the I2C bus before display or CH32 initialization.
- Obtain `esp_io_expander_handle_t` through `bsp_io_expander_init()` or the BSP display flow.
- Call `custom_io_expander_get_adc()` to read ADC.
- Convert voltage with the divider ratio from the schematic.
- Use the BSP/LVGL lock mechanism when updating LVGL UI objects.

### Troubleshooting

- If CH32 ADC reads fail, run [../ioexpander](../ioexpander/) first to verify CH32 access.
- If voltage has a large offset, confirm the hardware divider ratio is still `3.0` and check the ADC reference voltage assumption.
- If UI crashes occasionally, make sure all LVGL object updates happen between `bsp_display_lock()` and `bsp_display_unlock()`.
