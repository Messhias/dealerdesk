# DealerDesk — delivery task board

This document is the **task specification** for our take-home work. It answers
**what** we will do, **why**, and **how** (at planning level). It is not
implementation yet.

It is **our** priority answer for the Nextlane brief (part 1: improve how
changes get delivered; part 2: ship `TICKET-142.md` through that path). The
brief has **no** delivery checklist; choosing what to do first is part of the
answer. This board records that choice.

| Related doc | Role |
|-------------|------|
| `BRIEF.md` | Challenge envelope |
| `TICKET-142.md` | Fleet feature (part 2) |
| `docs/SYSTEM.md` | Current behaviour truth |
| `docs/SDLC.md` | BDD → TDD loop |
| `docs/JOURNAL.md` | Todo ↔ commit SHA log (filled while executing) |
| `docs/DOCKER.md` | Written when Phase 1 is executed |
| `ISSUES.MD` | Local issue notes (linked from phase follow-ups) |
| `AGENTS.md` | Project rules for agents |
| `NOTES.md` | Final ≤2-page write-up (Phase 6) |

## How to use this board

1. Execute **one phase per git branch**, merge to default branch, then start the next.
2. Each checkbox todo below = **one dedicated commit** when executed. Append a
   row to `docs/JOURNAL.md` (date, phase, task id, SHA, short note).
3. Do **not** start Phase 4 (fleet) until Phases 1–3 exist and baseline tests +
   gate are green.
4. Do **not** start Phase 5 (E2E) until Phase 4 fleet acceptance is green.
5. Do **not** start Phase 6 (NOTES) until Phase 5 **Done when** is true,
   including follow-ups P5-7..P5-13 (P5-1..P5-6 alone is not enough).
6. Do **not** open a PR against the upstream original challenge repository.

## Standing decisions (apply to every phase)

| Topic | Decision |
|-------|----------|
| Order | Harden delivery first, then fleet through that path |
| BDD | Gherkin in `BDD/features/` = **behaviour / acceptance contract** (PO language → what the system must do). Not an “implementation spec”. |
| TDD | C tests via CMake/CTest verify those scenarios 1:1 (stable scenario titles ↔ test names). |
| Gherkin runner | **In scope in Phase 5** (E2E against the real binary + terminal PNG proof). Features stay the acceptance contract; CTest/TDD remains the unit/seam gate. |
| Trusted quality | **Docker + CTest/TDD** (+ Docker pre-run gate). Phase 5 E2E is additional proof, not a replacement. |
| Multi-OS releases | GitHub Release binaries for Windows / macOS / Linux are a **CI/CD demonstration only**. **No** sanity-test suite on those artifacts. Out-of-Docker use is **user discretion**. Never call them production-certified. |
| Staff impact | Apart from intentional fleet behaviour, quotes/invoices stay correct **to the penny**. |
| Ambiguity | Reasonable call + write it down (JOURNAL / NOTES). |
| Ornament cuts | No Docker beauty contest, no Help/README archaeology, no product features outside `TICKET-142.md`. Phase 5 runner is intentional thin E2E, not a Cucumber showcase. |
| App version | In-app banner version lives only in `version.h` (`DEALERDESK_VERSION`). Do **not** hardcode version literals in `screen.c` or elsewhere. CD bumps `version.h` with `RELEASE.txt` on each master release so tag, Release notes, and binary header match. Manual bump only via an explicit release task. |

---

## Phase 0 — Project rules and task board (documentation)

**Branch:** `feature/docs-delivery-tasks` (and follow-ups for doc refinements)

**Why this phase exists:** Agents and humans need shared rules, a written
priority story, and traceability before any product or infra code lands.

| ID | Status | What | Why | How (when executing) |
|----|--------|------|-----|----------------------|
| P0-1 | [x] | Encode challenge ground rules in `AGENTS.md` + `.cursor/rules` | Same constraints in every agent session | Commit rules: AI expected, stack free, staff-invisible, commit-as-you-go, ambiguity→write down, Docker trust vs release demo, commit-per-todo |
| P0-2 | [x] | Create this task board | Priorities are the answer; must be explicit | Checklist + (this revision) what/why/how per phase |
| P0-3 | [x] | Create `docs/JOURNAL.md` scaffold | 100% todo↔SHA traceability for NOTES | Entry table; fill as commits land |
| P0-4 | [x] | Point `docs/SDLC.md` at TASKS / JOURNAL / commit-per-todo | Discoverable from the delivery loop | Short harden-delivery bullets |
| P0-5 | [x] | Align `BRIEF.md` with artifact gaps | Match official challenge (no upstream PR; what they look at) | Add submission/review wording without inventing product behaviour |
| P0-6 | [x] | Expand this board with what / why / how (this change) | Checklist alone is not enough to execute later without re-debating | Rewrite `docs/TASKS.md` as the full task specification; docs only |

