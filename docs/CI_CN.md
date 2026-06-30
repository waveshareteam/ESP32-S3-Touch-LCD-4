# 持续集成

[English](CI.md)

本仓库使用 GitHub Actions 对 ESP-IDF 示例进行自动构建检查。Arduino 示例和随附库不会被该工作流构建。

### 工作流

工作流名称：`ESP-IDF examples`

触发条件：

- Pull request 改动 `examples/esp-idf/` 下的文件。
- Pull request 改动根目录 README、示例索引、ESP-IDF 示例索引或 CI 文档的英文/中文版本。
- Pull request 改动 `.github/workflows/esp-idf-examples.yml` 或 `.github/scripts/discover_esp_idf_examples.py`。
- 推送到 `main` 且改动上述路径。
- 在 GitHub Actions 页面手动运行。

### 示例发现规则

发现脚本会扫描 `examples/esp-idf/` 下的一级目录。一个目录同时满足以下条件时，会被认为是可构建 ESP-IDF 示例：

- 包含 `CMakeLists.txt`
- 包含 `main/` 目录

PR 和 push 默认只构建有改动的示例。如果改动了全局文档、工作流或发现脚本，则构建全部 ESP-IDF 示例。若没有识别到具体示例，PR/push 会回退到全部构建，避免漏检。

### 手动运行

手动运行时可填写 `example` 输入：

| 输入值 | 含义 |
| --- | --- |
| `all` | 构建所有 ESP-IDF 示例 |
| `02_SD_Test` | 构建指定目录名对应的示例 |
| `examples/esp-idf/09_BatteryVoltage_LVGL` | 构建指定完整路径对应的示例 |

### 构建矩阵

当前工作流构建：

| 项目 | 值 |
| --- | --- |
| Target | `esp32s3` |
| ESP-IDF | `v5.5.4`、`v6.0.2` |
| CI Action | `espressif/esp-idf-ci-action@v1` |

### 提交注意事项

请不要提交本地生成文件，除非这些文件是有意维护的示例配置：

- `build/`
- `managed_components/`
- `dependencies.lock`
- 本地生成的 `sdkconfig`
- `sdkconfig.old`

示例推荐提交 `sdkconfig.defaults`，让 CI 和客户本地构建使用稳定默认值。
