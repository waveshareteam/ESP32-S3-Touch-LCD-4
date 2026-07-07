# 固件产物

[English](firmware.md)

本仓库将工厂固件保留在 `Firmware/` 目录，用于烧录和恢复。该文件应视为 Waveshare 发布的二进制文件，而不是 GitHub Actions 生成的输出。

## 工厂固件

当需要将板卡恢复到出厂固件状态，或在测试示例源码前确认硬件是否正常时，可以使用工厂固件。

工厂固件不参与源码构建 CI。CI 应构建从源码维护的 ESP-IDF 示例，并编译第一方 Arduino sketches；不应把仓库中已经提交的工厂二进制重新上传为 workflow 输出。

## CI 构建

当前 CI 用于编译验证：

- `ESP-IDF examples` 使用 ESP-IDF `v5.5.4` 和 `v6.0.2` 构建 `examples/esp-idf/` 下的源码示例。
- `Arduino examples` 使用 Arduino ESP32 core `3.3.10` 和仓库随附库，编译 `examples/Arduino-v3.3.2/examples/` 下的产品 sketches。

`.gitignore` 会忽略生成的 build 目录、依赖目录和下载的发布产物。

## 后续发布打包

如果维护者后续决定从 CI 发布源码构建的固件归档，只应打包源码构建结果。每个归档应包含：

- 固件二进制。
- 记录示例路径、框架、工具链版本、目标芯片和提交的 manifest。
- 烧录参数或偏移。
- 适用于常见主机环境的简易烧录脚本。

不要把工厂固件和 CI 生成的源码构建产物混在同一个 artifact 中。