---

## Phase 1 — Docker + CMake / CTest

**Branch:** `feature/docker-cmake-ctest`

**Why this phase exists:** The brief calls out no reliable build. Docker pins
toolchain and OS so legacy C compile/run/test does not depend on host arch.
CMake/CTest become the canonical build and test entrypoints inside that image.

**Out of scope here:** Product behaviour, real acceptance tests (Phase 2),
fleet, GHA release.

| ID | Status | What | Why | How (when executing) |
|----|--------|------|-----|----------------------|
| P1-1 | [x] | Add `Dockerfile` (build-essential, cmake/ctest) | Controllable Linux userland for legacy build | Debian-based image; `WORKDIR` = repo; no product change |
| P1-2 | [x] | Add root `CMakeLists.txt` for `dealerdesk` + `BUILD_TESTING` hook | Replace ad-hoc `gcc *.c` as the supported path in Docker | `include(CTest)`; `add_subdirectory(tests)` when testing on; link `-lm` |
| P1-3 | [x] | Add `docker-compose.yml` and/or `docs/DOCKER.md` | Repeatable “shell in, build, test, run” | `docker compose run --rm … bash`; document cmake/ctest/dealerdesk commands |
| P1-4 | [x] | Smoke: configure, build, trivial CTest green **in container** | Prove the pipeline before real scenarios | Placeholder `test_smoke` (or equivalent) returning 0; journal the proof |

**Done when:** Anyone can enter Docker, build, run CTest smoke, run
`dealerdesk` against `data/` without host-toolchain lottery.

---

## Phase 2 — Baseline acceptance BDD + TDD

**Branch:** `feature/baseline-bdd-tdd`

**Why this phase exists:** The brief calls out no tests and untrusted docs.
Before changing pricing for fleet, **current** behaviour (retail / trade only)
must be locked with acceptance scenarios and green CTest. Staff must not
notice non-fleet changes later.

**Order inside the phase (efficiency, not scope cut):** pricing docs →
pricing feature → persistence → quote/sell → stock → customers → full suite
in Docker.

**Truth:** `docs/SYSTEM.md` + sources + `data/*.dat`. Pricing = `calc_quote` in
`calc.c` (ignore wrong comments in headers).

| ID | Status | What | Why | How (when executing) |
|----|--------|------|-----|----------------------|
| P2-0 | [x] | Plain-language pricing in `docs/SYSTEM.md` | Money rules must be readable before BDD/TDD | Expand Pricing with bakery-level steps, `rnd2` in words, salesman 0–15, three worked sample invoices, screen vs saved invoice; docs only |
| P2-1 | [x] | `BDD/features/pricing.feature` | Money is the highest-risk area | Gherkin for retail/trade 10%, salesman clamp 0–15, trade-in, `rnd2`, goldens aligned with sample invoices |
| P2-2 | [x] | CTest C tests for pricing; green | Executable proof of P2-1 | Unit-test `rnd2` / `calc_quote`; map scenario titles 1:1 |
| P2-3 | [x] | `BDD/features/persistence.feature` + C tests; green | Flat files are the DB; regressions break production data | Load skip `#`/empty; round-trip; missing-file defaults; CTest `chdir` into private temp tree with its own `data/` (no production `dbfile` change) |
| P2-4 | [x] | `BDD/features/quote_sell.feature` + C tests; green | Quote/sell encode business rules and invoice packing | Sold blocked; confirm/cancel sell; `disc` sum; `fees = 205`; prefer seams, stdin only if required |
| P2-5 | [x] | `BDD/features/stock.feature` + C tests; green | Stock mutations are daily dealer ops | Add/find/price/reserve rules from `main.c` |
| P2-6 | [x] | `BDD/features/customers.feature` + C tests; green | Customer type drives discounts | Add/find; R/T only (no fleet yet) |
| P2-7 | [x] | Full baseline suite green inside Docker | Trusted gate input for Phase 3 | `ctest` in-container; restore `data/` after mutating checks |

**Done when:** Baseline acceptance suite is green in Docker and documents
current behaviour only (types `R` / `T`).

---

## Phase 3 — Pre-run gate + CI/CD release demo

**Branch:** `feature/ci-cd-gate-release`

**Why this phase exists:** The brief calls out no CI and nervous hand
releases. We want fail-closed tests before trusting a run, CI on every PR, and
a **demo** of tag → multi-OS artifacts (not a second quality gate).

