# Release Scripts

This directory contains helper scripts for packaging build outputs into flashable firmware archives.

Command examples below match the workflow versions current at the time of this update. The workflow file remains the source of truth when framework pins change.

## Board Variant Note

The generated artifacts target the shared ESP32-S3-Touch-LCD-4 / ESP32-S3-LCD-4 hardware family, but ESP32-S3-LCD-4 does not populate the GT911 touch controller. Display and peripheral firmware can usually be flashed to either board. Touch-driven LVGL or ESP-Brookesia firmware should be used on ESP32-S3-Touch-LCD-4, or adapted to run without pointer input before flashing to ESP32-S3-LCD-4.

## ESP-IDF

Build an example first, then package the generated build directory:

```bash
idf.py -C examples/esp-idf/06_lvgl_demo_v8 -B build/06_lvgl_demo_v8-v6.0.2 set-target esp32s3 build
python3 releases/package_firmware.py \
  --framework esp-idf \
  --project examples/esp-idf/06_lvgl_demo_v8 \
  --build-dir build/06_lvgl_demo_v8-v6.0.2 \
  --framework-version v6.0.2 \
  --target esp32s3
```

The script reads ESP-IDF's `flasher_args.json`, copies the required binary files, writes flash helper scripts, and creates a zip under `releases/dist/`.

## Arduino

Export binaries into a stable output directory, then package them:

```bash
arduino-cli compile \
  --fqbn esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB \
  --libraries examples/arduino/libraries \
  --export-binaries \
  --output-dir build/01_HelloWorld-3.3.11 \
  examples/arduino/01_HelloWorld

python3 releases/package_firmware.py \
  --framework arduino \
  --project examples/arduino/01_HelloWorld \
  --build-dir build/01_HelloWorld-3.3.11 \
  --framework-version 3.3.11 \
  --target esp32s3
```

Each archive includes `manifest.json`, `flash.sh`, `flash.bat`, `flash_args.txt`, and the firmware binaries under `bin/`.

`manifest.json` records `project_path` and `timestamp_utc` together with the framework version, Git commit, target, flash command, and file offsets. Compatibility aliases `project` and `generated_at` are retained.

## Download CI Artifacts

After a CI run completes, download and extract firmware artifacts with:

```bash
python3 releases/download_artifacts.py --run-id <run-id> --clean
```

If `--run-id` is omitted, the script finds the latest successful `examples.yml` run for the current branch:

```bash
python3 releases/download_artifacts.py --clean
```

The extracted firmware is written to `releases/downloads/run-<run-id>/`. Each artifact gets its own folder, for example `firmware-esp-idf-06_lvgl_demo_v8-v6.0.2/`, with `flash.sh`, `flash.bat`, `manifest.json`, and `bin/` ready for flashing. The sibling `artifacts.json` summary stores each artifact path relative to that run directory so it remains portable.

Use `--artifact <name>` to download one firmware package, or `--pattern "firmware-esp-idf-*v6.0.2"` to filter by glob pattern.

For interactive use, authenticate with `gh auth login`; the downloader reuses that session. For automation, set `GH_TOKEN` or `GITHUB_TOKEN` only for the current command or session. Do not place repository tokens in shell startup files.
