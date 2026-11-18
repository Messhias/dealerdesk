# DealerDesk — BDD terminal runner

C harness under `BDD/` that drives the built `dealerdesk` binary through a
terminal session. Gherkin lives in `BDD/features/`; step definitions in
`BDD/step_definitions/`; this directory is the runner.

Scenarios tagged `@terminal` run here. Other scenarios in `BDD/features/` are
the acceptance contract verified by unit/seam CTest under `tests/`.

Runs on host or Docker. Needs a Unix-like terminal session (Linux/macOS, or
Linux via Docker).

## Host

Needs a local C compiler, zlib headers (`zlib.h`), and PTY headers
(`pty.h` / `util.h`), plus link libs `-lz` and `-lutil`. `make bdd` checks
these and prints what is missing; it does not install packages.

```bash
make
make bdd
```

If `make docker-bdd` ran earlier, it writes `BDD/runner/bdd_runner.linux` and
does not overwrite the host runner. If an old Linux `bdd_runner` is still on
the mount, `make bdd` removes it and rebuilds for the host.

## Docker

```bash
make docker-bdd
```

Image includes `zlib1g-dev`. First run rebuilds the image when needed.

## Binary selection

1. `DEALERDESK_BIN` if set
2. `build-docker/dealerdesk` inside Docker (`/.dockerenv` or `DEALERDESK_BDD_DOCKER=1`)
3. Otherwise `build/dealerdesk`, then `build-docker/dealerdesk`

## Layout

| Path | Role |
|------|------|
| `BDD/features/` | All Gherkin (CTest contract + `@terminal` flows) |
| `BDD/runner/` | Feature parser, console session, screenshot |
| `BDD/step_definitions/` | Step implementations |
| `BDD/artifacts/` | PNG screenshots for fleet happy paths |
