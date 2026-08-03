#!/usr/bin/env bash

set -euo pipefail

readonly FORMATTING_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=tools/formatting/_clang-format-common.sh
source "$FORMATTING_DIR/_clang-format-common.sh"

formatter="$(resolve_clang_format)"
cd "$A3D_REPO_ROOT"
load_cpp_files "$@"

run_clang_format_in_batches "$formatter" \
	--dry-run \
	--Werror \
	--style=file \
	--fallback-style=none

echo "Formatting is clean for ${#A3D_CPP_FILES[@]} C/C++ file(s)."
