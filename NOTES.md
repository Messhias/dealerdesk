# DealerDesk — notes

## What changed, and why that order

The brief asks for two things: improve how changes get delivered, then ship
`TICKET-142` through that path. Priorities followed risk to dealership staff.

1. **Rules and a task board** (`AGENTS.md`, `docs/TASKS.md`, `docs/JOURNAL.md`)
   so agents and humans share the same constraints (no upstream PR, commit as
   you go, non-fleet totals stay correct to the penny).
2. **Docker + CMake/CTest** as the trusted build/test path, then **baseline
   BDD/TDD** for retail/trade pricing, persistence, stock, customers, and
   quote/sell before touching fleet math.
3. **Delivery gate and CI/CD demo**: Docker pre-run gate, GitHub Actions CTest,
   multi-OS Release binaries marked as demonstration only (no sanity suite on
   those artifacts).
4. **Fleet (TICKET-142)** through the same BDD → TDD loop, with retail/trade
   regression scenarios kept green.
5. **Terminal `@terminal` E2E** against the real binary with isolated temp
   `data/` and per-step PNGs, so menu behaviour is visible proof on top of
   CTest seams.

Product behaviour outside the ticket was left alone on purpose. Pricing truth
stayed in `calc.c` (`calc_quote`), not README comments.

## Working with agents

Coding agents drafted scenarios, tests, and C. Humans decided scope, ran
manual checks, and kept the board honest.

One miss that mattered: early Phase 5 work invented extra `@terminal` scenario
titles and parallel `*_terminal.feature` files instead of tagging the Gherkin
that already existed. That looked green locally but did not prove the original
acceptance contract on the UI. It was caught by reading the feature files
against the ask (“put `@terminal` on the existing scenarios”), deleting the
invented ones, and wiring step definitions to the real phrases. A related miss
was silencing Rider warnings with pragmas; the fix was aligning `StepFn` with
`const StepWorld *` and moving mutable paths onto `Console`, not hiding
inspections.

Another earlier miss (persistence): a first attempt added a production
`db_set_data_dir` seam so tests could point at a temp tree. Wrong shape for
“staff must not notice.” Caught in review; isolation moved to `chdir` into a
private temp `data/` with zero production persistence API change
(`docs/JOURNAL.md`, P2-3).

Steering that worked: keep Gherkin as the behaviour contract, CTest 1:1 on
scenario titles, and refuse fake terminal coverage for pure calc/file seams
until those phrases could be driven honestly through menus or seeded files.

## Assumptions

- Sample `data/*.dat` and `calc_quote` are the money source of truth when docs
  disagree.
- Fleet discount is 4% off list, then salesman on what remains; fees stay fixed.
- Multi-OS GitHub Release binaries demonstrate CD only; trusted quality is
  Docker + CTest (+ Docker pre-run gate). `make bdd` is additional UI proof,
  not a CI job.
- App version lives only in `version.h`; CD keeps `RELEASE.txt` in sync.
- Submission is this repository (`Messhias/dealerdesk`), not a PR to the
  upstream challenge repo.

## Next with more time

- Tighten JOURNAL SHAs for squash merges when the board SHA and GitHub merge
  commit differ.
- More honest PNG policy (screenshot only when the screen changes) and a short
  reviewer checklist in `BDD/runner/README.md`.
- Optional host smoke around Release artifacts, still without calling them
  production-certified.
- Small UX polish for fleet type entry if staff feedback asks for it, behind
  the same BDD/TDD gate.
