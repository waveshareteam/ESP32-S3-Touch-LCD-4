from __future__ import annotations

import argparse
import importlib.util
import json
import os
import sys
import tempfile
import unittest
import zipfile
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[1]


def load_module(name: str, relative_path: str):
    spec = importlib.util.spec_from_file_location(name, REPO_ROOT / relative_path)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"cannot load {relative_path}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[name] = module
    spec.loader.exec_module(module)
    return module


discover = load_module("discover_examples", "scripts/discover_examples.py")
package_firmware = load_module("package_firmware", "releases/package_firmware.py")
download_artifacts = load_module(
    "download_artifacts_impl", "releases/download_artifacts_impl.py"
)


def write_file(root: Path, relative_path: str, content: str = "") -> Path:
    path = root / relative_path
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(content, encoding="utf-8")
    return path


class DiscoveryTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temp_dir = tempfile.TemporaryDirectory()
        self.repo = Path(self.temp_dir.name)

        write_file(self.repo, "examples/esp-idf/Alpha/CMakeLists.txt")
        write_file(self.repo, "examples/esp-idf/Alpha/main/main.c")
        write_file(self.repo, "examples/esp-idf/Alpha/components/local/CMakeLists.txt")
        write_file(self.repo, "examples/esp-idf/Beta/CMakeLists.txt")
        write_file(self.repo, "examples/esp-idf/NotAProject/main/CMakeLists.txt")

        write_file(self.repo, "examples/arduino/Blink/Blink.ino")
        write_file(self.repo, "examples/arduino/Clock/Clock.ino")
        write_file(
            self.repo,
            "examples/arduino/libraries/Bundled/examples/Demo/Demo.ino",
        )

    def tearDown(self) -> None:
        self.temp_dir.cleanup()

    def args(self, surface: str, selector: str) -> argparse.Namespace:
        return argparse.Namespace(
            repo=str(self.repo),
            surface=surface,
            selector=selector,
            idf_versions=discover.DEFAULT_IDF_VERSIONS,
            arduino_core=discover.DEFAULT_ARDUINO_CORE,
            fqbn="esp32:esp32:esp32s3",
        )

    def matrix(self, surface: str, selector: str) -> list[dict[str, str]]:
        return discover.build_matrix(self.args(surface, selector))["include"]

    def test_discovery_excludes_nested_projects_and_bundled_examples(self) -> None:
        idf_names = [entry["name"] for entry in discover.discover_esp_idf(self.repo)]
        arduino_names = [
            entry["name"] for entry in discover.discover_arduino(self.repo)
        ]

        self.assertEqual(idf_names, ["Alpha", "Beta"])
        self.assertEqual(arduino_names, ["Blink", "Clock"])

    def test_selector_matches_names_directories_ancestors_and_internal_paths(self) -> None:
        entry = {"name": "Alpha", "path": "examples/esp-idf/Alpha"}

        for selector in (
            "Alpha",
            "examples/esp-idf/Alpha",
            "examples/esp-idf",
            "examples/esp-idf/Alpha/main/main.c",
        ):
            with self.subTest(selector=selector):
                self.assertTrue(discover.selector_matches(entry, selector))

        self.assertFalse(
            discover.selector_matches(entry, "examples/esp-idf/Beta/main/main.c")
        )

    def test_internal_paths_select_only_the_owning_example(self) -> None:
        idf_matrix = self.matrix("esp-idf", "examples/esp-idf/Alpha/main/main.c")
        arduino_matrix = self.matrix(
            "arduino", "examples/arduino/Clock/Clock.ino"
        )

        self.assertEqual(len(idf_matrix), 2)
        self.assertEqual({entry["name"] for entry in idf_matrix}, {"Alpha"})
        self.assertEqual(len(arduino_matrix), 1)
        self.assertEqual(arduino_matrix[0]["name"], "Clock")

    def test_common_global_paths_select_both_full_matrices(self) -> None:
        for selector in discover.COMMON_GLOBAL_SELECTORS:
            with self.subTest(selector=selector):
                self.assertEqual(len(self.matrix("esp-idf", selector)), 4)
                self.assertEqual(len(self.matrix("arduino", selector)), 2)

    def test_surface_global_paths_select_only_the_affected_surface(self) -> None:
        config_path = "config/sdkconfig.defaults"
        library_path = "examples/arduino/libraries/Bundled/src/Bundled.cpp"

        self.assertEqual(len(self.matrix("esp-idf", config_path)), 4)
        self.assertEqual(self.matrix("arduino", config_path), [])
        self.assertEqual(self.matrix("esp-idf", library_path), [])
        self.assertEqual(len(self.matrix("arduino", library_path)), 2)

    def test_invalid_selector_has_no_matches_on_either_surface(self) -> None:
        selector = "examples/does-not-exist/main.c"

        self.assertEqual(self.matrix("esp-idf", selector), [])
        self.assertEqual(self.matrix("arduino", selector), [])

    def test_framework_defaults_match_supported_ci_versions(self) -> None:
        self.assertEqual(discover.DEFAULT_IDF_VERSIONS, "v5.5.5,v6.0.2")
        self.assertEqual(discover.DEFAULT_ARDUINO_CORE, "3.3.11")


