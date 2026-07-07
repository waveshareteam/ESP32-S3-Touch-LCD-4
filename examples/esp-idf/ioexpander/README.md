# CH32V003 IO Expander Test And Guide

[中文](README_CN.md)

This example is a standalone ESP-IDF test program for the CH32V003 IO expander on ESP32-S3-Touch-LCD-4. It is written for customers who are using this board for the first time and want to understand what CH32 controls, how to initialize it, how to control the backlight, how to reset the display/touch panel, and how to read battery voltage.

This example does not start LVGL and does not initialize the full display driver. It only tests CH32 and prints each operation to the serial monitor. When customers see a dark screen, uncontrollable backlight, abnormal peripherals after quick reset, or I2C initialization failures, this example is a good first diagnostic step.

### What CH32 Does On This Board

ESP32-S3-Touch-LCD-4 uses CH32V003 as an onboard IO expander/helper controller. ESP32-S3 writes CH32 registers over I2C to indirectly control several board functions:

- LCD backlight PWM
- LCD reset
- Touch reset
- System power enable
- Buzzer enable
- RTC interrupt input readback
- Battery-voltage ADC readback

These functions should not all be treated as normal ESP32 GPIOs. For example, the backlight is not an ESP32-S3 LEDC pin; it is controlled by a CH32 PWM register.

### Hardware Connection

The ESP32-S3 to CH32V003 interface is:

| Item | Value |
| --- | --- |
| I2C port | `I2C_NUM_0` |
| SDA | `GPIO15` |
| SCL | `GPIO7` |
| CH32 I2C address | `0x24` |
| I2C speed | 400 kHz |

This example uses the following ESP-IDF component:

```yaml
dependencies:
  waveshare/custom_io_expander_ch32v003:
    version: "*"
```

### CH32 IO Map

| CH32 signal | ESP-IDF mask | Board function | Direction | Typical level/use |
| --- | --- | --- | --- | --- |
| `EXIO1` | `IO_EXPANDER_PIN_NUM_1` | Touch reset `TP_RST` | Output | `1` releases reset, `0` holds reset |
| `EXIO3` | `IO_EXPANDER_PIN_NUM_3` | LCD reset `LCD_RST` | Output | `1` releases reset, `0` holds reset |
| `EXIO5` | `IO_EXPANDER_PIN_NUM_5` | System power enable `SYS_EN` | Output | `1` enables related power rails |
| `EXIO6` | `IO_EXPANDER_PIN_NUM_6` | Buzzer `BEE_EN` | Output | `1` on, `0` off |
| `EXIO7` | `IO_EXPANDER_PIN_NUM_7` | RTC interrupt `RTC_INT` | Input | Read only |
| `EXIO_PWM` | `custom_io_expander_set_pwm()` | LCD backlight `BL_EN` | PWM | `0` off, `255` brightest |
| `EXIO_ADC` | `custom_io_expander_get_adc()` | Battery divider ADC | ADC | Raw value 0 to 1023 |

For normal LCD operation, keep `SYS_EN`, `LCD_RST`, and `TP_RST` high. Keep `BEE_EN` low when the buzzer is not needed.

### What The Example Does

After boot, the program:

1. Recovers the I2C bus before installing the I2C driver.
2. Creates a CH32V003 IO expander object at address `0x24`.
3. Sets `EXIO1`, `EXIO3`, `EXIO5`, and `EXIO6` as outputs.
4. Sets `EXIO7` as input.
5. Drives LCD, touch, system enable, and buzzer outputs low for 200 ms.
6. Drives `SYS_EN`, `LCD_RST`, and `TP_RST` high and keeps `BEE_EN` off.
7. Prints CH32 IO state, RTC interrupt state, battery ADC, and converted voltage.
8. Beeps the buzzer once.
9. Pulses LCD and touch reset low once.
10. Changes backlight brightness through 10%, 40%, 80%, 100%, 60%, 20%, and 100%.
11. Continues printing CH32 state and battery voltage every 2 seconds.

Even without a display image, the backlight brightness should visibly change during the PWM demo.

### Build And Run

ESP-IDF v5.5.4 and v6.0.2 are supported.

```bash
cd examples/esp-idf/ioexpander
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

Replace `PORT` with the actual serial port, such as `COM8` on Windows or `/dev/ttyACM0` on Linux.

### Expected Serial Log

Battery ADC raw values change with power source and battery voltage. The key log format is:

```text
I (...) ch32_ioexpander: Enable SYS_EN, release LCD_RST and TP_RST
I (...) ch32_ioexpander: CH32 output mask=0x6A SYS_EN=1 LCD_RST=1 TP_RST=1 BEE_EN=0 RTC_INT pin=...
I (...) ch32_ioexpander: Battery ADC raw=..., voltage=... V
I (...) ch32_ioexpander: Buzzer on for 80 ms
I (...) ch32_ioexpander: Set LCD backlight to 10% (PWM duty 26/255)
I (...) ch32_ioexpander: Set LCD backlight to 100% (PWM duty 255/255)
```

If these logs appear, ESP32-S3 can access CH32 and the basic CH32 register read/write, PWM, and ADC paths are working.

### Common Code Snippets

#### 1. Initialize CH32 Safely

Customer ESP-IDF projects should use a similar flow:

```c
board_i2c_recover();
i2c_new_master_bus(&i2c_bus_conf, &i2c_bus);
custom_io_expander_new_i2c_ch32v003(i2c_bus, 0x24, &io_expander);
esp_io_expander_set_dir(io_expander, output_mask, IO_EXPANDER_OUTPUT);
esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_7, IO_EXPANDER_INPUT);
```

`board_i2c_recover()` is important. During a quick reset, ESP32-S3 resets, but CH32 or another device on the same I2C bus may not reset at the same time. If a device still holds SDA low, the next boot may fail during I2C initialization or CH32 register writes. The recovery flow manually toggles SCL and generates a STOP condition so the bus returns to idle.

#### 2. Release LCD And Touch Reset

```c
#define TP_RST   IO_EXPANDER_PIN_NUM_1
#define LCD_RST  IO_EXPANDER_PIN_NUM_3
#define SYS_EN   IO_EXPANDER_PIN_NUM_5
#define BEE_EN   IO_EXPANDER_PIN_NUM_6

