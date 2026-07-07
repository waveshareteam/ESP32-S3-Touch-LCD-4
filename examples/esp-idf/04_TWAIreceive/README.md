# TWAI/CAN Receive Test

[中文](README_CN.md)

This example verifies the TWAI/CAN receive path on ESP32-S3-Touch-LCD-4. It initializes the ESP-IDF TWAI driver and monitors bus alerts. When a CAN data frame is received, the example reads it and transmits it back, which is useful for loopback or two-board testing.

### Hardware Connection

| Signal | GPIO | Description |
| --- | --- | --- |
| CAN TX | `GPIO6` | TWAI TX pin, defined in `components/can/can.h` |
| CAN RX | `GPIO0` | TWAI RX pin, defined in `components/can/can.h` |
| CAN bitrate | 500 kbit/s | `TWAI_TIMING_CONFIG_500KBITS()` |
| Mode | Normal | Requires a CAN transceiver and a valid bus |

Connect CAN_H/CAN_L to the same CAN bus and make sure proper termination is installed at the bus ends. For two-board testing, run `05_TWAItransmit` on one board and this example on the other.

### Build And Run

```bash
cd examples/esp-idf/04_TWAIreceive
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### Expected Behavior

- The TWAI driver is installed and started after boot.
- When data frames are present on the bus, the example reads and echoes them.
- Serial logs show CAN alerts, frame ID, and data bytes.

### Troubleshooting

- If no frame is received, check TX/RX pins, CAN_H/CAN_L wiring, termination, and matching bitrate on both ends.
- Normal mode requires a real CAN transceiver and ACK from another node; transmission can fail if the board is alone on the bus.
- If bus errors appear, first check whether CAN_H/CAN_L are swapped and whether the bus ground is shared.
