# Factory Firmware

[English](README.md) | [简体中文](README_CN.md) | [简体中文（规范版）](README_ZH.md)

This directory stores tested factory recovery images for the ESP32-S3-LCD-4 family. Each file is a single merged BIN flashed from `0x0`.

| File | Purpose |
| --- | --- |
| [ESP32-S3-LCD-4_FactoryOnly_20251218.bin](ESP32-S3-LCD-4_FactoryOnly_20251218.bin) | Factory recovery firmware for the LCD-only ESP32-S3-LCD-4 board, dated 2025-12-18. |
| [ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin](ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin) | Factory recovery firmware for the touch-equipped ESP32-S3-Touch-LCD-4 board, dated 2025-11-22. |

Before flashing:

- Confirm the board model and hardware revision. Use the LCD-only image only on ESP32-S3-LCD-4 and the touch image only on ESP32-S3-Touch-LCD-4.
- Back up any device-specific data that must be preserved, then erase the target device.
- Install esptool with `python -m pip install esptool` and flash the matching image from `0x0`:

  ```bash
  esptool.py --chip esp32s3 --port <PORT> erase_flash
  esptool.py --chip esp32s3 --port <PORT> write_flash 0x0 <FactoryOnly-image>.bin
  ```

SHA-256:

- `ESP32-S3-LCD-4_FactoryOnly_20251218.bin`: `7AF2CA6B44D970C65DABEF06E05659596EED9156873526893B0617CF0796BA76`
- `ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin`: `5DB7EE79F1063465D8114CEA3A4675C8C87BDF303C2A3EDAF6A55414C8634706`

Factory firmware source is not included in this repository. Source-built example firmware is published separately through the CI workflow.

