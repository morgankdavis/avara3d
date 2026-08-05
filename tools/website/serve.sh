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
SITE_DIR="${REPO_ROOT}/website"
DEMO_NAME="${A3D_WEBSITE_DEMO_NAME:-001-physics-sandbox}"
PORT="${A3D_WEBSITE_PORT:-${1:-8000}}"
WEB_BUILD_DIR="$(absolute_from_repo "${A3D_WEB_BUILD_DIR:-${2:-build-web-release}}")"
DEMO_SOURCE_DIR="${REPO_ROOT}/demos/${DEMO_NAME}"
DEMO_LINK="${SITE_DIR}/demo"
SOURCE_LINK="${SITE_DIR}/source"
BUILD_INFO_FILE="${SITE_DIR}/build-info.json"

find_demo_directory() {
    if [[ -n "${A3D_WEB_DEMO_DIR:-}" ]]; then
        absolute_from_repo "${A3D_WEB_DEMO_DIR}"
        return
    fi

    local candidates=(
        "${WEB_BUILD_DIR}/packaged/avara3d/web/release/demos/${DEMO_NAME}"
        "${WEB_BUILD_DIR}/packaged/avara3d/web/debug/demos/${DEMO_NAME}"
        "${WEB_BUILD_DIR}/demos/${DEMO_NAME}"
    )

    local candidate
    for candidate in "${candidates[@]}"; do
        if [[ -f "${candidate}/${DEMO_NAME}.js" && -f "${candidate}/${DEMO_NAME}.wasm" ]]; then
            printf '%s\n' "${candidate}"
            return
        fi
    done

    return 1
}

SERVER_PID=""

cleanup() {
    rm -rf -- "${DEMO_LINK}" "${SOURCE_LINK}"
    rm -f -- "${BUILD_INFO_FILE}"
}

stop_server() {
    if [[ -n "${SERVER_PID}" ]]; then
        kill "${SERVER_PID}" 2>/dev/null || true
        wait "${SERVER_PID}" 2>/dev/null || true
    fi
    exit 0
}

trap cleanup EXIT
trap stop_server INT TERM

if [[ ! -d "${SITE_DIR}" ]]; then
    printf 'error: website source directory not found: %s\n' "${SITE_DIR}" >&2
    exit 1
fi

if [[ ! -d "${DEMO_SOURCE_DIR}" ]]; then
    printf 'error: demo source directory not found: %s\n' "${DEMO_SOURCE_DIR}" >&2
    exit 1
fi

rm -rf -- "${DEMO_LINK}" "${SOURCE_LINK}"
ln -s -- "${DEMO_SOURCE_DIR}" "${SOURCE_LINK}"

if DEMO_DIRECTORY="$(find_demo_directory)"; then
    ln -s -- "${DEMO_DIRECTORY}" "${DEMO_LINK}"
    printf 'Demo build: %s\n' "${DEMO_DIRECTORY}"
else
    printf 'warning: no installed WebAssembly demo found under %s\n' "${WEB_BUILD_DIR}" >&2
    printf '         the page will remain usable and show the missing-build state\n' >&2
fi

GIT_COMMIT="$(git -C "${REPO_ROOT}" rev-parse --short=12 HEAD 2>/dev/null || printf 'unknown')"
GIT_BRANCH="$(git -C "${REPO_ROOT}" rev-parse --abbrev-ref HEAD 2>/dev/null || printf 'unknown')"

cat > "${BUILD_INFO_FILE}" <<JSON
{
    "branch": "${GIT_BRANCH}",
    "commit": "${GIT_COMMIT}",
    "assembledAt": "local",
    "demo": "${DEMO_NAME}"
}
JSON

printf 'Serving tracked website source at http://127.0.0.1:%s/\n' "${PORT}"
printf 'HTML/CSS/JS changes appear on normal browser refresh. Press Ctrl+C to stop.\n\n'

python3 - "${SITE_DIR}" "${PORT}" <<'PY' &
from __future__ import annotations

import functools
import http.server
import mimetypes
import socketserver
import sys
from pathlib import Path

site_directory = Path(sys.argv[1]).resolve()
port = int(sys.argv[2])

mimetypes.add_type("application/wasm", ".wasm")
mimetypes.add_type("text/javascript; charset=utf-8", ".js")
mimetypes.add_type("application/octet-stream", ".data")

class NoCacheHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self) -> None:
        self.send_header("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0")
        self.send_header("Pragma", "no-cache")
        self.send_header("Expires", "0")
        super().end_headers()

    def log_message(self, format: str, *args: object) -> None:
        sys.stderr.write("[website] " + format % args + "\n")

class ReusableThreadingServer(socketserver.ThreadingTCPServer):
    allow_reuse_address = True
    daemon_threads = True

handler = functools.partial(NoCacheHandler, directory=str(site_directory))

with ReusableThreadingServer(("127.0.0.1", port), handler) as server:
    try:
        server.serve_forever()
    except KeyboardInterrupt:
        pass
PY

SERVER_PID="$!"
wait "${SERVER_PID}"
