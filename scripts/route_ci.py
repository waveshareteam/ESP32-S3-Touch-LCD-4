#!/usr/bin/env python3
"""Classify changed paths into the smallest safe example CI matrices."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass, field
from pathlib import Path

import discover_examples


DOC_SUFFIXES = {".md", ".markdown", ".rst"}
DOC_ASSET_SUFFIXES = {".png", ".jpg", ".jpeg", ".gif", ".svg", ".webp", ".pdf"}
DOCUMENTATION_ASSET_PATHS = frozenset({"assets/ESP32-S3-LCD-4-family.jpg"})
GLOBAL_PATHS = frozenset(
    {
        ".github/workflows/examples.yml",
        "scripts/route_ci.py",
        "scripts/discover_examples.py",
        "releases/package_firmware.py",
    }
)
IDF_SHARED_PREFIXES = ("config/", "examples/esp-idf/components/", "components/")
ARDUINO_LIBRARY_PREFIX = "examples/arduino/libraries/"
FIRMWARE_PREFIX = "firmware/"
RELEASE_ARTIFACT_SUFFIXES = {".bin", ".zip", ".7z", ".rar", ".tar", ".tgz", ".gz", ".bz2", ".xz"}


def normalize(path: str) -> str:
    path = path.replace("\\", "/").strip().strip("/")
    while path.startswith("./"):
        path = path[2:]
    return path


def is_document(path: str) -> bool:
    return Path(path).suffix.lower() in DOC_SUFFIXES


def is_docs_asset(path: str) -> bool:
    return path in DOCUMENTATION_ASSET_PATHS or (
        path.startswith("docs/") and Path(path).suffix.lower() in DOC_ASSET_SUFFIXES
    )


def is_governance(path: str) -> bool:
    name = Path(path).name.lower()
    return (
        name.startswith("license")
        or name.startswith("code_of_conduct")
        or path.startswith(".github/ISSUE_TEMPLATE/")
        or path == ".github/pull_request_template.md"
        or path.startswith("tests/")
        or path == "config/markdown-audit.json"
        or (path.startswith("releases/") and (is_document(path) or name.startswith("download_artifacts")))
    )


def parse_name_status(text: str) -> list[str]:
    """Return every relevant path, retaining both sides of renames/copies."""
    paths: list[str] = []
    for raw in text.splitlines():
        fields = raw.split("\t")
        if not fields or not fields[0]:
            continue
        status = fields[0]
        if status.startswith(("R", "C")) and len(fields) >= 3:
            paths.extend((normalize(fields[1]), normalize(fields[2])))
        elif len(fields) >= 2:
            paths.append(normalize(fields[1]))
    return [path for path in paths if path]


def _owned(path: str, entries: list[dict[str, str]]) -> str | None:
    for entry in entries:
        root = entry["path"]
        if path == root or path.startswith(root + "/"):
            return root
    return None


@dataclass
class Route:
    idf: set[str] = field(default_factory=set)
    arduino: set[str] = field(default_factory=set)
    all_idf: bool = False
    all_arduino: bool = False
    firmware_touched: bool = False
    release_review_required: bool = False
    unknown: list[str] = field(default_factory=list)
    non_document: bool = False

    def summary(self) -> str:
        parts = []
        if self.all_idf:
            parts.append("all ESP-IDF")
        elif self.idf:
            parts.append("ESP-IDF: " + ", ".join(sorted(self.idf)))
        if self.all_arduino:
            parts.append("all Arduino")
        elif self.arduino:
            parts.append("Arduino: " + ", ".join(sorted(self.arduino)))
        if self.firmware_touched:
            parts.append("firmware release review required" if self.release_review_required else "firmware touched")
        elif self.release_review_required:
            parts.append("release review required")
        if self.unknown:
            parts.append("unknown paths: " + ", ".join(self.unknown))
        return "; ".join(parts) or "documentation/governance only"


def classify(paths: list[str], repo: Path) -> Route:
    idf_entries = discover_examples.discover_esp_idf(repo)
    arduino_entries = discover_examples.discover_arduino(repo)
    route = Route()
    for path in paths:
        if path.startswith(FIRMWARE_PREFIX):
            route.firmware_touched = True
            route.release_review_required |= Path(path).suffix.lower() in RELEASE_ARTIFACT_SUFFIXES
            route.non_document |= not is_document(path)
            continue
        if is_document(path) or is_docs_asset(path) or is_governance(path):
            continue
        route.non_document = True
        if path in GLOBAL_PATHS:
            route.all_idf = route.all_arduino = True
            continue
        if path.startswith(IDF_SHARED_PREFIXES):
            route.all_idf = True
            continue
        if path.startswith(ARDUINO_LIBRARY_PREFIX):
            route.all_arduino = True
            continue
        idf_owner = _owned(path, idf_entries)
        if idf_owner:
            route.idf.add(idf_owner)
            continue
        arduino_owner = _owned(path, arduino_entries)
        if arduino_owner:
            route.arduino.add(arduino_owner)
            continue
        # An archive or binary inside an owned example can be a real runtime or
        # build input, so ownership rules above take precedence. Unowned
        # checked-in artifacts are a release-review surface, not a reason to
        # compile unrelated product examples.
        if Path(path).suffix.lower() in RELEASE_ARTIFACT_SUFFIXES:
            route.release_review_required = True
            continue
        route.all_idf = route.all_arduino = True
        route.unknown.append(path)
    return route


def matrix(entries: list[dict[str, str]], selected: set[str], all_entries: bool, surface: str) -> dict[str, list[dict[str, str]]]:
    items = entries if all_entries else [entry for entry in entries if entry["path"] in selected]
    if surface == "esp-idf":
        return {"include": [entry | {"idf": version} for entry in items for version in discover_examples.DEFAULT_IDF_VERSIONS.split(",")]}
    return {"include": [entry | {"core": discover_examples.DEFAULT_ARDUINO_CORE, "fqbn": "esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB"} for entry in items]}


def route_outputs(paths: list[str], repo: Path, selector: str = "") -> dict[str, str]:
    if selector:
        idf = discover_examples.build_matrix(argparse.Namespace(repo=str(repo), surface="esp-idf", selector=selector, idf_versions=discover_examples.DEFAULT_IDF_VERSIONS, arduino_core=discover_examples.DEFAULT_ARDUINO_CORE, fqbn="esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB"))
        arduino = discover_examples.build_matrix(argparse.Namespace(repo=str(repo), surface="arduino", selector=selector, idf_versions=discover_examples.DEFAULT_IDF_VERSIONS, arduino_core=discover_examples.DEFAULT_ARDUINO_CORE, fqbn="esp32:esp32:esp32s3:USBMode=hwcdc,CDCOnBoot=cdc,FlashSize=16M,PSRAM=opi,PartitionScheme=app3M_fat9M_16MB"))
        if not idf["include"] and not arduino["include"]:
            raise ValueError(f"No examples matched selector '{selector}'.")
        route = Route(
            idf={entry["path"] for entry in idf["include"]},
            arduino={entry["path"] for entry in arduino["include"]},
            non_document=True,
        )
    else:
        route = classify(paths, repo)
        idf = matrix(discover_examples.discover_esp_idf(repo), route.idf, route.all_idf, "esp-idf")
        arduino = matrix(discover_examples.discover_arduino(repo), route.arduino, route.all_arduino, "arduino")
    return {
        "idf_matrix": json.dumps(idf, separators=(",", ":")),
        "arduino_matrix": json.dumps(arduino, separators=(",", ":")),
        "idf_count": str(len(idf["include"])),
        "arduino_count": str(len(arduino["include"])),
        "docs_only": str(not route.non_document).lower(),
        "firmware_touched": str(route.firmware_touched).lower(),
        "release_review_required": str(route.release_review_required).lower(),
        "route_summary": route.summary(),
    }


def git_diff(base: str, head: str) -> str:
    result = subprocess.run(["git", "diff", "--name-status", "-M", f"{base}...{head}"], text=True, capture_output=True, check=False)
    if result.returncode:
        raise RuntimeError(result.stderr.strip() or "git diff failed")
    return result.stdout


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo", default=".")
    parser.add_argument("--base")
    parser.add_argument("--head", default="HEAD")
    parser.add_argument("--name-status-file")
    parser.add_argument("--selector", default="")
    parser.add_argument("--github-output")
    args = parser.parse_args()
    try:
        if args.name_status_file:
            text = Path(args.name_status_file).read_text(encoding="utf-8")
        elif args.base:
            text = git_diff(args.base, args.head)
        else:
            text = sys.stdin.read()
        paths = parse_name_status(text)
        if not args.selector and not paths:
            raise ValueError("Changed-path diff is empty or unavailable.")
        outputs = route_outputs(paths, Path(args.repo).resolve(), args.selector)
    except (OSError, RuntimeError, ValueError) as error:
        print(f"route-ci: {error}", file=sys.stderr)
        raise SystemExit(2)
    for key, value in outputs.items():
        print(f"{key}={value}")
    if args.github_output:
        with open(args.github_output, "a", encoding="utf-8") as handle:
            for key, value in outputs.items():
                handle.write(f"{key}={value}\n")


if __name__ == "__main__":
    main()