class WorkflowContractTests(unittest.TestCase):
    def test_example_workflow_enforces_selection_and_versions(self) -> None:
        workflow = (REPO_ROOT / ".github/workflows/examples.yml").read_text(
            encoding="utf-8"
        )

        self.assertIn("--idf-versions v5.5.5,v6.0.2", workflow)
        self.assertIn("--arduino-core 3.3.11", workflow)
        self.assertIn("No examples matched selector", workflow)
        self.assertIn('"releases/package_firmware.py"', workflow)
        self.assertNotIn('"releases/**"', workflow)


class ReleaseHelperTests(unittest.TestCase):
    def test_esp_idf_flash_entries_follow_flasher_args(self) -> None:
        with tempfile.TemporaryDirectory() as temp_name:
            root = Path(temp_name)
            build_dir = root / "build"
            firmware_dir = root / "firmware"
            firmware_dir.mkdir()

            write_file(build_dir, "bootloader/bootloader.bin", "boot")
            write_file(build_dir, "partition_table/partition-table.bin", "part")
            write_file(build_dir, "application.bin", "app")
            flasher_args = {
                "flash_files": {
                    "0x10000": "application.bin",
                    "0x0": "bootloader/bootloader.bin",
                    "0x8000": "partition_table/partition-table.bin",
                }
            }
            write_file(
                build_dir,
                "flasher_args.json",
                json.dumps(flasher_args),
            )

            command_pairs, entries, parsed = package_firmware.esp_idf_flash_entries(
                build_dir, firmware_dir
            )

            self.assertEqual(parsed, flasher_args)
            self.assertEqual(
                command_pairs,
                [
                    "0x0",
                    "bin/0x0_bootloader.bin",
                    "0x8000",
                    "bin/0x8000_partition-table.bin",
                    "0x10000",
                    "bin/0x10000_application.bin",
                ],
            )
            self.assertEqual(
                [entry["offset"] for entry in entries],
                ["0x0", "0x8000", "0x10000"],
            )

    def test_arduino_flash_entries_prefer_merged_binary(self) -> None:
        with tempfile.TemporaryDirectory() as temp_name:
            root = Path(temp_name)
            build_dir = root / "build"
            firmware_dir = root / "firmware"
            firmware_dir.mkdir()
            write_file(build_dir, "Blink.ino.bin", "app")
            write_file(build_dir, "Blink.ino.merged.bin", "merged")

            command_pairs, entries = package_firmware.arduino_flash_entries(
                build_dir, firmware_dir
            )

            self.assertEqual(
                command_pairs, ["0x0", "bin/Blink.ino.merged.bin"]
            )
            self.assertEqual(entries[0]["source"], "Blink.ino.merged.bin")

    def test_flash_helpers_include_cross_platform_contract_files(self) -> None:
        with tempfile.TemporaryDirectory() as temp_name:
            package_dir = Path(temp_name)
            command = [
                "python",
                "-m",
                "esptool",
                "--port",
                "$PORT",
                "write_flash",
                "0x0",
                "bin/app.bin",
            ]

            package_firmware.write_flash_helpers(package_dir, command, "demo")

            for name in ("flash.sh", "flash.bat", "flash_args.txt", "README.md"):
                with self.subTest(name=name):
                    self.assertTrue((package_dir / name).is_file())

    def test_package_manifest_uses_standard_and_compatibility_fields(self) -> None:
        with tempfile.TemporaryDirectory() as temp_name:
            repo = Path(temp_name)
            write_file(repo, "examples/esp-idf/Demo/CMakeLists.txt")
            write_file(repo, "build/application.bin", "app")
            write_file(
                repo,
                "build/flasher_args.json",
                json.dumps({"flash_files": {"0x10000": "application.bin"}}),
            )
            args = argparse.Namespace(
                framework="esp-idf",
                project="examples/esp-idf/Demo",
                build_dir="build",
                output_dir="dist",
                name="demo",
                framework_version="v6.0.2",
                target="esp32s3",
                git_sha="abc123",
            )

            previous_cwd = Path.cwd()
            try:
                os.chdir(repo)
                archive_path = package_firmware.package(args)
                with zipfile.ZipFile(archive_path) as archive:
                    manifest = json.loads(archive.read("demo/manifest.json"))
            finally:
                os.chdir(previous_cwd)

            self.assertEqual(
                manifest["project_path"], "examples/esp-idf/Demo"
            )
            self.assertEqual(manifest["project"], manifest["project_path"])
            self.assertEqual(manifest["generated_at"], manifest["timestamp_utc"])
            self.assertTrue(manifest["timestamp_utc"].endswith("+00:00"))
            self.assertEqual(manifest["git_sha"], "abc123")

    def test_download_summary_paths_are_relative_to_run_directory(self) -> None:
        with tempfile.TemporaryDirectory() as temp_name:
            output_root = Path(temp_name) / "run-123"
            artifact_dir = output_root / "firmware-demo"
            summary_path = download_artifacts.relative_output_path(artifact_dir, output_root)

            self.assertEqual(summary_path, "firmware-demo")

    def test_github_auth_help_uses_session_scoped_credentials(self) -> None:
        help_text = download_artifacts.github_auth_help()

        self.assertIn("gh auth login", help_text)
        self.assertNotIn("startup", help_text.lower())

    def test_download_extraction_rejects_parent_traversal(self) -> None:
        with self.assertRaises(ValueError):
            download_artifacts.member_parts("../outside.bin", None)

        self.assertEqual(
            download_artifacts.member_parts("bundle/bin/app.bin", "bundle"),
            ("bin", "app.bin"),
        )


if __name__ == "__main__":
    unittest.main()
