# RS485 UART Echo Test

[中文](README_CN.md)

This example verifies the RS485/UART data path on ESP32-S3-Touch-LCD-4. It uses UART1, echoes any received data back to the external sender, and prints the received string to the debug monitor.

### Hardware Connection

| Signal | Default GPIO | Description |
| --- | --- | --- |
| UART TXD | `GPIO44` | Connect to the external RS485/serial RX side |
| UART RXD | `GPIO43` | Connect to the external RS485/serial TX side |
| GND | `GND` | Common ground with the external device |
| Level | 3.3 V | The external serial interface must be 3.3 V compatible |

The default pins come from `main/Kconfig.projbuild`. If your wiring is different, run `idf.py menuconfig` and change UART TXD/RXD under `Echo Example Configuration`.

### Build And Run

```bash
cd examples/esp-idf/01_RS485_Test
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### Expected Behavior

1. Open a serial tool connected to the RS485/UART port.
2. Send text such as `hello`.
3. The external serial tool receives the same text as an echo.
4. ESP-IDF monitor prints a log similar to:

```text
I (...) UART TEST: Recv str: hello
```

### Troubleshooting

- If there is no echo, check that TX/RX are crossed and that the external device uses 3.3 V logic.
- If there is echo but no monitor log, the UART path is working; the transmitted data may simply not be printable text.
- If using the onboard RS485 transceiver, verify A/B wiring, termination, and bus direction control according to the product hardware design.
