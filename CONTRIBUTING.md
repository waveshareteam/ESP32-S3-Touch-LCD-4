# Contributing

Thank you for improving this board repository.

## Pull Requests

1. Open an issue first for larger changes, hardware behavior changes, or public API changes.
2. Keep changes focused on one topic.
3. Update the relevant README or documentation when paths, behavior, hardware assumptions, or CI coverage change.
4. Use repo-relative paths in discussions and validation notes.
5. Let GitHub Actions validate ESP-IDF and Arduino examples for repository changes.

## Local Generated Files

Do not commit local build output or generated dependency folders such as `build/`, `managed_components/`, `dependencies.lock`, local `sdkconfig`, or `sdkconfig.old`.

Use `sdkconfig.defaults` for stable example defaults that should be shared with users and CI.

## Example Coverage

ESP-IDF examples live under `examples/esp-idf/`. Arduino product sketches live under `examples/arduino/examples/` and use bundled libraries from `examples/arduino/libraries/`.

Bundled library examples are not part of product CI unless a maintainer intentionally expands the CI scope.
