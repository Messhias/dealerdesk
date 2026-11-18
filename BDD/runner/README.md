# DealerDesk — BDD terminal runner

C harness that drives the real `dealerdesk` binary for scenarios tagged
`@terminal` inside the existing `BDD/features/*.feature` files.

There are no parallel `*_terminal.feature` files. Tag the original Scenario
blocks (same titles CTest already maps 1:1) when the behaviour has an honest
menu path. Invented Scenario titles are not allowed.

Each `@terminal` scenario uses an isolated temp copy of `data/` (same idea as
CTest), asserts stdout / persisted files, and writes per-step PNGs under
`BDD/artifacts/<feature_stem>/`.

Scenarios without `@terminal` stay the CTest behaviour contract only. The
runner ignores them.

Step definitions are split under `BDD/step_definitions/` to match feature
areas (`stock_steps.c`, `customers_steps.c`, …) plus shared
`steps_common.c` / `steps_registry.c`. Every `.c` there is compiled into
`bdd_runner` via `BDD/CMakeLists.txt`.

Not a CI job. Local or Docker via `make bdd` / `make docker-bdd`.

## Host

```bash
make
make bdd
```

## Docker

```bash
make docker-bdd
```

## Binary selection

1. `DEALERDESK_BIN` if set
2. `build-docker/dealerdesk` inside Docker
3. Otherwise `build/dealerdesk`, then `build-docker/dealerdesk`
