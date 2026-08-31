#!/usr/bin/env bash

set -euo pipefail

SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
REPO_ROOT="$(cd -- "${SCRIPT_DIR}/../.." && pwd)"

absolute_from_repo() {
    local path="$1"
    if [[ "${path}" == /* ]]; then
        printf '%s\n' "${path}"
    else
        printf '%s/%s\n' "${REPO_ROOT}" "${path}"
    fi
}

SITE_SOURCE_DIR="${REPO_ROOT}/website"
DEMO_NAME="${A3D_WEBSITE_DEMO_NAME:-janus}"
WEB_BUILD_DIR="$(absolute_from_repo "${A3D_WEB_BUILD_DIR:-${1:-build-web-release}}")"
OUTPUT_DIR="$(absolute_from_repo "${A3D_WEBSITE_OUTPUT_DIR:-${2:-build-website}}")"
DEMO_SOURCE_DIR="${REPO_ROOT}/demos/${DEMO_NAME}"
REQUIRE_DEMO="${A3D_REQUIRE_WEB_DEMO:-0}"
REQUIRE_DOCUMENTATION="${A3D_REQUIRE_DOCUMENTATION:-0}"
TEMP_DIR="${OUTPUT_DIR}.tmp.$$"

cleanup() {
    rm -rf -- "${TEMP_DIR}"
}
trap cleanup EXIT

find_demo_directory() {
    if [[ -n "${A3D_WEB_DEMO_DIR:-}" ]]; then
        local requested
        requested="$(absolute_from_repo "${A3D_WEB_DEMO_DIR}")"

        if [[ -f "${requested}/${DEMO_NAME}.js" &&
              -f "${requested}/${DEMO_NAME}.wasm" ]]; then
            printf '%s\n' "${requested}"
            return
        fi

        return 1
    fi

    local candidates=(
        "${WEB_BUILD_DIR}/packaged/avara3d/web/release/demos/${DEMO_NAME}"
        "${WEB_BUILD_DIR}/packaged/avara3d/web/debug/demos/${DEMO_NAME}"
        "${WEB_BUILD_DIR}/demos/${DEMO_NAME}"
    )

    local candidate
    for candidate in "${candidates[@]}"; do
        if [[ -f "${candidate}/${DEMO_NAME}.js" &&
              -f "${candidate}/${DEMO_NAME}.wasm" ]]; then
            printf '%s\n' "${candidate}"
            return
        fi
    done

    return 1
}

find_documentation_directory() {
    if [[ -n "${A3D_DOCUMENTATION_DIR:-}" ]]; then
        local requested
        requested="$(absolute_from_repo "${A3D_DOCUMENTATION_DIR}")"
        if [[ -f "${requested}/index.html" ]]; then
            printf '%s\n' "${requested}"
            return
        fi
        return 1
    fi

    local candidates=(
        "${REPO_ROOT}/build-documentation/docs/html"
        "${REPO_ROOT}/gitlab-build-documentation/docs/html"
    )

    local candidate
    for candidate in "${candidates[@]}"; do
        if [[ -f "${candidate}/index.html" ]]; then
            printf '%s\n' "${candidate}"
            return
        fi
    done

    return 1
}

if [[ ! -d "${SITE_SOURCE_DIR}" ]]; then
    printf 'error: website source directory not found: %s\n' "${SITE_SOURCE_DIR}" >&2
    exit 1
fi

if [[ ! -d "${DEMO_SOURCE_DIR}" ]]; then
    printf 'error: demo source directory not found: %s\n' "${DEMO_SOURCE_DIR}" >&2
    exit 1
fi

rm -rf -- "${TEMP_DIR}"
mkdir -p -- "${TEMP_DIR}"
cp -a -- "${SITE_SOURCE_DIR}/." "${TEMP_DIR}/"

# Local development overlays are symlinks inside website/. They are replaced
# with copied build/source files in the assembled artifact.
rm -rf -- "${TEMP_DIR}/demo" "${TEMP_DIR}/source"
rm -f -- \
    "${TEMP_DIR}/.gitignore" \
    "${TEMP_DIR}/README.md" \
    "${TEMP_DIR}/build-info.json" \
    "${TEMP_DIR}/source-manifest.json"
mkdir -p -- "${TEMP_DIR}/demo" "${TEMP_DIR}/source"

find "${DEMO_SOURCE_DIR}" -maxdepth 1 -type f \
    \( -name '*.cc' -o -name '*.h' \) \
    -exec cp -- {} "${TEMP_DIR}/source/" \;

if [[ -d "${DEMO_SOURCE_DIR}/data" ]]; then
    cp -a -- "${DEMO_SOURCE_DIR}/data" "${TEMP_DIR}/source/data"
fi

python3 \
    "${SCRIPT_DIR}/generate-source-manifest.py" \
    "${DEMO_SOURCE_DIR}" \
    "${TEMP_DIR}/source-manifest.json"

if DEMO_DIRECTORY="$(find_demo_directory)"; then
    cp -a -- "${DEMO_DIRECTORY}/." "${TEMP_DIR}/demo/"
    printf 'Included WebAssembly demo: %s\n' "${DEMO_DIRECTORY}"
else
    printf 'warning: no installed WebAssembly demo found under %s\n' "${WEB_BUILD_DIR}" >&2
    printf '         the site will show its unavailable-demo state\n' >&2

    if [[ "${REQUIRE_DEMO}" == "1" ]]; then
        exit 1
    fi
fi

DOCUMENTATION_INCLUDED=false
if DOCUMENTATION_DIRECTORY="$(find_documentation_directory)"; then
    rm -rf -- "${TEMP_DIR}/api"
    mkdir -p -- "${TEMP_DIR}/api"
    cp -a -- "${DOCUMENTATION_DIRECTORY}/." "${TEMP_DIR}/api/"
    DOCUMENTATION_INCLUDED=true
    printf 'Included API documentation: %s\n' "${DOCUMENTATION_DIRECTORY}"
else
    printf 'warning: generated API documentation was not found\n' >&2
    printf '         the site will retain its API placeholder page\n' >&2

    if [[ "${REQUIRE_DOCUMENTATION}" == "1" ]]; then
        exit 1
    fi
fi

GIT_COMMIT="${CI_COMMIT_SHA:-$(git -C "${REPO_ROOT}" rev-parse HEAD 2>/dev/null || printf 'unknown')}"
GIT_BRANCH="${CI_COMMIT_BRANCH:-$(git -C "${REPO_ROOT}" rev-parse --abbrev-ref HEAD 2>/dev/null || printf 'unknown')}"
BUILD_TIME="$(date -u +'%Y-%m-%dT%H:%M:%SZ')"

cat > "${TEMP_DIR}/build-info.json" <<JSON
{
    "branch": "${GIT_BRANCH}",
    "commit": "${GIT_COMMIT}",
    "assembledAt": "${BUILD_TIME}",
    "demo": "${DEMO_NAME}",
    "apiDocumentation": ${DOCUMENTATION_INCLUDED}
}
JSON

rm -rf -- "${OUTPUT_DIR}"
mv -- "${TEMP_DIR}" "${OUTPUT_DIR}"
trap - EXIT

printf '\nWebsite assembled successfully:\n  %s\n' "${OUTPUT_DIR}"
