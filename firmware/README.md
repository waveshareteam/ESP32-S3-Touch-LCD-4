# Factory Firmware

[English](README.md) | [简体中文](README_CN.md) | [简体中文（规范版）](README_ZH.md)

This directory stores the tested factory image for the ESP32-S3-Touch-LCD-4 development board.

| File | Purpose |
| --- | --- |
| [ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin](ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin) | Factory recovery firmware for the ESP32-S3-Touch-LCD-4 board, dated 20251122. Flash from `0x0`. |

Before flashing:

- Back up any device-specific data that must be preserved.
- Confirm the board model and hardware revision.
- Erase the target device before writing the merged image.
- Follow the flashing procedure in the product documentation.

SHA-256: `5DB7EE79F1063465D8114CEA3A4675C8C87BDF303C2A3EDAF6A55414C8634706`

This file is a single merged BIN for factory recovery.

