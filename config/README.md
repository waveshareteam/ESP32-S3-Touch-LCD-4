# Config

[中文](README_CN.md)

This directory is reserved for shared configuration overlays used by more than one ESP-IDF example.

No shared overlays are active yet. The repository currently keeps each maintained setting in the owning example's `sdkconfig.defaults`.

Keep example-specific `sdkconfig.defaults` files next to the example that owns them. Move only reusable configuration fragments here, and document how they are consumed before adding new files.

`markdown-audit.json` is the repository contract for the route Markdown gate and full inventory: it records first-party English/Chinese pairs, changed-scope docs-only assertions, and narrow machine-template exemptions. The homepage contract is a multi-product hub with a local family hero and explicit product quick link; the exact family image is the only allowed documentation asset outside `docs/`.

The full inventory does not use `--strict` because existing public `_CN.md` paths have a known warning. Keep those paths stable; policy errors remain gate failures.
