# 持续集成

[English](CI.md)

本仓库使用 GitHub Actions 在相关文件改动时自动构建 ESP-IDF 示例并编译 Arduino sketches。Arduino 检查使用仓库内已经随附的库。

### 工作流

| 工作流 | 范围 | 工具链 |
| --- | --- | --- |
| `ESP-IDF examples` | `examples/esp-idf/` 下的 ESP-IDF 工程 | ESP-IDF `v5.5.4` 和 `v6.0.2` |
| `Arduino examples` | `examples/Arduino-v3.3.2/examples/` 下的产品 sketches | Arduino CLI `1.5.0` 和 ESP32 Arduino core `3.3.10` |

触发条件：

- Pull request 改动 `examples/esp-idf/` 下的文件。
- Pull request 改动 `examples/Arduino-v3.3.2/` 下的文件。
- Pull request 改动根目录 README、示例索引、ESP-IDF 示例索引或 CI 文档的英文/中文版本。
- Pull request 改动 `.github/workflows/esp-idf-examples.yml`、`.github/workflows/arduino-examples.yml`、`.github/scripts/discover_esp_idf_examples.py` 或 `.github/scripts/discover_arduino_examples.py`。
- 推送到 `main` 且改动上述路径。
- 在 GitHub Actions 页面手动运行。

### 改动发现规则

`ESP-IDF examples` 会扫描 `examples/esp-idf/` 下的一级目录。一个目录同时满足以下条件时，会被认为是可构建 ESP-IDF 示例：

- 包含 `CMakeLists.txt`
- 包含 `main/` 目录

`Arduino examples` 会扫描 `examples/Arduino-v3.3.2/examples/` 下的一级目录。目录中包含 `.ino` 文件时，会被认为是可编译 Arduino sketch。`examples/Arduino-v3.3.2/libraries/` 下的改动会选择全部 Arduino sketches，因为这些随附库被多个示例共享。

PR 和 push 默认只构建或编译有改动的示例。如果改动了全局文档、工作流或发现脚本，则对应工作流会构建全部示例。若没有识别到具体示例，PR/push 会回退到全部构建，避免漏检。

### 手动运行

手动运行 ESP-IDF 工作流时可填写 `example` 输入：

| 输入值 | 含义 |
| --- | --- |
| `all` | 构建所有 ESP-IDF 示例 |
| `02_SD_Test` | 构建指定目录名对应的示例 |
| `examples/esp-idf/09_BatteryVoltage_LVGL` | 构建指定完整路径对应的示例 |

手动运行 Arduino 工作流时可填写 `sketch` 输入：

| 输入值 | 含义 |
| --- | --- |
| `all` | 编译全部产品 Arduino sketches |
| `01_HelloWorld` | 按目录名编译指定 sketch |
| `examples/Arduino-v3.3.2/examples/14_LVGL_BatteryVoltage` | 按完整路径编译指定 sketch |

### 构建矩阵

ESP-IDF 工作流构建：

| 项目 | 值 |
| --- | --- |
| Target | `esp32s3` |
| ESP-IDF | `v5.5.4`、`v6.0.2` |
| CI Action | `espressif/esp-idf-ci-action@v1` |

Arduino 工作流编译配置：

| 项目 | 值 |
| --- | --- |
| Arduino CLI | `1.5.0` |
| ESP32 Arduino core | `esp32:esp32@3.3.10` |
| FQBN | `esp32:esp32:esp32s3` |
| 板卡选项 | `USBMode=hwcdc`、`CDCOnBoot=cdc`、`FlashSize=16M`、`PSRAM=opi`、`PartitionScheme=app3M_fat9M_16MB` |
| 库路径 | `examples/Arduino-v3.3.2/libraries` |

### 提交注意事项

请不要提交本地生成文件，除非这些文件是有意维护的示例配置：

- `build/`
- `managed_components/`
- `dependencies.lock`
- 本地生成的 `sdkconfig`
- `sdkconfig.old`

示例推荐提交 `sdkconfig.defaults`，让 CI 和客户本地构建使用稳定默认值。
