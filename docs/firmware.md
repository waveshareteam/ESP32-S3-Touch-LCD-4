# Firmware Artifacts

[中文](firmware_CN.md)

`Firmware/` contains factory binary artifacts for user flashing and recovery flows. These binaries are released files from Waveshare; they are not source projects and are not rebuilt by CI.

Source-maintained firmware should live under `examples/esp-idf/` or another documented source directory with its own `CMakeLists.txt`, component manifest, and validation path.

CI build outputs are packaged by `releases/package_firmware.py` and uploaded as workflow artifacts. Each generated zip contains `manifest.json`, `flash.sh`, `flash.bat`, `flash_args.txt`, and the binaries needed by esptool.

For local release packaging, build the target project first and run the Python script from the repository root. Generated archives are written under `releases/dist/` by default.

Do not mix factory firmware and CI-generated source builds in the same artifact.
