# Continuous Integration

[中文](CI_CN.md)

This repository uses GitHub Actions to build ESP-IDF examples automatically. Arduino sketches and bundled libraries are intentionally not built by this workflow.

### Workflow

Workflow name: `ESP-IDF examples`

It runs when:

- A pull request changes files under `examples/esp-idf/`.
- A pull request changes the English or Chinese root README, example index, ESP-IDF example index, or CI document.
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
