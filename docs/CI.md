# CI

[中文](CI_CN.md)

The `Build Examples` workflow discovers build targets dynamically and builds source-maintained examples in GitHub Actions.

- ESP-IDF projects are discovered from `examples/esp-idf/*/CMakeLists.txt`.
- Arduino sketches are discovered from `.ino` files under `examples/arduino/`, excluding `examples/arduino/libraries/**`.
- Factory/recovery firmware under `firmware/` is documented for flashing, but is not rebuilt by CI.

When a configured CI path filter matches a source, shared configuration, discovery, workflow, or release-packaging change, CI discovers every first-party target and runs the complete configured matrix. Documentation-only and governance-only changes do not trigger product builds.

`workflow_dispatch` accepts `all`, an example directory name, or a repo-relative path. This allows maintainers to run the full matrix or intentionally select one example for diagnosis.

Matrix configured at the time of this update:

- ESP-IDF `v5.5.5` and `v6.0.2`, target `esp32s3`.
- Arduino-ESP32 core `3.3.11`, FQBN `esp32:esp32:esp32s3` with 16 MB Flash, OPI PSRAM, USB CDC on boot, and `app3M_fat9M_16MB` partitioning.
- Bundled Arduino libraries from `examples/arduino/libraries`.

The workflow file is the source of truth for version pins. Update this snapshot whenever those pins change.

Each successful ESP-IDF and Arduino matrix build uploads a flashable firmware artifact. Download the artifact zip from the workflow run, extract it, then run `flash.sh` or `flash.bat` with the board serial port.

For artifact packaging and download details, see [../releases/README.md](../releases/README.md).

If an example requires hardware, credentials, or an upstream component that is not yet compatible with a selected framework version, document the exclusion here before excluding it from CI.
