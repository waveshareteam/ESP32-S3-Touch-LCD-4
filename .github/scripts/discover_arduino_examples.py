#!/usr/bin/env python3
"""Discover Arduino sketches that should be compiled by CI."""

from __future__ import annotations

import argparse
import fnmatch
import json
import os
import subprocess
import sys
from pathlib import Path


ARDUINO_ROOT = Path("examples/Arduino-v3.3.2")
SKETCHES_ROOT = ARDUINO_ROOT / "examples"
LIBRARIES_ROOT = ARDUINO_ROOT / "libraries"
GLOBAL_SKETCH_PATTERNS = (
    ".github/workflows/arduino-examples.yml",
    ".github/scripts/discover_arduino_examples.py",
    "docs/CI.md",
    "docs/CI_CN.md",
    "examples/README.md",
    "examples/README_CN.md",
    "examples/Arduino-v3.3.2/README.md",
    "examples/Arduino-v3.3.2/README_CN.md",
    "README.md",
    "README_CN.md",
)


def run_git(args: list[str]) -> list[str]:
    result = subprocess.run(
        ["git", *args],
        check=True,
        text=True,
        stdout=subprocess.PIPE,
    )
    return [line.strip() for line in result.stdout.splitlines() if line.strip()]


def list_sketches() -> list[str]:
    if not SKETCHES_ROOT.exists():
        return []

    sketches = []
    for path in SKETCHES_ROOT.iterdir():
        if path.is_dir() and any(child.suffix.lower() == ".ino" for child in path.iterdir()):
            sketches.append(path.as_posix())
    return sorted(sketches)


def normalize_sketch(value: str) -> str:
    value = value.strip().strip("/").replace("\\", "/")
    if not value:
        return value
    if value == "all":
        return value

    if value.endswith(".ino"):
        value = str(Path(value).parent).replace("\\", "/")

    root = SKETCHES_ROOT.as_posix()
    if value.startswith(root + "/"):
        parts = value.split("/")
        if len(parts) >= 4:
            return "/".join(parts[:4])
        return value

    return (SKETCHES_ROOT / value).as_posix()


def discover_from_paths(paths: list[str], known_sketches: set[str]) -> list[str]:
    selected = set()
    arduino_root = ARDUINO_ROOT.as_posix()
    sketches_root = SKETCHES_ROOT.as_posix()
    libraries_root = LIBRARIES_ROOT.as_posix()
    arduino_prefix = arduino_root + "/"
    sketches_prefix = sketches_root + "/"
    libraries_prefix = libraries_root + "/"

    for changed_path in paths:
        changed_path = changed_path.strip().strip("/").replace("\\", "/")
        if any(fnmatch.fnmatch(changed_path, pattern) for pattern in GLOBAL_SKETCH_PATTERNS):
            selected.update(known_sketches)
            continue

        if changed_path == libraries_root or changed_path.startswith(libraries_prefix):
            selected.update(known_sketches)
            continue

        if changed_path == sketches_root:
            selected.update(known_sketches)
            continue

        if changed_path.startswith(sketches_prefix):
            parts = changed_path.split("/")
            if len(parts) < 4:
                selected.update(known_sketches)
                continue

            sketch = "/".join(parts[:4])
            if sketch in known_sketches:
                selected.add(sketch)
            continue

        if changed_path == arduino_root or changed_path.startswith(arduino_prefix):
            selected.update(known_sketches)

    return sorted(selected)


def discover_changed_sketches(
    base_ref: str | None,
    head_ref: str,
    known_sketches: set[str],
) -> list[str]:
    if base_ref:
        diff_args = ["diff", "--name-only", f"{base_ref}...{head_ref}"]
    else:
        diff_args = ["diff-tree", "--no-commit-id", "--name-only", "-r", head_ref]

    return discover_from_paths(run_git(diff_args), known_sketches)


def github_output(name: str, value: str) -> None:
    output_path = os.environ.get("GITHUB_OUTPUT")
    if output_path:
        with open(output_path, "a", encoding="utf-8") as output:
            output.write(f"{name}={value}\n")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--base-ref")
    parser.add_argument("--head-ref", default="HEAD")
    parser.add_argument("--sketch", default="")
    parser.add_argument(
        "--fallback-all",
        action="store_true",
        help="Compile all sketches when no changed sketch is detected.",
    )
    args = parser.parse_args()

    known_sketches = set(list_sketches())
    requested_sketch = normalize_sketch(args.sketch)

    if requested_sketch == "all":
        selected = sorted(known_sketches)
    elif requested_sketch:
        if requested_sketch not in known_sketches:
            print(f"Unknown Arduino sketch: {args.sketch}", file=sys.stderr)
            print("Known sketches:", file=sys.stderr)
            for sketch in sorted(known_sketches):
                print(f"  {sketch}", file=sys.stderr)
            return 1
        selected = [requested_sketch]
    else:
        selected = discover_changed_sketches(args.base_ref, args.head_ref, known_sketches)
        if args.fallback_all and not selected:
            selected = sorted(known_sketches)

    matrix = {"sketch": selected}
    matrix_json = json.dumps(matrix, separators=(",", ":"))
    has_sketches = "true" if selected else "false"

    github_output("matrix", matrix_json)
    github_output("has_sketches", has_sketches)
    github_output("sketches", ",".join(selected))

    print(matrix_json)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
