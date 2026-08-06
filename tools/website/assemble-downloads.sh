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

CHANNEL="${A3D_DOWNLOAD_CHANNEL:-${CI_COMMIT_BRANCH:-}}"

case "${CHANNEL}" in
    stable)
        PUBLIC_LABEL="Stable"
        ;;
    testing)
        PUBLIC_LABEL="Preview"
        ;;
    *)
        printf 'error: A3D_DOWNLOAD_CHANNEL must be stable or testing\n' >&2
        exit 1
        ;;
esac

LINUX_INPUT_DIR="$(
    absolute_from_repo \
        "${A3D_DOWNLOAD_LINUX_DIR:-avara3d/linux/release}"
)"

MACOS_INPUT_DIR="$(
    absolute_from_repo \
        "${A3D_DOWNLOAD_MACOS_DIR:-avara3d/macos/release}"
)"

WINDOWS_INPUT_DIR="$(
    absolute_from_repo \
        "${A3D_DOWNLOAD_WINDOWS_DIR:-avara3d/windows/release}"
)"

OUTPUT_DIR="$(
    absolute_from_repo \
        "${A3D_DOWNLOAD_OUTPUT_DIR:-build-downloads}"
)"

TEMP_DIR="${OUTPUT_DIR}.tmp.$$"

cleanup() {
    rm -rf -- "${TEMP_DIR}"
}

trap cleanup EXIT

for command in tar zip python3; do
    if ! command -v "${command}" >/dev/null 2>&1; then
        printf 'error: required command not found: %s\n' "${command}" >&2
        exit 1
    fi
done

for input_dir in \
        "${LINUX_INPUT_DIR}" \
        "${MACOS_INPUT_DIR}" \
        "${WINDOWS_INPUT_DIR}"; do

    if [[ ! -d "${input_dir}" ]]; then
        printf 'error: release input directory not found: %s\n' \
            "${input_dir}" >&2
        exit 1
    fi
done

PIPELINE_IID="${CI_PIPELINE_IID:-local}"

FULL_COMMIT="$(
    if [[ -n "${CI_COMMIT_SHA:-}" ]]; then
        printf '%s\n' "${CI_COMMIT_SHA}"
    else
        git -C "${REPO_ROOT}" rev-parse HEAD 2>/dev/null ||
            printf 'unknown\n'
    fi
)"

SHORT_COMMIT="$(
    if [[ -n "${CI_COMMIT_SHORT_SHA:-}" ]]; then
        printf '%s\n' "${CI_COMMIT_SHORT_SHA}"
    else
        git -C "${REPO_ROOT}" rev-parse --short=12 HEAD 2>/dev/null ||
            printf 'unknown\n'
    fi
)"

BRANCH="${CI_COMMIT_BRANCH:-${CHANNEL}}"
BUILD_TIME="${CI_PIPELINE_CREATED_AT:-$(date -u +'%Y-%m-%dT%H:%M:%SZ')}"

LINUX_FILENAME="avara3d-linux-x86_64-${CHANNEL}-${PIPELINE_IID}.tar.gz"
MACOS_FILENAME="avara3d-macos-universal-${CHANNEL}-${PIPELINE_IID}.zip"
WINDOWS_FILENAME="avara3d-windows-x86_64-${CHANNEL}-${PIPELINE_IID}.zip"

rm -rf -- "${TEMP_DIR}"
mkdir -p -- \
    "${TEMP_DIR}/output" \
    "${TEMP_DIR}/linux/avara3d" \
    "${TEMP_DIR}/macos/avara3d" \
    "${TEMP_DIR}/windows/avara3d"

cp -a -- \
    "${LINUX_INPUT_DIR}/." \
    "${TEMP_DIR}/linux/avara3d/"

cp -a -- \
    "${MACOS_INPUT_DIR}/." \
    "${TEMP_DIR}/macos/avara3d/"

cp -a -- \
    "${WINDOWS_INPUT_DIR}/." \
    "${TEMP_DIR}/windows/avara3d/"

tar \
    -C "${TEMP_DIR}/linux" \
    -czf "${TEMP_DIR}/output/${LINUX_FILENAME}" \
    avara3d

(
    cd -- "${TEMP_DIR}/macos"

    zip \
        -q \
        -r \
        -y \
        "${TEMP_DIR}/output/${MACOS_FILENAME}" \
        avara3d
)

