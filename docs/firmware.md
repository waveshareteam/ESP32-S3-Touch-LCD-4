# Firmware Artifacts

[中文](firmware_CN.md)

`firmware/` contains factory/recovery binary artifacts for user flashing and recovery flows. These binaries are released files from Waveshare; they are not source projects and are not rebuilt by CI.

The immutable factory artifact inventory is [`firmware/artifacts.json`](../firmware/artifacts.json). Verify the working-tree files read-only from the repository root with `python3 scripts/verify_firmware_artifacts.py --repo . --manifest firmware/artifacts.json`; CI adds `--index` to verify the exact checked-in blobs. The manifest records only the repository-relative artifact path, SHA-256 digest, byte size, and artifact kind. It does not make factory firmware part of example matrices or infer an archive's role from its filename suffix alone; an archive becomes a delivery artifact only when first-party evidence explicitly classifies it in the manifest.

Source-maintained firmware should live under `examples/esp-idf/` or another documented source directory with its own `CMakeLists.txt`, component manifest, and validation path.

CI build outputs are packaged by `releases/package_firmware.py` and uploaded as workflow artifacts. Before flashing artifacts to ESP32-S3-LCD-4, prefer display/peripheral examples or firmware paths that tolerate missing GT911 touch input. Each generated zip contains `manifest.json`, `flash.sh`, `flash.bat`, `flash_args.txt`, and the binaries needed by esptool.

`manifest.json` records the repo-relative `project_path`, UTC `timestamp_utc`, framework version, Git commit, target, flash command, and file offsets. The legacy `project` and `generated_at` keys remain for consumers of earlier archives.

For local release packaging, build the target project first and run the Python script from the repository root. Generated archives are written under `releases/dist/` by default.

Do not mix factory/recovery firmware and CI-generated source builds in the same artifact or report them as the same build surface.
