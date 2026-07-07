# Continuous Integration

[中文](CI_CN.md)

This repository uses GitHub Actions to build ESP-IDF examples and compile Arduino sketches automatically when related files change. Arduino checks use the bundled libraries already stored in this repository.

### Workflows

| Workflow | Scope | Toolchain |
| --- | --- | --- |
| `ESP-IDF examples` | ESP-IDF projects under `examples/esp-idf/` | ESP-IDF `v5.5.4` and `v6.0.2` |
| `Arduino examples` | Product sketches under `examples/Arduino-v3.3.2/examples/` | Arduino CLI `1.5.0` and ESP32 Arduino core `3.3.10` |

They run when:

- A pull request changes files under `examples/esp-idf/`.
- A pull request changes files under `examples/Arduino-v3.3.2/`.
- A pull request changes the English or Chinese root README, example index, ESP-IDF example index, or CI document.
- A pull request changes `.github/workflows/esp-idf-examples.yml`, `.github/workflows/arduino-examples.yml`, `.github/scripts/discover_esp_idf_examples.py`, or `.github/scripts/discover_arduino_examples.py`.
- A push to `main` touches the same paths.
- A workflow is started manually from the GitHub Actions page.

### Change Discovery

`ESP-IDF examples` scans first-level directories under `examples/esp-idf/`. A directory is considered buildable when it contains both:

- `CMakeLists.txt`
- `main/`

`Arduino examples` scans first-level directories under `examples/Arduino-v3.3.2/examples/`. A directory is considered compilable when it contains an `.ino` file. Changes under `examples/Arduino-v3.3.2/libraries/` select all Arduino sketches because those bundled libraries are shared.

For pull requests and pushes, only changed examples are built by default. If a global document, workflow, or discovery script changes, the affected workflow builds all examples. If no specific changed example is detected, PR/push runs fall back to all examples to avoid missing coverage.

### Manual Runs

Manual ESP-IDF runs accept one `example` input:

| Value | Meaning |
| --- | --- |
| `all` | Build all ESP-IDF examples |
| `02_SD_Test` | Build the example by directory name |
| `examples/esp-idf/09_BatteryVoltage_LVGL` | Build the example by full path |

Manual Arduino runs accept one `sketch` input:

| Value | Meaning |
| --- | --- |
| `all` | Compile all product Arduino sketches |
| `01_HelloWorld` | Compile the sketch by directory name |
| `examples/Arduino-v3.3.2/examples/14_LVGL_BatteryVoltage` | Compile the sketch by full path |

### Build Matrix

The ESP-IDF workflow builds:

| Item | Value |
| --- | --- |
| Target | `esp32s3` |
| ESP-IDF | `v5.5.4`, `v6.0.2` |
| CI Action | `espressif/esp-idf-ci-action@v1` |

The Arduino workflow compiles with:

| Item | Value |
| --- | --- |
| Arduino CLI | `1.5.0` |
| ESP32 Arduino core | `esp32:esp32@3.3.10` |
| FQBN | `esp32:esp32:esp32s3` |
| Board options | `USBMode=hwcdc`, `CDCOnBoot=cdc`, `FlashSize=16M`, `PSRAM=opi`, `PartitionScheme=app3M_fat9M_16MB` |
| Libraries | `examples/Arduino-v3.3.2/libraries` |

### Commit Notes

Do not commit local generated files unless they are intentionally curated example configuration files:

- `build/`
- `managed_components/`
- `dependencies.lock`
- locally generated `sdkconfig`
- `sdkconfig.old`

Use `sdkconfig.defaults` for stable example defaults that should be shared with CI and customers.
