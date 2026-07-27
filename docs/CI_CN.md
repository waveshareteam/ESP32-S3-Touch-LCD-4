# CI

[English](CI.md)

`Build Examples` 工作流会动态发现构建目标，并在 GitHub Actions 中构建从源码维护的示例。

- ESP-IDF 工程从 `examples/esp-idf/*/CMakeLists.txt` 发现。
- Arduino sketches 从 `examples/arduino/` 下的 `.ino` 文件发现，但排除 `examples/arduino/libraries/**`。
- `firmware/` 下的工厂/恢复固件仅用于烧录和恢复说明，不由 CI 重新构建。

当工作流配置的路径过滤器匹配源码、共享配置、发现脚本、工作流或发布打包变更时，CI 会发现所有第一方目标并运行完整矩阵。仅文档或仓库治理文件的变更不会触发产品构建。

`workflow_dispatch` 支持填写 `all`、示例目录名或仓库相对路径，维护者可以运行完整矩阵，也可以为诊断目的明确选择单个示例。

本文更新时配置的 CI 矩阵：

- ESP-IDF `v5.5.5` 和 `v6.0.2`，目标芯片 `esp32s3`。
- Arduino-ESP32 core `3.3.11`，FQBN 为 `esp32:esp32:esp32s3`，并启用 16 MB Flash、OPI PSRAM、USB CDC on boot 和 `app3M_fat9M_16MB` 分区。
- 使用 `examples/arduino/libraries` 中的随附 Arduino 库。

版本固定值以工作流文件为准；工作流更新版本时，请同步更新这里的快照。

每个成功的 ESP-IDF 和 Arduino 矩阵构建都会上传可烧录 firmware artifact。下载 workflow run 中的 artifact zip，解压后使用板卡串口运行 `flash.sh` 或 `flash.bat`。

Artifact 打包和下载说明见 [../releases/README.md](../releases/README.md)。

如果某个示例需要硬件、凭据，或依赖的上游组件尚未兼容所选框架版本，请先在本文档记录排除原因，再从 CI 中排除。
