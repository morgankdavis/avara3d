#!/usr/bin/env python3

from __future__ import annotations

import argparse
import json
from pathlib import Path

SOURCE_FILES = ("main.cc", "App.h", "App.cc")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Generate the manifest used by the website demo source browser."
    )
    parser.add_argument("demo_directory", type=Path)
    parser.add_argument("output", type=Path)
    return parser.parse_args()


def file_entry(path: Path, demo_directory: Path) -> dict[str, object]:
    return {
        "path": path.relative_to(demo_directory).as_posix(),
        "size": path.stat().st_size,
    }


def main() -> None:
    args = parse_args()
    demo_directory = args.demo_directory.resolve()
    output = args.output.resolve()

    if not demo_directory.is_dir():
        raise SystemExit(f"demo directory not found: {demo_directory}")

    files: list[dict[str, object]] = []

    for name in SOURCE_FILES:
        path = demo_directory / name
        if path.is_file():
            files.append(file_entry(path, demo_directory))

    data_directory = demo_directory / "data"
    if data_directory.is_dir():
        for path in sorted(data_directory.rglob("*"), key=lambda item: item.as_posix().lower()):
            if path.is_file() and not path.is_symlink():
                files.append(file_entry(path, demo_directory))

    manifest = {
        "demo": demo_directory.name,
        "files": files,
    }

    output.parent.mkdir(parents=True, exist_ok=True)
    temporary_output = output.with_name(f".{output.name}.tmp")
    temporary_output.write_text(
        json.dumps(manifest, indent=2, ensure_ascii=False) + "\n",
        encoding="utf-8",
    )
    temporary_output.replace(output)


if __name__ == "__main__":
    main()
