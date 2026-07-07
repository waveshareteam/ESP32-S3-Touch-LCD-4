# Firmware Artifacts

[中文](firmware_CN.md)

This repository keeps the factory firmware image under `Firmware/` for flashing and recovery. Treat that file as a released binary from Waveshare, not as a generated output from GitHub Actions.

## Factory Firmware

Use the factory image when you need to restore the board to the shipped firmware state or verify that hardware is working before testing example source code.

The factory image is intentionally excluded from source-build CI. CI should build source-maintained ESP-IDF examples and compile first-party Arduino sketches; it should not re-upload checked-in factory binaries as workflow output.

## CI Builds

Current CI is compile validation:

- `ESP-IDF examples` builds source examples under `examples/esp-idf/` with ESP-IDF `v5.5.4` and `v6.0.2`.
- `Arduino examples` compiles product sketches under `examples/Arduino-v3.3.2/examples/` with Arduino ESP32 core `3.3.10` and bundled libraries.

Generated build directories, dependency folders, and downloaded release artifacts are ignored by `.gitignore`.

## Future Release Packaging

If maintainers decide to publish source-built firmware archives from CI, package only source-built outputs. Each archive should include:

- Firmware binaries.
- A manifest with the example path, framework, toolchain version, target, and commit.
- Flash arguments or offsets.
- A simple flashing helper for common host environments.

Do not mix factory firmware and CI-generated source builds in the same artifact.
