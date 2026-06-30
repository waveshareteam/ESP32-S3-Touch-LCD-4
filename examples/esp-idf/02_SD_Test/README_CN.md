# SD 卡测试

[English](README.md)

本示例用于验证 ESP32-S3-Touch-LCD-4 的 SD 卡接口。程序会初始化 CH32V003，释放板载电源/复位相关控制，然后通过 SDMMC 1-bit 模式挂载 SD 卡，写入文件、重命名、读取文件、格式化 FATFS，再重新写入并读取一个文件。

### 硬件和引脚

| 功能 | GPIO/信号 | 说明 |
| --- | --- | --- |
| SD D0 | `GPIO4` | SDMMC 1-bit 数据线 |
| SD CMD | `GPIO1` | SD 命令线 |
| SD CLK | `GPIO2` | SD 时钟线 |
| CH32 SDA | `GPIO15` | CH32V003 I2C SDA |
| CH32 SCL | `GPIO7` | CH32V003 I2C SCL |
| CH32 地址 | `0x24` | 用于释放 `SYS_EN`、LCD/触摸复位等 |

请插入 FAT32 可识别的 SD/SDHC/SDXC 卡。示例未使用独立的 CD/WP 引脚。

### CH32 初始化

示例启动时会先执行 I2C 总线恢复，再初始化 CH32V003，并将以下输出配置为有效状态：

- `EXIO1` / `TP_RST`
- `EXIO3` / `LCD_RST`
- `EXIO5` / `SYS_EN`
- `EXIO6` / `BEE_EN`

这样可以避免快速复位后 I2C 总线或 CH32 状态异常导致 SD 示例启动不稳定。

### 编译和运行

```bash
cd examples/esp-idf/02_SD_Test
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

如需在挂载失败时自动格式化，可运行 `idf.py menuconfig`，启用 `SD SPI Example Configuration` 中的格式化选项。注意本示例后半段会主动执行一次 FATFS 格式化，用于验证格式化流程。

### 期望日志

```text
Initializing SD card
Using SDMMC peripheral
Mounting filesystem
Filesystem mounted
Opening file /sdcard/hello.txt
File written
Reading file /sdcard/foo.txt
Read from file: 'Hello ...!'
file doesnt exist, format done
Opening file /sdcard/nihao.txt
Read from file: 'Nihao ...!'
Card unmounted
```

### 常见问题

- 挂载失败时，确认 SD 卡接触良好，并优先使用 FAT32 格式。
- 初始化失败时，确认 CH32 I2C 引脚是 `GPIO15`/`GPIO7`，且没有其它代码占用同一 I2C 总线。
- 如果客户修改硬件或使用飞线 SD 模块，可能需要降低 SDMMC 时钟并检查上拉电阻。
- 本示例会格式化 SD 卡，请不要使用保存重要数据的卡。