(
    cd -- "${TEMP_DIR}/windows"

    zip \
        -q \
        -r \
        "${TEMP_DIR}/output/${WINDOWS_FILENAME}" \
        avara3d
)

export A3D_MANIFEST_OUTPUT_DIR="${TEMP_DIR}/output"
export A3D_MANIFEST_CHANNEL="${CHANNEL}"
export A3D_MANIFEST_PUBLIC_LABEL="${PUBLIC_LABEL}"
export A3D_MANIFEST_BRANCH="${BRANCH}"
export A3D_MANIFEST_COMMIT="${FULL_COMMIT}"
export A3D_MANIFEST_SHORT_COMMIT="${SHORT_COMMIT}"
export A3D_MANIFEST_PIPELINE_IID="${PIPELINE_IID}"
export A3D_MANIFEST_PIPELINE_URL="${CI_PIPELINE_URL:-}"
export A3D_MANIFEST_BUILD_TIME="${BUILD_TIME}"
export A3D_MANIFEST_LINUX_FILE="${LINUX_FILENAME}"
export A3D_MANIFEST_MACOS_FILE="${MACOS_FILENAME}"
export A3D_MANIFEST_WINDOWS_FILE="${WINDOWS_FILENAME}"

python3 <<'PY'
import hashlib
import json
import os
from pathlib import Path

output_dir = Path(os.environ["A3D_MANIFEST_OUTPUT_DIR"])

package_specs = {
    "linux": {
        "label": "Linux",
        "architecture": "x86-64",
        "format": "tar.gz",
        "filename": os.environ["A3D_MANIFEST_LINUX_FILE"],
    },
    "macos": {
        "label": "macOS",
        "architecture": "Universal",
        "format": "zip",
        "filename": os.environ["A3D_MANIFEST_MACOS_FILE"],
    },
    "windows": {
        "label": "Windows",
        "architecture": "x86-64",
        "format": "zip",
        "filename": os.environ["A3D_MANIFEST_WINDOWS_FILE"],
    },
}

checksum_lines = []

for package in package_specs.values():
    package_path = output_dir / package["filename"]

    digest = hashlib.sha256()
    with package_path.open("rb") as package_file:
        for block in iter(lambda: package_file.read(1024 * 1024), b""):
            digest.update(block)

    package["size"] = package_path.stat().st_size
    package["sha256"] = digest.hexdigest()

    checksum_lines.append(
        f'{package["sha256"]}  {package["filename"]}'
    )

pipeline_iid = os.environ["A3D_MANIFEST_PIPELINE_IID"]
pipeline_value = (
    int(pipeline_iid)
    if pipeline_iid.isdigit()
    else None
)

manifest = {
    "schemaVersion": 1,
    "channel": os.environ["A3D_MANIFEST_CHANNEL"],
    "publicLabel": os.environ["A3D_MANIFEST_PUBLIC_LABEL"],
    "branch": os.environ["A3D_MANIFEST_BRANCH"],
    "commit": os.environ["A3D_MANIFEST_COMMIT"],
    "shortCommit": os.environ["A3D_MANIFEST_SHORT_COMMIT"],
    "pipelineIid": pipeline_value,
    "pipelineUrl": os.environ["A3D_MANIFEST_PIPELINE_URL"] or None,
    "builtAt": os.environ["A3D_MANIFEST_BUILD_TIME"],
    "packages": package_specs,
}

with (output_dir / "manifest.json").open("w", encoding="utf-8") as file:
    json.dump(manifest, file, indent=4)
    file.write("\n")

with (output_dir / "SHA256SUMS").open("w", encoding="utf-8") as file:
    file.write("\n".join(checksum_lines))
    file.write("\n")
PY

mkdir -p -- "$(dirname -- "${OUTPUT_DIR}")"
rm -rf -- "${OUTPUT_DIR}"
mv -- "${TEMP_DIR}/output" "${OUTPUT_DIR}"

rm -rf -- "${TEMP_DIR}"
trap - EXIT

printf '\nDownload bundle assembled successfully:\n  %s\n' \
    "${OUTPUT_DIR}"

find "${OUTPUT_DIR}" \
    -maxdepth 1 \
    -type f \
    -printf '  %f\n' |
    sort

