# DealerDesk — Docker build and test

Trusted quality path: compile and run CTest inside Docker. The image pins a
Debian userland with `build-essential` (gcc) and `cmake`/`ctest`, matching the
challenge one-liner `gcc -o dealerdesk *.c -lm` plus our CMake entrypoint.

Host CMake (without Docker) is also supported from the repository root via the
root `Makefile` and the “MODERN BUILD / TEST” section in `README.md`. Use
Docker when you want a reproducible toolchain; use host when cmake is already
installed locally.

Multi-OS GitHub Release binaries (when present) are a CI/CD demo only. They are
not a second quality gate.

GitHub Actions CI (`.github/workflows/ci.yml`) uses `debian:bookworm-slim`,
the same base OS as this repo’s Dockerfile, and runs CMake/CTest on every
pull request to `master` (no re-run on merge push; no docker compose in CI).
Local trusted runs remain `make docker-test` / the pre-run gate.

GitHub Actions CD (`.github/workflows/cd-release.yml`) is a **delivery demo
only**: on push to `master` it may create the next `v2.N` tag, update
`RELEASE.txt` (with `[skip ci]`), and attach **Windows + Linux** binaries to a
GitHub Release (macOS omitted because hosted runner queue stalled the
pipeline). Those artifacts have **no** sanity suite; out-of-Docker use is user
discretion. Docker + CTest remains the trusted quality path. Release notes and
the `RELEASE.txt` modern history share the same disclaimer + changelog text.

To exercise the build **without** merging or tagging: Actions → CD Release
Demo → Run workflow → pick the feature branch → leave `dry_run` enabled. That
uploads Win/Linux build artifacts only (no tag, no Release, no `RELEASE.txt`
commit).

Image build context is filtered by `.dockerignore` (git metadata, host build
dirs, IDE folders, and similar noise stay out of the context).

## Pre-run gate

The image `ENTRYPOINT` runs configure/build if needed, then
`ctest --output-on-failure`, then `exec` of `build-docker/dealerdesk`. Any
failure aborts before the app starts (fail closed).

From the repository root (host), use the local wrapper instead of remembering
compose one-liners:

```bash
./scripts/run-with-tests.sh
make docker-run
```

`make docker-run` calls `scripts/run-with-tests.sh`, which runs
`docker compose run --rm dealerdesk` and reuses the image entrypoint.

Escape hatch (bypass the gate for shell or test-only work):

```bash
make docker-shell
make docker-test
# or: docker compose run --rm --entrypoint bash dealerdesk
```

## Enter the container

From the repository root:

```bash
make docker-shell
# or: docker compose run --rm --entrypoint bash dealerdesk
```

The repo is mounted at `/workspace`. Always run build, test, and the app from
that directory so `data/` resolves.

## Configure, build, test, run

One-shot CTest inside Docker (no app):

```bash
make docker-test
```

Gated app start:

```bash
make docker-run
```

Or interactively inside the container (after `make docker-shell`):

Inside the container use `build-docker/` (not host `build/`). CMakeCache
stores absolute paths; sharing one dir between Mac and the container breaks
configure.

```bash
cmake -S . -B build-docker
cmake --build build-docker
ctest --test-dir build-docker --output-on-failure
./build-docker/dealerdesk
```

`make docker-test` already uses `build-docker/`.

## Host path (no Docker)

Install cmake, then from the repository root:

```bash
make test
make run
```

See `README.md` (MODERN BUILD / TEST) for brew/apt install notes and the
legacy `gcc -o dealerdesk *.c -lm` one-liner.

## BDD terminal suite (host or Docker)

C harness under `BDD/` (features, runner, step definitions). Full instructions:
`BDD/runner/README.md`.

This suite is **not** part of GitHub Actions CI. CI stays CMake/CTest only
(Phase 3 gate). Run the terminal BDD suite locally or in Docker when you want
`@terminal` / PNG proof:

```bash
make bdd          # host: build/dealerdesk + build/bdd_runner (CMake)
make docker-bdd   # same suite in the image (build-docker/)
```

## After mutating sample data

```bash
git checkout -- data
```
