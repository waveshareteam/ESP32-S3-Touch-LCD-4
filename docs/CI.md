# Continuous Integration / 持续集成

[English](#english) | [中文](#中文)

## English

This repository uses GitHub Actions to build ESP-IDF examples automatically. Arduino sketches and bundled libraries are intentionally not built by this workflow.

### Workflow

Workflow name: `ESP-IDF examples`

It runs when:

- A pull request changes files under `examples/esp-idf/`.
- A pull request changes the root `README.md`, `examples/README.md`, `examples/esp-idf/README.md`, or this file.
- A pull request changes `.github/workflows/esp-idf-examples.yml` or `.github/scripts/discover_esp_idf_examples.py`.
- A push to `main` touches the same paths.
- The workflow is started manually from the GitHub Actions page.

### Example Discovery

The discovery script scans first-level directories under `examples/esp-idf/`. A directory is considered buildable when it contains both:

- `CMakeLists.txt`
- `main/`

For pull requests and pushes, only changed examples are built by default. If a global document, the workflow, or the discovery script changes, all ESP-IDF examples are built. If no specific changed example is detected, PR/push runs fall back to all examples to avoid missing coverage.

### Manual Runs

Manual runs accept one `example` input:

| Value | Meaning |
| --- | --- |
| `all` | Build all ESP-IDF examples |
| `02_SD_Test` | Build the example by directory name |
| `examples/esp-idf/09_BatteryVoltage_LVGL` | Build the example by full path |

### Build Matrix

The current workflow builds:

| Item | Value |
| --- | --- |
| Target | `esp32s3` |
| ESP-IDF | `v5.5.4`, `v6.0.2` |
| CI Action | `espressif/esp-idf-ci-action@v1` |

### Commit Notes

Do not commit local generated files unless they are intentionally curated example configuration files:

- `build/`
- `managed_components/`
- `dependencies.lock`
- locally generated `sdkconfig`
- `sdkconfig.old`

Use `sdkconfig.defaults` for stable example defaults that should be shared with CI and customers.

## 中文

本仓库使用 GitHub Actions 对 ESP-IDF 示例进行自动构建检查。Arduino 示例和随附库不会被该工作流构建。

### 工作流

工作流名称：`ESP-IDF examples`

触发条件：

- Pull request 改动 `examples/esp-idf/` 下的文件。
- Pull request 改动根目录 `README.md`、`examples/README.md`、`examples/esp-idf/README.md` 或本文件。
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
