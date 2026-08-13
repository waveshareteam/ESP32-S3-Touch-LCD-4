#!/usr/bin/env python3
"""Verify immutable, checked-in firmware artifacts without modifying them."""

from __future__ import annotations

import argparse
import hashlib
import json
import subprocess
import sys
from pathlib import Path, PurePosixPath


MANIFEST_VERSION = 1
SHA256_LENGTH = 64
DELIVERY_ARCHIVE_SUFFIXES = frozenset(
    {".zip", ".7z", ".rar", ".tar", ".tgz", ".gz", ".bz2", ".xz"}
)


class VerificationError(ValueError):
    """The manifest or an immutable artifact is invalid."""


def safe_relative_path(value: object) -> str:
    if not isinstance(value, str) or not value:
        raise VerificationError("artifact path must be a non-empty string")
    if "\\" in value:
        raise VerificationError(f"artifact path must use POSIX separators: {value}")
    path = PurePosixPath(value)
    if path.is_absolute() or any(part in {"", ".", ".."} for part in path.parts):
        raise VerificationError(f"unsafe artifact path: {value}")
    return path.as_posix()


def index_entries(repo: Path) -> dict[str, tuple[str, str]]:
    result = subprocess.run(
        ["git", "-C", str(repo), "ls-files", "-s", "-z"],
        check=False,
        capture_output=True,
    )
    if result.returncode:
        raise VerificationError("cannot read checked-in files with git ls-files")
    entries: dict[str, tuple[str, str]] = {}
    for item in result.stdout.split(b"\0"):
        if not item:
            continue
        metadata, path = item.split(b"\t", 1)
        mode, object_id, stage = metadata.decode("ascii").split()
        if stage != "0":
            continue
        entries[path.decode("utf-8")] = (mode, object_id)
    return entries


def index_blob(repo: Path, object_id: str) -> bytes:
    result = subprocess.run(
        ["git", "-C", str(repo), "cat-file", "blob", object_id],
        check=False,
        capture_output=True,
    )
    if result.returncode:
        raise VerificationError("cannot read artifact from the Git index")
    return result.stdout


def reject_duplicate_keys(pairs: list[tuple[str, object]]) -> dict[str, object]:
    result: dict[str, object] = {}
    for key, value in pairs:
        if key in result:
            raise VerificationError(f"duplicate JSON object key: {key}")
        result[key] = value
    return result


def read_manifest(contents: bytes) -> list[dict[str, object]]:
    try:
        data = json.loads(contents.decode("utf-8"), object_pairs_hook=reject_duplicate_keys)
    except (UnicodeDecodeError, json.JSONDecodeError) as error:
        raise VerificationError(f"cannot read manifest: {error}") from error
    if not isinstance(data, dict) or set(data) != {"version", "artifacts"}:
        raise VerificationError("manifest schema must contain only version and artifacts")
    if (
        type(data["version"]) is not int
        or data["version"] != MANIFEST_VERSION
        or not isinstance(data["artifacts"], list)
    ):
        raise VerificationError("unsupported manifest version or artifacts list")

    artifacts: list[dict[str, object]] = []
    seen_paths: set[str] = set()
    for item in data["artifacts"]:
        if not isinstance(item, dict) or set(item) != {"path", "kind", "sha256", "size"}:
            raise VerificationError("artifact schema must contain path, kind, sha256, and size")
        path = safe_relative_path(item["path"])
        kind = item["kind"]
        digest = item["sha256"]
        size = item["size"]
        if path in seen_paths:
            raise VerificationError(f"duplicate artifact path: {path}")
        seen_paths.add(path)
        if kind not in {"firmware", "delivery_archive"}:
            raise VerificationError(f"unsupported artifact kind: {kind}")
        if kind == "firmware" and Path(path).suffix.lower() != ".bin":
            raise VerificationError(f"firmware artifact must end in .bin: {path}")
        if kind == "delivery_archive" and Path(path).suffix.lower() not in DELIVERY_ARCHIVE_SUFFIXES:
            raise VerificationError(f"delivery archive has an unsupported suffix: {path}")
        if not isinstance(digest, str) or len(digest) != SHA256_LENGTH or any(
            char not in "0123456789abcdef" for char in digest
        ):
            raise VerificationError(f"invalid SHA-256 digest: {path}")
        if isinstance(size, bool) or not isinstance(size, int) or size < 0:
            raise VerificationError(f"invalid artifact size: {path}")
        artifacts.append({"path": path, "kind": kind, "sha256": digest, "size": size})
    return artifacts


