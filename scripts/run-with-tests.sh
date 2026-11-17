#!/usr/bin/env bash
# Host wrapper for the Docker pre-run gate (same path as make docker-run).
# Requires Docker. Must be run from the repository root.
set -euo pipefail

ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

if [[ ! -f docker-compose.yml || ! -f Dockerfile ]]; then
    echo "run-with-tests.sh: run from the DealerDesk repository root" >&2
    exit 1
fi

exec docker compose run --rm dealerdesk "$@"
