# DealerDesk — delivery task board

This board is **our** priority answer for the Nextlane take-home (part 1:
improve delivery, then part 2: ship `TICKET-142.md`). It is **not** a checklist
imposed by the challenge brief. The brief has no delivery checklist; choosing
what to do first is part of the answer.

See also: `BRIEF.md`, `TICKET-142.md`, `docs/SDLC.md`, `docs/JOURNAL.md`,
`AGENTS.md`. Final write-up: `NOTES.md` (phase 5).

## Rules for this board

- **BDD then TDD** on every behaviour change (`docs/SDLC.md`).
- Gherkin under `features/` = **behaviour / acceptance contract** (what the
  system must do). CTest runs the C tests that verify it. No Gherkin runtime
  runner in this board.
- **One git commit per completed todo** below. Append `docs/JOURNAL.md` with
  date, phase, task id, commit SHA, and a short note.
- **Trusted quality:** Docker + CTest/TDD (+ Docker pre-run gate).
- **Multi-OS GitHub Release binaries** (Windows, macOS, Linux) are a
  **CI/CD demonstration only**. There is **no** sanity-test suite on those
  artifacts. Out-of-Docker builds/downloads/smoke are at the **user’s
  discretion** and do not replace the Docker + CTest gate.
- Do not start fleet (`TICKET-142`) until baseline acceptance tests and the
  gate exist.
- Do not open a PR against the upstream original challenge repository.

## Phase 0 — Project rules and task board

Branch: `feature/docs-delivery-tasks`

- [x] P0-1 Encode challenge ground rules in `AGENTS.md` and `.cursor/rules`
- [x] P0-2 Write this task board (`docs/TASKS.md`)
- [x] P0-3 Create `docs/JOURNAL.md` scaffold and log Phase 0 commits
- [x] P0-4 Point `docs/SDLC.md` at TASKS, JOURNAL, and commit-per-todo
- [ ] P0-5 Align `BRIEF.md` with artifact gaps (no upstream PR; what they look at)

## Phase 1 — Docker + CMake/CTest

Branch: `feature/docker-cmake-ctest`

- [ ] P1-1 Add `Dockerfile` with toolchain, cmake, and ctest
- [ ] P1-2 Add root `CMakeLists.txt` for app (+ tests hook when `BUILD_TESTING`)
- [ ] P1-3 Add compose or documented `docker run` / `docker compose` usage
- [ ] P1-4 Smoke: configure, build, and run a trivial/empty CTest in-container

## Phase 2 — Baseline acceptance BDD + TDD

Branch: `feature/baseline-bdd-tdd`

Order inside the phase: pricing → persistence → quote/sell → stock/customers.

- [ ] P2-1 Add `features/pricing.feature` (retail/trade, clamp, trade-in, goldens)
- [ ] P2-2 Add CTest C tests for pricing scenarios (1:1 titles); green
- [ ] P2-3 Add `features/persistence.feature` + C tests; green
- [ ] P2-4 Add `features/quote_sell.feature` + C tests; green
- [ ] P2-5 Add `features/stock.feature` + C tests; green
- [ ] P2-6 Add `features/customers.feature` + C tests; green
- [ ] P2-7 Full baseline suite green inside Docker

## Phase 3 — Gate + CI/CD release demo

Branch: `feature/ci-cd-gate-release`

- [ ] P3-1 Docker entrypoint / script: CTest must pass before `dealerdesk`
- [ ] P3-2 Local script equivalent for the Docker-centric path
- [ ] P3-3 GitHub Actions CI: build + full CTest on PR and default-branch push
- [ ] P3-4 GitHub Actions CD: on merge to default branch, tag/release with
      Windows, macOS, and Linux executables; release body includes the
      demo-only / no-sanity / user-discretion disclaimer

## Phase 4 — TICKET-142 fleet discount

Branch: `feature/ticket-142-fleet-discount`

- [ ] P4-1 Acceptance scenarios for fleet (Gherkin) + retail/trade regression
- [ ] P4-2 Red CTest coverage for fleet scenarios
- [ ] P4-3 Implement fleet marking and 4% vehicle-price discount line
- [ ] P4-4 Green suite in Docker; non-fleet totals unchanged to the penny;
      existing data loads; existing invoices still display

## Phase 5 — NOTES and submission shape

- [ ] P5-1 Write `NOTES.md` (≤2 pages) from challenge prompts using JOURNAL
- [ ] P5-2 Confirm agent setup artifacts and todo↔SHA traceability in JOURNAL;
      no PR to upstream original

## Ornament cuts only (not this board’s core)

- No Cucumber/Behave runner
- No multi-stage Docker beauty contest
- No Help-screen / README archaeology beyond need
- No product features outside `TICKET-142.md`
- Never market multi-OS release binaries as sanity-tested
