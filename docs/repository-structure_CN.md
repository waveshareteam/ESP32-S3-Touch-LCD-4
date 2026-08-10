# 仓库结构

[English](repository-structure.md)

- `examples/esp-idf/`：第一方 ESP-IDF 项目。
- `examples/arduino/`：第一方 Arduino 示例及随附库。
- `config/`：共享配置片段。
- `docs/`：CI、组件、固件和兼容性说明。
- `firmware/`：已记录但不由 CI 构建的出厂/恢复二进制文件。
- `releases/`：固件打包脚本。
- `hardware/`：V4.0 硬件资料和公开原理图。

CI 仅构建第一方示例；随附 Arduino 库中的示例和测试不是产品 CI 目标。
