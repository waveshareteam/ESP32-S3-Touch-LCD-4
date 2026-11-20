[English](README.md) | [简体中文](README_CN.md) | [简体中文（规范版）](README_ZH.md)

# 出厂固件

本目录保存 ESP32-S3-LCD-4 系列已完成实物验证的出厂恢复镜像。每个文件都是从 `0x0` 地址烧录的单个合并 BIN。

| 文件 | 用途 |
| --- | --- |
| [ESP32-S3-LCD-4_FactoryOnly_20251218.bin](ESP32-S3-LCD-4_FactoryOnly_20251218.bin) | 不带触摸的 ESP32-S3-LCD-4 出厂恢复固件，日期为 2025-12-18。 |
| [ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin](ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin) | 带触摸的 ESP32-S3-Touch-LCD-4 出厂恢复固件，日期为 2025-11-22。 |

烧录前：

- 确认开发板型号和硬件版本。ESP32-S3-LCD-4 只能使用不带触摸的镜像，ESP32-S3-Touch-LCD-4 只能使用带触摸的镜像。
- 备份必须保留的设备专用数据，然后整片擦除目标设备。
- 使用 `python -m pip install esptool` 安装 esptool，并将对应镜像从 `0x0` 地址烧录：

  ```bash
  esptool.py --chip esp32s3 --port <PORT> erase_flash
  esptool.py --chip esp32s3 --port <PORT> write_flash 0x0 <FactoryOnly-image>.bin
  ```

SHA-256：

- `ESP32-S3-LCD-4_FactoryOnly_20251218.bin`：`7AF2CA6B44D970C65DABEF06E05659596EED9156873526893B0617CF0796BA76`
- `ESP32-S3-Touch-LCD-4_FactoryOnly_20251122.bin`：`5DB7EE79F1063465D8114CEA3A4675C8C87BDF303C2A3EDAF6A55414C8634706`

出厂固件源码暂未包含在本仓库中。源码示例固件通过 CI 工作流单独发布。

