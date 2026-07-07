# TWAI/CAN Transmit Test

[中文](README_CN.md)

This example verifies the TWAI/CAN transmit path on ESP32-S3-Touch-LCD-4. After initializing the ESP-IDF TWAI driver, it sends one standard CAN data frame every second. The frame ID is `0x123` and the payload is `11 22 33 44 55 66 77 88`.

### Hardware Connection

| Signal | GPIO | Description |
| --- | --- | --- |
| CAN TX | `GPIO6` | TWAI TX pin, defined in `components/can/can.h` |
| CAN RX | `GPIO0` | TWAI RX pin, defined in `components/can/can.h` |
| CAN bitrate | 500 kbit/s | `TWAI_TIMING_CONFIG_500KBITS()` |
| Frame format | Standard data frame | ID `0x123`, DLC 8 |

Connect CAN_H/CAN_L to the same CAN bus and make sure proper termination is installed at the bus ends. Use a CAN analyzer as the receiver, or run `04_TWAIreceive` on another board.

### Build And Run

```bash
cd examples/esp-idf/05_TWAItransmit
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### Expected Behavior

One frame is transmitted every second:

```text
ID: 0x123
Data: 11 22 33 44 55 66 77 88
```

The exact log format depends on the alert and transmit handling in `components/can/can.c`.

### Troubleshooting

- If transmission fails, make sure at least one other CAN node is present to provide ACK.
- If the receiver does not see frames, confirm both sides use 500 kbit/s.
- If bus error counters increase, check CAN_H/CAN_L wiring, termination, and common ground.
- To change the test frame, edit `message.identifier`, `data_length_code`, and `message.data[]` in `main/main.c`.
