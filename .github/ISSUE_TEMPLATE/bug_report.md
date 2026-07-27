---
name: Bug report
about: Report a reproducible problem with this board repository
title: "[Bug] "
labels: bug
assignees: ""
---

### Report Type
Select the build surface involved:

- [ ] Source-built GitHub Actions artifact
- [ ] Source build made outside GitHub Actions
- [ ] Factory/recovery image from `firmware/`
- [ ] Source code, documentation, or hardware issue without a firmware image
- [ ] Unsure

### Summary

Describe the problem briefly.

### Board And Environment

- Board revision:
- Example path:
- Framework: ESP-IDF, Arduino, or not applicable
- Framework version:
- Host operating system:
- Power supply and connected peripherals:

### Source-Built CI Artifact

Complete this section only for a GitHub Actions artifact.

- Workflow run URL:
- Matrix job name:
- Artifact name:
- `manifest.json` project path, Git SHA, and UTC timestamp:

### Factory/Recovery Image

Complete this section only for an image from `firmware/`.

- Firmware filename:
- Flash command/tool and result:

### Reproduction Steps

1.
2.
3.

### Expected Behavior

Describe what should happen.

### Actual Behavior

Describe what happened instead.

### Logs Or Screenshots

Paste the relevant serial log, complete CI job link, flash-tool output, or screenshot. Please remove secrets such as Wi-Fi passwords, tokens, or private URLs.

### Additional Context

Add any wiring, power supply, peripheral, or configuration details that may help reproduce the issue.
