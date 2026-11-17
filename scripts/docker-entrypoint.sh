#!/usr/bin/env bash
# Pre-run gate: configure/build if needed, CTest must pass, then exec dealerdesk.
# Fail closed: any configure/build/ctest failure aborts before the app starts.
set -euo pipefail

cd /workspace

BUILD_DIR="${DEALERDESK_BUILD_DIR:-build-docker}"
BIN="${BUILD_DIR}/dealerdesk"

if [[ ! -f "${BUILD_DIR}/CMakeCache.txt" || ! -x "${BIN}" ]]; then
    cmake -S . -B "${BUILD_DIR}"
    cmake --build "${BUILD_DIR}"
fi

ctest --test-dir "${BUILD_DIR}" --output-on-failure

exec "${BIN}" "$@"
