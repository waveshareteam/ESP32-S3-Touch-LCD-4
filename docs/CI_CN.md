# CI

[English](CI.md)

`Build Examples` 工作流先分类改动，再在 GitHub Actions 中构建受影响的源码维护示例。`ci-status` 在 PR 与 main/master 推送中始终出现，包括仅文档改动。

- ESP-IDF 工程从 `examples/esp-idf/*/CMakeLists.txt` 发现。
- Arduino sketches 从 `examples/arduino/` 下的 `.ino` 文件发现，但排除 `examples/arduino/libraries/**`。
- `firmware/` 下的工厂/恢复固件仅用于烧录和恢复说明，不由 CI 重新构建。

Markdown 即使位于示例或随附库中也只算文档。直接示例源码只选择所属入口；`config/`、ESP-IDF 共享输入、随附 Arduino 库源码以及工作流/发现/打包输入会选择相应完整表面。未知非文档输入会保守选择两个表面；`firmware/` 改动会作为独立维护范围报告且绝不进入示例矩阵，其中二进制或归档改动还需要发布审查。

`workflow_dispatch` 支持填写 `all`、示例目录名或仓库相对路径；无匹配的选择器会被拒绝。

本文更新时配置的 CI 矩阵：

- ESP-IDF `v5.5.5` 和 `v6.0.2`，目标芯片 `esp32s3`。
- Arduino-ESP32 core `3.3.11`，FQBN 为 `esp32:esp32:esp32s3`，并启用 16 MB Flash、OPI PSRAM、USB CDC on boot 和 `app3M_fat9M_16MB` 分区。
- 使用 `examples/arduino/libraries` 中的随附 Arduino 库。

版本固定值以工作流与示例发现脚本共同为准；当前 CI 配置更新时，请同步更新这里的快照。

完整矩阵最多 33 个构建（10 个 ESP-IDF 示例 × 2 个版本，加 13 个 Arduino sketches）。每个成功构建都会上传可烧录 firmware artifact。下载 workflow run 中的 artifact zip，解压后使用板卡串口运行 `flash.sh` 或 `flash.bat`。

Artifact 打包和下载说明见 [../releases/README_CN.md](../releases/README_CN.md)。

如果某个示例需要硬件、凭据，或依赖的上游组件尚未兼容所选框架版本，请先在本文档记录排除原因，再从 CI 中排除。
