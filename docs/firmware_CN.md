# 固件产物

[English](firmware.md)

`Firmware/` 保存面向用户烧录和恢复流程的工厂二进制文件。这些文件是 Waveshare 发布产物，不是源码工程，也不会由 CI 重新构建。

从源码维护的固件应放在 `examples/esp-idf/`，或另一个有明确说明的源码目录，并包含自己的 `CMakeLists.txt`、组件 manifest 和验证路径。

CI 构建输出由 `releases/package_firmware.py` 打包并上传为 workflow artifacts。烧录到 ESP32-S3-LCD-4 前，请优先选择显示/外设示例，或确认固件路径能够容忍缺少 GT911 触摸输入。每个 zip 包含 `manifest.json`、`flash.sh`、`flash.bat`、`flash_args.txt` 以及 esptool 烧录所需的二进制文件。

本地发布打包时，请先构建目标工程，再从仓库根目录运行 Python 脚本。生成的归档默认写入 `releases/dist/`。

不要把工厂固件和 CI 生成的源码构建产物混在同一个 artifact 中。
