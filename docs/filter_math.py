#!/usr/bin/env python3

import re
import sys
from pathlib import Path

SECTION_MARKER = re.compile(r"^\s*// \[[^\]\r\n]+\]\s*$")
NAMESPACE_DECL = "namespace a3d::math {"
SYNTHETIC_DOC = "/*! <!-- Doxygen-only: self-documenting declaration. --> */ "
NAMESPACE_DOC = "/** @brief Mathematical types and utilities used throughout A3D. */ "


def add_prefix(line: str, prefix: str) -> str:
    stripped = line.lstrip(" \t")
    indent = line[: len(line) - len(stripped)]
    return f"{indent}{prefix}{stripped}"


def code_before_comment(line: str) -> str:
    return line.split("//", 1)[0].rstrip()


def main() -> int:
    if len(sys.argv) != 2:
        return 2

    path = Path(sys.argv[1])
    lines = path.read_text(encoding="utf-8").splitlines(keepends=True)

    in_documented_section = False
    template_pending = False
    template_body_depth = 0
    output: list[str] = []

    for line in lines:
        without_eol = line.rstrip("\r\n")
        code = code_before_comment(without_eol)
        stripped = code.strip()

        if template_body_depth > 0:
            template_body_depth += code.count("{") - code.count("}")
            output.append(line)
            continue

        if template_pending:
            if not stripped or stripped.startswith("//"):
                output.append(line)
                continue

            output.append(add_prefix(line, SYNTHETIC_DOC))
            template_pending = False
            template_body_depth = max(0, code.count("{") - code.count("}"))
            continue

        if SECTION_MARKER.fullmatch(without_eol):
            in_documented_section = True
            output.append(line)
            continue

        if stripped == NAMESPACE_DECL:
            output.append(add_prefix(line, NAMESPACE_DOC))
            continue

        if not in_documented_section or not stripped or stripped.startswith("//") or stripped.startswith("#"):
            output.append(line)
            continue

        if stripped.startswith("template<"):
            template_pending = True
            output.append(line)
            continue

        if re.match(r"struct\s+[A-Za-z_]\w*\s*\{", stripped):
            output.append(add_prefix(line, SYNTHETIC_DOC))
            continue

        if code.endswith(";") and stripped != "};":
            output.append(add_prefix(line, SYNTHETIC_DOC))
            continue

        output.append(line)

    sys.stdout.writelines(output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
