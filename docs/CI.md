# CI

[中文](CI_CN.md)

The `Build Examples` workflow first classifies changes, runs the Markdown gate in the route job, then builds only the affected source-maintained examples in GitHub Actions. Its `ci-status` job is always present on pull requests and main/master pushes, including documentation-only changes.

- ESP-IDF projects are discovered from `examples/esp-idf/*/CMakeLists.txt`.
- Arduino sketches are discovered from `.ino` files under `examples/arduino/`, excluding `examples/arduino/libraries/**`.
- Factory/recovery firmware under `firmware/` is documented for flashing, but is not rebuilt by CI.

Markdown is documentation-only even inside an example or bundled library. Direct example source selects its owning entry; `config/`, ESP-IDF shared inputs, bundled Arduino-library source, workflow/discovery/packaging inputs select the relevant full surface. Unknown non-document inputs conservatively select both surfaces. `firmware/` changes are reported as a separate maintenance surface and never enter an example matrix; binary or archive changes additionally require release review.

On pull requests, the route job runs the repository-local Markdown audit against the merge base at the exact pull-request head. Ordinary branch pushes use the non-zero `before` commit as the changed-scope base; tags, manual runs, and all-zero `before` pushes run only the complete inventory. When routing reports `docs_only=true`, every changed-scope command also asserts `--expect-docs-only`. Every event runs the complete tracked Markdown inventory. Any audit failure fails `route`, and therefore `ci-status`.

The Markdown contract defines this homepage as a multi-product hub: the localized headers use a neutral family title, a local family hero, and product quick links. The inventory runs without `--strict` because existing public `_CN.md` paths produce a known warning; those paths remain stable while policy errors still fail the gate.

`workflow_dispatch` accepts `all`, an example directory name, or a repo-relative path and rejects a selector with no matching example.

Matrix configured at the time of this update:

- ESP-IDF `v5.5.5` and `v6.0.2`, target `esp32s3`.
- Arduino-ESP32 core `3.3.11`, FQBN `esp32:esp32:esp32s3` with 16 MB Flash, OPI PSRAM, USB CDC on boot, and `app3M_fat9M_16MB` partitioning.
- Bundled Arduino libraries from `examples/arduino/libraries`.

The workflow and discovery script together are the source of truth for version pins. Update this snapshot whenever that current CI configuration changes.

The largest matrix is 33 builds (10 ESP-IDF examples × 2 versions plus 13 Arduino sketches). Each successful matrix build uploads a flashable firmware artifact. Download the artifact zip from the workflow run, extract it, then run `flash.sh` or `flash.bat` with the board serial port.

For artifact packaging and download details, see [../releases/README.md](../releases/README.md).

If an example requires hardware, credentials, or an upstream component that is not yet compatible with a selected framework version, document the exclusion here before excluding it from CI.
