# 配置

[English](README.md)

此目录用于多个 ESP-IDF 示例共用的配置 overlay。当前没有启用共享 overlay；稳定的示例专属设置应保留在所属示例的 `sdkconfig.defaults` 中。新增可复用片段前，请说明其使用方式。

`markdown-audit.json` 是 route Markdown gate 与完整清单的仓库契约，记录第一方中英文配对、changed-scope 的 docs-only 断言和机器模板的窄豁免。首页契约为多产品 hub，使用本地产品族 hero 和显式产品 quick link；产品族图片是 `docs/` 之外唯一允许的文档 asset。

完整清单不启用 `--strict`，因为现有公开 `_CN.md` 路径存在已知 warning。请保持这些路径稳定；策略 error 仍会使 gate 失败。
