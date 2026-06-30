# TWAI/CAN Transmit Test / TWAI-CAN 发送测试

[中文](#中文) | [English](#english)

## 中文

本示例用于验证 ESP32-S3-Touch-LCD-4 的 TWAI/CAN 发送路径。程序初始化 ESP-IDF TWAI 驱动后，每 1 秒发送一个标准 CAN 数据帧，ID 为 `0x123`，数据为 `11 22 33 44 55 66 77 88`。

### 硬件连接

| 信号 | GPIO | 说明 |
| --- | --- | --- |
| CAN TX | `GPIO6` | TWAI 发送脚，定义在 `components/can/can.h` |
| CAN RX | `GPIO0` | TWAI 接收脚，定义在 `components/can/can.h` |
| CAN bitrate | 500 kbit/s | `TWAI_TIMING_CONFIG_500KBITS()` |
| 帧格式 | 标准数据帧 | ID `0x123`，DLC 8 |

请将 CAN_H/CAN_L 接入同一 CAN 总线，并确保总线两端有合适的终端电阻。可以用 CAN 分析仪接收，也可以让另一块板运行 `04_TWAIreceive`。

### 编译和运行

```bash
cd examples/esp-idf/05_TWAItransmit
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### 期望现象

每秒发送一帧：

```text
ID: 0x123
Data: 11 22 33 44 55 66 77 88
```

具体日志格式由 `components/can/can.c` 中的 alert 和发送处理决定。

### 常见问题

- 如果发送失败，确认总线上至少有一个其它节点提供 ACK。
- 如果接收端看不到帧，确认两端 bitrate 都是 500 kbit/s。
- 如果总线错误计数增加，检查 CAN_H/CAN_L、终端电阻和共地。
- 如需修改测试帧，直接编辑 `main/main.c` 中的 `message.identifier`、`data_length_code` 和 `message.data[]`。

## English

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