| ID | Status | What | Why | How (when executing) |
|----|--------|------|-----|----------------------|
| P3-1 | [x] | Docker entrypoint: CTest must pass before `dealerdesk` | Trusted pre-run gate | Script: build if needed → `ctest --output-on-failure` → `exec` binary; fail closed |
| P3-2 | [x] | Local script for the same Docker-centric path | Same habit outside compose one-liners | e.g. `scripts/run-with-tests.sh` documented in `docs/DOCKER.md` |
| P3-3 | [x] | GitHub Actions CI: build + full CTest on PR to default branch | Reviewers see green TDD; merge not “happy” otherwise | Debian Bookworm container job; PR-only (no re-run on master push) — see [ISSUE-002](../ISSUES.MD#issue-002--cd-github-release-failed-without-git-checkout-ci-re-ran-on-master-merge) |
| P3-4 | [x] | GHA CD: merge to default branch → version tag + Win/macOS/Linux executables | Show full delivery loop | Release body **must** state: demo only; **no** sanity suite on those binaries; out-of-Docker use is user discretion; Docker+CTest remains trusted |
| P3-5 | [x] | Each complete CD release updates `RELEASE.txt` + Release notes | Reviewers see what shipped without digging git alone | Baseline **2.3**; first automated tag **`v2.4`** then `v2.5`…; workflow generates changelog since previous tag, rewrites `Last release:` + modern history section, puts the same notes in the GitHub Release body (with P3-4 disclaimer) |

**Done when:** Gate blocks failed tests in Docker; CI runs TDD; release
workflow exists with the disclaimer everywhere it ships; `RELEASE.txt`
and the GitHub Release body record each automated release from 2.4 onward.

### Phase 3 follow-ups (issues)

| Issue | Status | Where | Note |
|-------|--------|-------|------|
| [ISSUE-002](../ISSUES.MD#issue-002--cd-github-release-failed-without-git-checkout-ci-re-ran-on-master-merge) | fixed | CD Release + CI triggers | `gh release create` needed checkout/`--repo`; CI no longer runs on master push after PR green |

---

## Phase 4 — TICKET-142 fleet discount

**Branch:** `feature/ticket-142-fleet-discount`

**Why this phase exists:** Part 2 of the brief. Deliver fleet **through** the
path built in Phases 1–3 (scenarios → red → green → refactor).

**Acceptance (from ticket):**

- Quote for fleet shows a “Fleet discount” line and correct total
- Non-fleet quotes/invoices unchanged to the penny
- Existing `.dat` files load without manual conversion
- Existing invoices still display correctly
- 4% off **vehicle price** only; fees (delivery, registration) not discounted
- Own line, same idea as trade discount
- Staff can mark fleet on new and existing customers (e.g. Greenway Taxis)

| ID | Status | What | Why | How (when executing) |
|----|--------|------|-----|----------------------|
| P4-1 | [x] | Fleet Gherkin + retail/trade regression scenarios | BDD first; lock acceptance language | New `BDD/features/` scenarios; keep baseline regressions |
| P4-2 | [x] | Red CTest for fleet scenarios | TDD: fail before production change | Tests compiled/registered; failing for missing fleet behaviour |
| P4-3 | [x] | Implement fleet mark + 4% line in calc/UI/persistence as required | Smallest change that greens P4-2 | Extend customer type/UX, `calc_quote`, quote/invoice print; document storage call for combined `disc` if needed |
| P4-4 | [x] | Green full suite in Docker; penny regression; data/invoice checks | Prove part 2 without breaking part 1 | Gate + CTest green; manual smoke; restore `data/` if mutated |

**Done when:** Fleet acceptance criteria pass under Docker CTest and manual
check; non-fleet totals unchanged to the penny.

---

## Phase 5 — Gherkin E2E + terminal proof

**Branch:** `feature/gherkin-e2e-terminal`

**Why this phase exists:** `BDD/features/*.feature` are the acceptance contract
(BDD). CTest under `tests/` is the TDD proof for unit/seam scenarios (1:1).
`BDD/runner` drives the real `dealerdesk` binary for scenarios tagged
`@terminal`, asserts production behaviour (stdout / persisted state), and can
capture PNGs as human-visible E2E evidence. BDD and TDD stay together; the
runner is the UI path, not a replacement for CTest.

**Assertions:** Primary checks are against production (stdout / persisted state
from the binary). PNGs are visual proof; optional PNG asserts are acceptable
because the UI is a stable terminal, not a designed GUI.

**Standing split (do not blur):**

| Layer | Features / scenarios | Who runs them |
|-------|----------------------|---------------|
| Gherkin + CTest only | `pricing.feature`, `persistence.feature` (+ unit `rnd2`) | `ctest` / `make test` / `make docker-test` |
| Gherkin + CTest today; **must** also gain `@terminal` | stock, customers (beyond mark-fleet), quote/sell, remaining fleet UX (add new as F) | CTest now; `make bdd` / `make docker-bdd` when P5-8..P5-11 land |
| `@terminal` already | smoke Main Menu; mark existing as fleet; fleet quote (+ 2 PNGs) | `make bdd` / `make docker-bdd` |

**Out of scope here:** Cucumber beauty contest; changing non-fleet totals;
forcing `@terminal` onto pure `calc_quote` / flat-file persistence math (those
stay Gherkin contract + CTest).

| ID | Status | What | Why | How (when executing) |
|----|--------|------|-----|----------------------|
| P5-1 | [x] | Scaffold BDD runner (Gherkin → drive `dealerdesk`) | Wire `.feature` steps to the real binary on host or Docker | C console session + step definitions under `BDD/`; isolated temp `data/`; `BDD/runner/README.md` |
| P5-2 | [x] | E2E assert: mark existing customer as fleet | TICKET-142 / fleet via UI | `@terminal` feature + steps; assert stdout/state |
| P5-3 | [x] | PNG for “mark existing as fleet” flow | Human-visible E2E proof | `BDD/artifacts/mark_existing_fleet.png` |
| P5-4 | [x] | E2E assert: fleet quote shows “Fleet discount” line and total | Ticket quote acceptance on the terminal | `@terminal` feature + steps on production binary |
| P5-5 | [x] | PNG for “fleet quote” flow | Second visual E2E proof | `BDD/artifacts/fleet_quote.png` |
| P5-6 | [x] | make/docker target (+ CI if it fits) for the BDD suite | Repeatable habit; CTest remains trusted unit gate | `make bdd` / `make docker-bdd`; fail closed on that path |
| P5-7 | [ ] | Board documents CTest-only vs `@terminal` split | Stop treating Phase 5 as “done” while menu flows lack UI proof | This standing-split table; pricing/persistence stay CTest; no fake `@terminal` on calc/file seams |
| P5-8 | [ ] | `@terminal` stock UI scenarios + steps; green | Stock is daily dealer ops; needs binary proof | Tag/write stock terminal scenarios; steps in `BDD/step_definitions/`; assert stdout/state; `make bdd` green |
| P5-9 | [ ] | `@terminal` customers UI scenarios + steps; green | Add/find/list R/T beyond mark-fleet | Tag/write customers terminal scenarios; steps; assert stdout/state; `make bdd` green |
| P5-10 | [ ] | `@terminal` quote/sell UI scenarios + steps; green | Quote/sell encode sold-block and invoice packing on the real UI | Tag/write quote/sell terminal scenarios; steps; assert stdout/state; `make bdd` green |
| P5-11 | [ ] | `@terminal` add new customer as fleet; green | Ticket: staff can mark fleet on new customers too | Terminal flow for type F on add; assert stdout/state; `make bdd` green |
| P5-12 | [ ] | PNG per new happy path (or assert-only if agreed) | Same visual bar as P5-3/P5-5 | Screen-emulation PNGs under `BDD/artifacts/`; skip only if explicitly agreed when executing |
| P5-13 | [ ] | Full `make bdd` + `make docker-bdd` green; then mark phase done | Phase 5 is incomplete until menu UI proof exists | Host + Docker suite green; CTest still green; checkboxes only after implementation |

**Done when:** P5-7..P5-13 are done; CTest remains green; non-fleet totals
unchanged to the penny; docs explain host and Docker BDD runs. Phase 5 is
**not** done after P5-1..P5-6 alone. Do **not** start Phase 6 until this
**Done when** is true.

---

## Phase 6 — NOTES and submission shape

**Why this phase exists:** Required deliverable. Interviewers read history,
agent setup, and a short NOTES file. This remains the **last** phase.

**Blocked until:** Phase 5 **Done when** (including P5-7..P5-13) is satisfied.

| ID | Status | What | Why | How (when executing) |
|----|--------|------|-----|----------------------|
| P6-1 | [ ] | Write `NOTES.md` (≤2 pages) | Challenge requires it | Use JOURNAL + git history: what/why priority, one agent miss + how caught, assumptions, next with more time |
| P6-2 | [ ] | Confirm agent setup + todo↔SHA completeness; no upstream PR | Submission shape | Check rules/skills/scripts/workflows in repo; JOURNAL complete; own remote only |

**Done when:** `NOTES.md` exists, setup is in-repo, history tells the story,
nothing was PRed to `felipeamarante/dealerdesk`.

---

## Execution order (reminder)

```text
P0 docs/rules  →  P1 Docker/CMake  →  P2 baseline BDD/TDD
  →  P3 gate + CI/CD demo  →  P4 TICKET-142
  →  P5 Gherkin E2E + PNG (incl. P5-7..P5-13)  →  P6 NOTES
```

Do **not** start Phase 6 until Phase 5 follow-ups P5-7..P5-13 are green.

Implementation of Phases 1–6 starts only when we explicitly begin that phase’s
branch. Until then, this file is the contract for what we will build.
