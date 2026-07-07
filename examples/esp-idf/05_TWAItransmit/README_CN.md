# TWAI/CAN 发送测试

[English](README.md)

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
