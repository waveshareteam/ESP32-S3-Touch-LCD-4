# 发布脚本

[English](README.md)

本目录包含将构建输出打包为可烧录固件归档的工具。工作流文件是框架版本的唯一来源。ESP-IDF 和 Arduino 打包命令分别使用 `releases/package_firmware.py`，每个归档包含 `manifest.json`、烧录脚本、参数和二进制文件。

可用 `python3 releases/download_artifacts.py --run-id <run-id> --clean` 下载指定 CI 运行的归档；省略 `--run-id` 时下载当前分支最近的成功运行。认证请使用当前会话的 `gh auth login`、`GH_TOKEN` 或 `GITHUB_TOKEN`，不要写入 shell 启动文件。
