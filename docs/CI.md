# Continuous Integration

This repository uses GitHub Actions to build ESP-IDF examples when they are
added or changed. Arduino sketches are intentionally ignored by this workflow.

## ESP-IDF Example Builds

The `ESP-IDF examples` workflow runs on:

- Pull requests that change files under `examples/ESP-IDF-v5.3.1/`.
- Pull requests that change the ESP-IDF example index or CI documentation.
- Pull requests that change the workflow or discovery script.
- Pushes to `main` that touch the same paths.
- Manual runs from the GitHub Actions page.

The workflow discovers buildable examples by looking for directories under
`examples/ESP-IDF-v5.3.1/` that contain both:

- `CMakeLists.txt`
- `main/`

For pull requests and pushes, only changed examples are built. If a repository
level ESP-IDF document, the workflow, or the discovery script changes, the
workflow builds all ESP-IDF examples.

Manual runs accept one input:

| Input | Value |
| --- | --- |
| `example` | `all`, a directory name such as `02_SD_Test`, or a full path such as `examples/ESP-IDF-v5.3.1/09_BatteryVoltage_LVGL` |

The workflow currently builds with:

- ESP-IDF Docker image `espressif/idf:v5.3.1`.
- Target: `esp32s3`.

Generated build artifacts, `managed_components/`, dependency lock files, and
local `sdkconfig` files should stay out of commits unless they are intentionally
curated.
