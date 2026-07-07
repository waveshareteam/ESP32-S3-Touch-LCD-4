# RS485 串口回显测试

[English](README.md)

本示例用于验证 ESP32-S3-Touch-LCD-4 的 RS485/UART 数据通路。程序使用 UART1，收到外部串口发送的数据后立即回传，并在调试串口打印收到的字符串。

### 硬件连接

| 信号 | 默认 GPIO | 说明 |
| --- | --- | --- |
| UART TXD | `GPIO44` | 连接到外部 RS485/串口接收端 |
| UART RXD | `GPIO43` | 连接到外部 RS485/串口发送端 |
| GND | `GND` | 与外部设备共地 |
| 电平 | 3.3 V | 外部串口必须兼容 3.3 V 电平 |

默认引脚来自 `main/Kconfig.projbuild`。如果客户硬件连接不同，可运行 `idf.py menuconfig`，在 `Echo Example Configuration` 中修改 UART TXD/RXD。

### 编译和运行

```bash
cd examples/esp-idf/01_RS485_Test
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

### 期望现象

1. 打开连接到 RS485/UART 端口的外部串口工具。
2. 发送任意文本，例如 `hello`。
3. 外部串口工具会收到相同内容的回显。
4. ESP-IDF monitor 会打印类似日志：

```text
I (...) UART TEST: Recv str: hello
```

### 常见问题

- 如果没有回显，先确认 TX/RX 是否交叉连接，并确认外部设备是 3.3 V 电平。
- 如果 monitor 没有日志但外部串口有回显，说明 UART 数据通路正常，可能只是发送内容没有可打印字符。
- 如果使用的是板载 RS485 收发器，请确认外部 A/B 线连接、终端电阻和总线方向控制是否符合产品硬件设计。
