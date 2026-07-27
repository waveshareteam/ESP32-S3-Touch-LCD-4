# Repository Structure

This repository uses the Waveshare ESP32 product layout for ESP32-S3-Touch-LCD-4 and ESP32-S3-LCD-4:

- `examples/esp-idf/`: first-party ESP-IDF projects for the shared 4-inch ESP32-S3 LCD board family.
- `examples/arduino/`: first-party Arduino sketches plus bundled libraries required by those sketches.
- `config/`: reserved for shared configuration overlays used by more than one example; no shared overlays are active yet.
- `docs/`: maintainer notes for CI, components, firmware, and compatibility.
- `firmware/`: factory/recovery binary artifacts that are documented but not built in CI.
- `releases/`: scripts for packaging build outputs into flashable firmware archives.
- `hardware/`: the V4.0 hardware reference and public schematic files under `hardware/schematics/`.

CI intentionally builds only first-party examples. Examples and tests inside bundled Arduino libraries remain available for library users, but they are not product CI targets.

ESP32-S3-LCD-4 shares the display and peripheral paths with ESP32-S3-Touch-LCD-4 but does not populate the GT911 touch controller. CI validates that touch-capable examples compile; firmware that requires pointer input should either run in a documented display-only mode or be adapted before flashing to the LCD-only variant.
