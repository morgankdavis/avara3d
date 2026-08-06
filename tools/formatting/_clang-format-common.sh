#!/usr/bin/env bash

# Shared implementation for format.sh and check-format.sh.

set -euo pipefail

readonly A3D_EXPECTED_CLANG_FORMAT_MAJOR=22
readonly A3D_FORMATTING_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
readonly A3D_FALLBACK_ROOT="$(cd "$A3D_FORMATTING_DIR/../.." && pwd)"

if A3D_REPO_ROOT="$(git -C "$A3D_FALLBACK_ROOT" rev-parse --show-toplevel 2>/dev/null)"; then
	readonly A3D_REPO_ROOT
else
	readonly A3D_REPO_ROOT="$A3D_FALLBACK_ROOT"
fi

resolve_clang_format() {
	local candidate=""

	if [[ -n "${CLANG_FORMAT:-}" ]]; then
		candidate="$CLANG_FORMAT"
	elif command -v "clang-format-${A3D_EXPECTED_CLANG_FORMAT_MAJOR}" >/dev/null 2>&1; then
		candidate="$(command -v "clang-format-${A3D_EXPECTED_CLANG_FORMAT_MAJOR}")"
	elif command -v clang-format >/dev/null 2>&1; then
		candidate="$(command -v clang-format)"
	else
		echo "error: clang-format ${A3D_EXPECTED_CLANG_FORMAT_MAJOR} was not found." >&2
		echo "Set CLANG_FORMAT=/path/to/clang-format or install clang-format ${A3D_EXPECTED_CLANG_FORMAT_MAJOR}." >&2
		return 1
	fi

	local version
	version="$("$candidate" --version 2>&1)"

	if [[ ! "$version" =~ version[[:space:]]+${A3D_EXPECTED_CLANG_FORMAT_MAJOR}([.][0-9]+)* ]]; then
		if [[ "${A3D_ALLOW_CLANG_FORMAT_VERSION_MISMATCH:-0}" != "1" ]]; then
			echo "error: expected clang-format ${A3D_EXPECTED_CLANG_FORMAT_MAJOR}.x; found:" >&2
			echo "  $version" >&2
			echo "Set A3D_ALLOW_CLANG_FORMAT_VERSION_MISMATCH=1 to override deliberately." >&2
			return 1
		fi
	fi

	printf '%s\n' "$candidate"
}

is_cpp_source() {
	case "$1" in
		*.c|*.cc|*.cpp|*.cxx|*.h|*.hh|*.hpp|*.hxx|*.inl|*.ipp)
			return 0
			;;
		*)
			return 1
			;;
	esac
}

is_excluded_path() {
	local path="$1"

	case "$path" in
		lib/external/*|./lib/external/*|"$A3D_REPO_ROOT"/lib/external/*)
			return 0
			;;
		cmake-build-*/*|./cmake-build-*/*|"$A3D_REPO_ROOT"/cmake-build-*/*)
			return 0
			;;
		build/*|./build/*|"$A3D_REPO_ROOT"/build/*)
			return 0
			;;
		out/*|./out/*|"$A3D_REPO_ROOT"/out/*)
			return 0
			;;
		*)
			return 1
			;;
	esac
}

collect_cpp_files() {
	if (( $# > 0 )); then
		local path
		for path in "$@"; do
			if is_excluded_path "$path"; then
				echo "warning: skipping excluded path: $path" >&2
			elif [[ -f "$path" ]] && is_cpp_source "$path"; then
				printf '%s\0' "$path"
			else
				echo "warning: skipping non-C++ file or missing path: $path" >&2
			fi
		done
		return
	fi

	if git -C "$A3D_REPO_ROOT" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
		git -C "$A3D_REPO_ROOT" ls-files -z -- \
			'*.c' '*.cc' '*.cpp' '*.cxx' \
			'*.h' '*.hh' '*.hpp' '*.hxx' \
			'*.inl' '*.ipp' \
			':(exclude,glob)lib/external/**' \
			':(exclude,glob)cmake-build-*/**' \
			':(exclude,glob)build/**' \
			':(exclude,glob)out/**'
	else
		find "$A3D_REPO_ROOT" \
			\( -path "$A3D_REPO_ROOT/lib/external" \
				-o -path "$A3D_REPO_ROOT/cmake-build-*" \
				-o -path "$A3D_REPO_ROOT/build" \
				-o -path "$A3D_REPO_ROOT/out" \) -prune \
			-o -type f \
			\( -name '*.c' -o -name '*.cc' -o -name '*.cpp' -o -name '*.cxx' \
				-o -name '*.h' -o -name '*.hh' -o -name '*.hpp' -o -name '*.hxx' \
				-o -name '*.inl' -o -name '*.ipp' \) -print0
	fi
}

load_cpp_files() {
	A3D_CPP_FILES=()

	local file
	while IFS= read -r -d '' file; do
		A3D_CPP_FILES+=("$file")
	done < <(collect_cpp_files "$@")

	if (( ${#A3D_CPP_FILES[@]} == 0 )); then
		if (( $# > 0 )); then
			echo "No eligible C/C++ files selected."
			return 0
		fi

		echo "No C/C++ files found." >&2
		return 1
	fi
}

run_clang_format_in_batches() {
	local formatter="$1"
	shift

	local batch_size=100
	local start
	for ((start = 0; start < ${#A3D_CPP_FILES[@]}; start += batch_size)); do
		"$formatter" "$@" "${A3D_CPP_FILES[@]:start:batch_size}"
	done
}
