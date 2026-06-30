# SD Card Test

[中文](README_CN.md)

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
