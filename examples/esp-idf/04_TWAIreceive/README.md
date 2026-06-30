# TWAI/CAN Receive Test / TWAI-CAN 接收测试

[中文](#中文) | [English](#english)

## 中文

本示例用于验证 ESP32-S3-Touch-LCD-4 的 TWAI/CAN 接收路径。程序初始化 ESP-IDF TWAI 驱动，监听总线事件；收到 CAN 数据帧后读取该帧并立即重新发送，方便客户做回环或两板互测。

### 硬件连接

| 信号 | GPIO | 说明 |
| --- | --- | --- |
| CAN TX | `GPIO6` | TWAI 发送脚，定义在 `components/can/can.h` |
| CAN RX | `GPIO0` | TWAI 接收脚，定义在 `components/can/can.h` |
| CAN bitrate | 500 kbit/s | `TWAI_TIMING_CONFIG_500KBITS()` |
| 模式 | Normal | 需要连接 CAN 收发器和有效总线 |

请将 CAN_H/CAN_L 接入同一 CAN 总线，并确保总线两端有合适的终端电阻。两块板互测时，可以一块运行 `05_TWAItransmit`，另一块运行本示例。

### 编译和运行

```bash
cd examples/esp-idf/04_TWAIreceive
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### 期望现象

- 程序启动后 TWAI 驱动安装并启动。
- 总线上有数据帧时，示例读取帧并回传。
- 串口日志会显示 CAN alert、ID 和数据内容。

### 常见问题

- 如果没有收到数据，确认 TX/RX 引脚、CAN_H/CAN_L、终端电阻和两端波特率一致。
- Normal 模式需要真实 CAN 收发器和总线 ACK；如果总线上没有其它节点，发送可能失败。
- 如果出现 bus error，优先检查 CAN_H/CAN_L 是否接反以及总线地线是否共地。

## English

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
