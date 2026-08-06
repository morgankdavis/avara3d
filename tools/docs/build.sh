#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"
BUILD_DIR="${A3D_DOCUMENTATION_BUILD_DIR:-${1:-build-documentation}}"

if [[ "${BUILD_DIR}" != /* ]]; then
    BUILD_DIR="${REPO_ROOT}/${BUILD_DIR}"
fi

command -v cmake >/dev/null 2>&1 || {
    echo "error: cmake was not found" >&2
    exit 1
}
command -v doxygen >/dev/null 2>&1 || {
    echo "error: doxygen was not found" >&2
    exit 1
}
command -v dot >/dev/null 2>&1 || {
    echo "error: Graphviz dot was not found" >&2
    exit 1
}

cmake \
    -S "${REPO_ROOT}" \
    -B "${BUILD_DIR}" \
    -G "${A3D_DOCUMENTATION_GENERATOR:-Ninja}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DA3D_BUILD_DOCUMENTATION=ON \
    -DA3D_BUILD_EDITOR=OFF \
    -DA3D_BUILD_TESTS=OFF \
    -DA3D_BUILD_DEMOS=OFF

cmake --build "${BUILD_DIR}" --target a3d-docs

test -f "${BUILD_DIR}/docs/html/index.html"

printf '\nAvara3D API documentation generated:\n  %s\n' \
    "${BUILD_DIR}/docs/html/index.html"
