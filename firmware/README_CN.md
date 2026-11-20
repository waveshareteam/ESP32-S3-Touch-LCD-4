[English](README.md) | [简体中文](README_CN.md) | [简体中文（规范版）](README_ZH.md)

# 出厂固件

[English](README.md) | [简体中文](README_ZH.md)

本目录保存 ESP32-S3-Touch-LCD-4 开发板的实测出厂镜像。

| 文件 | 用途 |
| --- | --- |
| [ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin](ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin) | ESP32-S3-Touch-LCD-4 出厂恢复固件，日期为 20251122，从 `0x0` 地址开始烧录。 |

烧录前：

- 备份需要保留的设备专用数据。
- 确认开发板型号和硬件版本。
- 烧录合并镜像前先整片擦除目标设备。
- 按产品文档中的烧录流程操作。

SHA-256：`5DB7EE79F1063465D8114CEA3A4675C8C87BDF303C2A3EDAF6A55414C8634706`

该文件是用于恢复出厂功能的单个合并 BIN。