def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def verify(repo: Path, manifest: Path, index: bool = False) -> None:
    repo = repo.resolve()
    try:
        manifest_relative = safe_relative_path(manifest.absolute().relative_to(repo).as_posix())
    except ValueError as error:
        raise VerificationError("manifest must be inside the repository") from error
    entries = index_entries(repo)
    manifest_entry = entries.get(manifest_relative)
    if index:
        if manifest_entry is None:
            raise VerificationError("manifest is not checked in")
        if manifest_entry[0] != "100644":
            raise VerificationError("manifest must be a regular index entry")
        manifest_contents = index_blob(repo, manifest_entry[1])
    else:
        if manifest.is_symlink():
            raise VerificationError("manifest must not be a symlink")
        if manifest_entry is not None and manifest_entry[0] != "100644":
            raise VerificationError("manifest must be a regular index entry")
        try:
            manifest_contents = manifest.read_bytes()
        except OSError as error:
            raise VerificationError(f"cannot read manifest: {error}") from error
    artifacts = read_manifest(manifest_contents)
    listed_paths = {artifact["path"] for artifact in artifacts}

    for artifact in artifacts:
        path = artifact["path"]
        file_path = repo / path
        try:
            file_path.resolve().relative_to(repo)
        except ValueError as error:
            raise VerificationError(f"artifact resolves outside the repository: {path}") from error
        entry = entries.get(path)
        if entry is None:
            raise VerificationError(f"artifact is not checked in: {path}")
        if entry[0] == "120000" or (not index and file_path.is_symlink()):
            raise VerificationError(f"artifact must not be a symlink: {path}")
        if entry[0] != "100644":
            raise VerificationError(f"artifact must be a regular index entry: {path}")
        if index:
            contents = index_blob(repo, entry[1])
            if len(contents) != artifact["size"]:
                raise VerificationError(f"size mismatch: {path}")
            if hashlib.sha256(contents).hexdigest() != artifact["sha256"]:
                raise VerificationError(f"SHA-256 mismatch: {path}")
            continue
        if not file_path.is_file():
            raise VerificationError(f"missing artifact: {path}")
        if file_path.stat().st_size != artifact["size"]:
            raise VerificationError(f"size mismatch: {path}")
        if sha256_file(file_path) != artifact["sha256"]:
            raise VerificationError(f"SHA-256 mismatch: {path}")

    unlisted_artifacts = sorted(
        path
        for path in entries
        if Path(path).suffix.lower() == ".bin" and path not in listed_paths
    )
    if unlisted_artifacts:
        raise VerificationError(
            f"unlisted checked-in firmware binary: {', '.join(unlisted_artifacts)}"
        )


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("--repo", default=".")
    parser.add_argument("--manifest", default="firmware/artifacts.json")
    parser.add_argument("--index", action="store_true", help="verify Git index blobs")
    args = parser.parse_args()
    repo = Path(args.repo)
    manifest = Path(args.manifest)
    if not manifest.is_absolute():
        manifest = repo / manifest
    try:
        verify(repo, manifest, args.index)
    except VerificationError as error:
        print(f"firmware-artifacts: {error}", file=sys.stderr)
        raise SystemExit(1)
    print("firmware artifacts verified")


if __name__ == "__main__":
    main()
