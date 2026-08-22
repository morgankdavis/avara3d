#!/usr/bin/env python3

import re
import sys
from pathlib import Path

SECTION_MARKER = re.compile(r"^(?P<indent>\s*)// \[(?P<title>[^\]\r\n]+)\]\s*$")
NAMESPACE_DECL = "namespace a3d::math {"
NAMESPACE_DOC = "/** @brief Mathematical types and utilities used throughout A3D. */ "


def synthetic_doc(line_number: int) -> str:
    return f"/*! @anchor a3d_math_decl_{line_number} */ "


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

    namespace_close_line = max(
        line_number
        for line_number, line in enumerate(lines, start=1)
        if code_before_comment(line.rstrip("\r\n")).strip() == "}"
    )

    in_documented_section = False
    section_open = False
    template_pending = False
    template_body_depth = 0
    output: list[str] = []

    for line_number, line in enumerate(lines, start=1):
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

            output.append(add_prefix(line, synthetic_doc(line_number)))
            template_pending = False
            template_body_depth = max(0, code.count("{") - code.count("}"))
            continue

        section_match = SECTION_MARKER.fullmatch(without_eol)
        if section_match:
            indent = section_match.group("indent")
            title = section_match.group("title")

            if section_open:
                output.append(f"{indent}/// @}}\n")

            output.append(line)
            output.append(f"{indent}/** @name {title} */\n")
            output.append(f"{indent}/// @{{\n")
            in_documented_section = True
            section_open = True
            continue

        if stripped == NAMESPACE_DECL:
            output.append(add_prefix(line, NAMESPACE_DOC))
            continue

        if section_open and line_number == namespace_close_line:
            indent = line[: len(line) - len(line.lstrip(" \t"))]
            output.append(f"{indent}/// @}}\n")
            section_open = False
            output.append(line)
            continue

        if not in_documented_section or not stripped or stripped.startswith("//") or stripped.startswith("#"):
            output.append(line)
            continue

        if stripped.startswith("template<"):
            template_pending = True
            output.append(line)
            continue

        if re.match(r"struct\s+[A-Za-z_]\w*\s*\{", stripped):
            output.append(add_prefix(line, synthetic_doc(line_number)))
            continue

        if code.endswith(";") and stripped != "};":
            output.append(add_prefix(line, synthetic_doc(line_number)))
            continue

        output.append(line)

    sys.stdout.writelines(output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