esp_io_expander_set_dir(io_expander, TP_RST | LCD_RST | SYS_EN | BEE_EN,
                        IO_EXPANDER_OUTPUT);
esp_io_expander_set_level(io_expander, TP_RST | LCD_RST | SYS_EN | BEE_EN, 0);
vTaskDelay(pdMS_TO_TICKS(200));
esp_io_expander_set_level(io_expander, SYS_EN | LCD_RST | TP_RST, 1);
esp_io_expander_set_level(io_expander, BEE_EN, 0);
```

If screen or touch occasionally behaves abnormally after a quick reset, explicitly drive the reset pins low during startup and then release them as shown above.

#### 3. Control LCD Backlight

LCD backlight is controlled by a CH32 PWM register. It cannot be controlled directly with a normal ESP32 GPIO or LEDC channel. Use:

```c
custom_io_expander_set_pwm(io_expander, 0);    // backlight off
custom_io_expander_set_pwm(io_expander, 128);  // about 50% brightness
custom_io_expander_set_pwm(io_expander, 255);  // maximum brightness
```

For percentage-based control:

```c
uint8_t duty = percent * 255 / 100;
custom_io_expander_set_pwm(io_expander, duty);
```

When using the Waveshare BSP display interface, you can also call after display initialization:

```c
bsp_display_brightness_set(80);  // 80% brightness
```

The BSP writes CH32 PWM internally as well.

#### 4. Read Battery Voltage

CH32 ADC returns a raw value from 0 to 1023. With the onboard divider, battery voltage is converted as:

```c
voltage = raw * 3.3f / 1023.0f * 3.0f;
```

Basic read flow:

```c
uint16_t raw = 0;
custom_io_expander_get_adc(io_expander, &raw);
float voltage = raw * 3.3f / 1023.0f * 3.0f;
```

For products, average multiple samples to reduce ADC noise. This example's `read_battery_voltage()` already averages 8 samples.

#### 5. Control The Buzzer

```c
esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_6, 1);
vTaskDelay(pdMS_TO_TICKS(80));
esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_6, 0);
```

Always drive `BEE_EN` low after a beep, otherwise the buzzer may remain on.

#### 6. Read RTC Interrupt State

`EXIO7` is connected to `RTC_INT` and should be configured as input.

```c
uint32_t level = 0;
uint8_t int_reg = 0;

esp_io_expander_set_dir(io_expander, IO_EXPANDER_PIN_NUM_7, IO_EXPANDER_INPUT);
esp_io_expander_get_level(io_expander, IO_EXPANDER_PIN_NUM_7, &level);
custom_io_expander_get_int(io_expander, &int_reg);
```

### If Your Project Already Uses BSP

If the customer project already depends on the Waveshare BSP, it can obtain the CH32 handle initialized by the BSP:

```c
#include "bsp/esp-bsp.h"

esp_io_expander_handle_t io_expander = bsp_io_expander_init();
```

Display examples usually initialize CH32 and display-related power/reset controls inside `bsp_display_start()`. This standalone example shows the lower-level CH32 calls so customers can understand what the BSP does internally.

### FAQ

**CH32 is not found: `CH32V003 IO expander not found at 0x24`**

Check first:

- I2C SDA is `GPIO15`.
- I2C SCL is `GPIO7`.
- No other code already owns the same I2C port.
- I2C bus recovery runs before `i2c_new_master_bus()`.

**Backlight does not change**

The backlight on this board is not controlled by a normal ESP32 GPIO. Use `custom_io_expander_set_pwm()`, or `bsp_display_brightness_set()` in the BSP display flow.

**LCD or touch is abnormal after quick reset**

At startup, do three things:

1. Recover the I2C bus.
2. Initialize CH32.
3. Drive `LCD_RST` and `TP_RST` low for 100 to 200 ms, then release them high.

This example includes the full flow. See `ch32_init()` and `ch32_reset_lcd_and_touch()`.

**Buzzer keeps sounding**

Confirm `EXIO6` is configured as output and drive it low after the beep:

```c
esp_io_expander_set_level(io_expander, IO_EXPANDER_PIN_NUM_6, 0);
```

**Battery voltage is not as expected**

The current conversion uses a 3.3 V ADC reference and a 3.0 divider ratio. If the customer changes the hardware divider, update `BATTERY_DIVIDER_RATIO` in `main/main.c` accordingly.
