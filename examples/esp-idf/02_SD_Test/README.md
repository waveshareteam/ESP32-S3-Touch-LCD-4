# SD Card Test / SD 卡测试

[中文](#中文) | [English](#english)

## 中文

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

## English

This example verifies the SD card interface on ESP32-S3-Touch-LCD-4. It initializes CH32V003 first, releases the onboard power/reset controls, mounts the SD card in SDMMC 1-bit mode, writes a file, renames it, reads it back, formats FATFS, then writes and reads another file.

### Hardware And Pins

| Function | GPIO/Signal | Description |
| --- | --- | --- |
| SD D0 | `GPIO4` | SDMMC 1-bit data line |
| SD CMD | `GPIO1` | SD command line |
| SD CLK | `GPIO2` | SD clock line |
| CH32 SDA | `GPIO15` | CH32V003 I2C SDA |
| CH32 SCL | `GPIO7` | CH32V003 I2C SCL |
| CH32 address | `0x24` | Used to release `SYS_EN`, LCD/touch reset, and related controls |

Insert an SD/SDHC/SDXC card that can be formatted as FAT32. This example does not use separate CD/WP pins.

### CH32 Initialization

At startup, the example recovers the I2C bus, initializes CH32V003, and sets the following outputs to a valid state:

- `EXIO1` / `TP_RST`
- `EXIO3` / `LCD_RST`
- `EXIO5` / `SYS_EN`
- `EXIO6` / `BEE_EN`

This helps avoid unstable startup after quick resets when the I2C bus or CH32 state has not fully reset.

### Build And Run

```bash
cd examples/esp-idf/02_SD_Test
idf.py set-target esp32s3
idf.py build
idf.py -p PORT flash monitor
```

To allow formatting when mount fails, run `idf.py menuconfig` and enable the format option under `SD SPI Example Configuration`. Note that the later part of this example intentionally formats FATFS to verify the format flow.

### Expected Log

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

### Troubleshooting

- If mounting fails, check card contact first and prefer a FAT32 card.
- If initialization fails, confirm the CH32 I2C pins are `GPIO15`/`GPIO7` and no other code owns the same I2C bus.
- If the hardware is modified or an external SD breakout is used, lower the SDMMC clock and check pull-up resistors.
- This example formats the SD card. Do not use a card that contains important data.
