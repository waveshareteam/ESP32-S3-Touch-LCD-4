# Config

[中文](README_CN.md)

This directory is reserved for shared configuration overlays used by more than one ESP-IDF example.

No shared overlays are active yet. The repository currently keeps each maintained setting in the owning example's `sdkconfig.defaults`.

Keep example-specific `sdkconfig.defaults` files next to the example that owns them. Move only reusable configuration fragments here, and document how they are consumed before adding new files.

`markdown-audit.json` is the repository contract for a full Markdown audit: it records first-party English/Chinese pairs, the single-product homepage rule, and the narrow machine-template exemptions.
