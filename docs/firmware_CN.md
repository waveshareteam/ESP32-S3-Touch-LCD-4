# 固件产物

[English](firmware.md)

`firmware/` 保存面向用户烧录和恢复流程的工厂/恢复二进制文件。这些文件是 Waveshare 发布产物，不是源码工程，也不会由 CI 重新构建。

不可变工厂固件清单位于 [`firmware/artifacts.json`](../firmware/artifacts.json)。请在仓库根目录以只读方式运行 `python3 scripts/verify_firmware_artifacts.py --repo . --manifest firmware/artifacts.json` 校验工作树文件；CI 会增加 `--index` 以校验精确的已检入 blob。该 manifest 仅记录仓库相对路径、SHA-256 摘要、字节大小和产物类型；它不会把工厂固件加入示例矩阵，也不会仅凭文件名后缀推断归档用途。只有在第一方证据于 manifest 中明确分类后，归档才视为交付产物。

从源码维护的固件应放在 `examples/esp-idf/`，或另一个有明确说明的源码目录，并包含自己的 `CMakeLists.txt`、组件 manifest 和验证路径。

CI 构建输出由 `releases/package_firmware.py` 打包并上传为 workflow artifacts。烧录到 ESP32-S3-LCD-4 前，请优先选择显示/外设示例，或确认固件路径能够容忍缺少 GT911 触摸输入。每个 zip 包含 `manifest.json`、`flash.sh`、`flash.bat`、`flash_args.txt` 以及 esptool 烧录所需的二进制文件。

`manifest.json` 会记录仓库相对路径 `project_path`、UTC 时间 `timestamp_utc`、框架版本、Git 提交、目标芯片、烧录命令和文件偏移。为兼容旧版归档的使用方，仍保留 `project` 和 `generated_at` 字段。

本地发布打包时，请先构建目标工程，再从仓库根目录运行 Python 脚本。生成的归档默认写入 `releases/dist/`。

不要把工厂/恢复固件和 CI 生成的源码构建产物混在同一个 artifact 中，也不要把两者作为同一种构建结果报告。
