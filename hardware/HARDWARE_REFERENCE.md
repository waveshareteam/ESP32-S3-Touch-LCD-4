# Hardware Reference

[中文](HARDWARE_REFERENCE_CN.md)

This note summarizes the ESP32-S3-Touch-LCD-4 V4.0 schematic stored at [`hardware/schematics/ESP32-S3-Touch-LCD-4 V4.0.pdf`](<schematics/ESP32-S3-Touch-LCD-4 V4.0.pdf>). Use the PDF and the physical board revision as the source of truth for electrical work.

## Board Variants

- ESP32-S3-Touch-LCD-4 populates the GT911 capacitive touch controller.
- ESP32-S3-LCD-4 shares the display and peripheral design but does not populate GT911. A successful touch-capable CI build does not prove pointer input is available on this variant.

## Confirmed V4.0 Blocks

| Block | Schematic identification | Notes |
| --- | --- | --- |
| MCU module | ESP32-S3-WROOM-1-N16R8 | 16 MB Flash and 8 MB octal PSRAM |
| Display | 4-inch 480 x 480 RGB LCD | RGB data/timing plus serial control and reset |
| Touch | GT911 interface | Present only on the Touch variant |
| Helper controller | CH32V003F4U6 | Backlight, resets, buzzer, power control, and battery ADC glue |
| RTC | PCF85063ATL | 32.768 kHz crystal and interrupt |
| CAN/TWAI | TJA1051T/3/1J | CANH/CANL protection and switchable 120 ohm termination |
| RS485 | SP3485EN | Differential interface with switchable 120 ohm termination |
| Storage | microSD | 1-bit SDMMC-connected card slot |
| External interface | VIN, GND, RS485, CAN, I2C | Check connector orientation on the physical board before wiring |

The schematic marks the DC input as 5 to 36 V. Confirm the product documentation and board revision before applying external power.

## Shared Control Bus

The board software and schematic agree on the primary shared I2C signals:

| Signal | ESP32-S3 GPIO |
| --- | --- |
| SCL | GPIO7 |
| SDA | GPIO15 |

The CH32V003 helper uses I2C address `0x24` in the maintained examples and managed component. Because display reset, touch reset, backlight, buzzer, system power, and battery measurement depend on this controller, a CH32 or shared-bus failure can appear as several unrelated peripheral failures.

## Confirmed Peripheral Pins

| Function | Signals |
| --- | --- |
| RTC | PCF85063A at `0x51` on SDA GPIO15/SCL GPIO7; `RTC_INT` routes through CH32V003 EXIO7 |
| TWAI/CAN | TX GPIO6, RX GPIO0 |
| RS485 | TX GPIO44, RX GPIO43 |
| microSD (1-bit SDMMC) | CLK GPIO2, CMD GPIO1, D0 GPIO4 |

## Hardware Review Checklist

Before changing board-facing code or documentation:

1. Identify the exact board model and revision.
2. Cross-check the schematic, managed BSP configuration, Arduino pin configuration, and affected example.
3. Review display timing/reset, touch INT/RST/I2C, CH32 control, RTC, SD, RS485, CAN/TWAI, USB, power, and any shared pins touched by the change.
4. Keep factory/recovery images separate from source-built CI artifacts.
5. Treat CI as compile and packaging validation; record separate physical-board validation when electrical behavior changes.

## Files

- Schematic: [`hardware/schematics/ESP32-S3-Touch-LCD-4 V4.0.pdf`](<schematics/ESP32-S3-Touch-LCD-4 V4.0.pdf>)
- Factory/recovery image: [`firmware/`](../firmware/)
- Managed component strategy: [`docs/components.md`](../docs/components.md)
