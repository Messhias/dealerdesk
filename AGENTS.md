# Agent instructions — DealerDesk

Canonical project instructions for any coding agent (Claude, Copilot, Cursor,
DeepSeek, Codex, Gemini, and similar). Tool-specific entry files in this repo
defer here so constraints stay shared.

## What this repo is

C terminal app for a car dealership: stock (individual vehicles), customers,
quotes, invoices. Flat-file persistence under `data/`.

## Read first

- Behavioural truth: `docs/SYSTEM.md` and the C sources
- Delivery loop: `docs/SDLC.md`
- Challenge brief: `BRIEF.md`
- Pending feature: `TICKET-142.md`

## Agent entry points

| Path | Used by |
|------|---------|
| `AGENTS.md` | Shared / Codex / DeepSeek / generic agents |
| `CLAUDE.md` | Claude Code and Claude-based agents |
| `.github/copilot-instructions.md` | GitHub Copilot |
| `.cursor/rules/` | Cursor |

## Source of truth

- Prefer **code and `data/*.dat`** over `README.md` when they disagree.
- Pricing truth is `calc.c` (`calc_quote`), not the README pricing section.
- Do not invent code, features, or behaviour the ticket did not ask for.

## Language

- Documentation, code comments, commit messages, and agent instruction files:
  **English**.
- Chat with the user may be in another language; repo files stay English.

## Global project rules

Hard constraints for this repository. Not optional, not tool-specific.

- **No git commit trailers.** Never add `Co-authored-by`, `Signed-off-by`,
  `Generated-by`, `Assisted-by`, `Made-with`, or any equivalent metadata to
  commits, PRs, or files. Authorship stays with the human owner only.
- Do not force-push or rewrite shared history unless explicitly requested.
- Do not push or open a PR unless the user asks.
- **Commit-per-todo (delivery workstream):** when working items listed in
  `docs/TASKS.md`, each completed todo gets exactly one dedicated commit.
  Append `docs/JOURNAL.md` with date, phase, task id, commit SHA, and a short
  note. No bundling unrelated todos. Outside that workstream, do not commit
  unless the user asks.

## Challenge ground rules (take-home)

How this project is worked for the Nextlane DealerDesk challenge. Priorities
are part of the answer; see `docs/TASKS.md` for the chosen sequence.

- AI tools are expected; steering them is part of the work (`NOTES.md` later).
- Any test framework, build tool, or CI is fine. GitHub Actions is convenient
  for reviewers, not required.
- Apart from an intentional new feature, dealership staff must not notice
  changes. Keep non-ticket quote/invoice totals correct to the penny.
- Suggested effort is about 6–8 hours within roughly one week. Finishing
  everything is not required by the brief; this repo’s chosen bar is still
  full baseline BDD/TDD before fleet (see `docs/TASKS.md`).
- Commit as you go; history is reviewed.
- If the ticket or code is unclear: make a reasonable call and write it down
  (`docs/JOURNAL.md` and later `NOTES.md`).

## Quality gate and releases

- **Trusted quality path:** Docker environment + acceptance scenarios
  (Gherkin behaviour/acceptance contract) + CTest/TDD, including the Docker
  pre-run gate when present.
- **Multi-OS GitHub Release binaries** (Windows, macOS, Linux) are a
  **demonstration of a CI/CD delivery cycle** only. There is **no** sanity-test
  suite around those host artifacts. Out-of-Docker builds, downloads, and any
  host-OS smoke checks are at the **user’s discretion** and do not replace the
  Docker + CTest gate. Never describe those binaries as production-certified.
- **App version:** single source is `version.h` (`DEALERDESK_VERSION`), shown
  by `screen.c` in the terminal banner. Do not hardcode a version string in
  `screen.c` or invent a new literal elsewhere. On each CD release to `master`,
  `scripts/update-release-txt.sh` updates `RELEASE.txt` and `version.h` to the
  same version before the tag, so GitHub Release and the binary header match.
  Only bump manually when an explicit release task says so.

## Working rules

- Follow `docs/SDLC.md`. **BDD and TDD are mandatory** on every behaviour
  change: write scenarios first, then red → green → refactor. Do not ship
  production behaviour without that loop.
- Acceptance scenarios live under `BDD/features/` as a behaviour/acceptance
  contract (PO language). CTest runs the verifying C tests. Do not call
  feature files an “implementation spec”. Terminal UI proof lives under `BDD/` (runner, step definitions, `@terminal`
  features, PNG artifacts). Phase 5 on `docs/TASKS.md`; it does not replace
  CTest.
- Dealership staff rely on current behaviour. Outside an intentional ticket,
  keep quotes/invoices for existing customer types correct to the penny.
- Run from repository root (`data/` is relative). Preferred controllable path:
  Docker + CMake/CTest (see `docs/TASKS.md`). Legacy one-liner still works:
  `gcc -o dealerdesk *.c -lm`
- After mutating sample data during manual checks, restore with
  `git checkout -- data` when appropriate.
- Manual checks always accompany automated tests; the agent does not replace
  human verification.
- Task board: `docs/TASKS.md`. Running log: `docs/JOURNAL.md`. Do not open a
  pull request against the upstream original challenge repository.

## Module map

| Path | Role |
|------|------|
| `main.c` | Menus and screens |
| `calc.c` | Pricing |
| `dbfile.c` | Flat-file I/O |
| `screen.c` | Terminal helpers |
| `version.h` | In-app version (`DEALERDESK_VERSION`) |
| `globals.h` | Structs and limits |
| `docs/SYSTEM.md` | Documented current behaviour |
| `docs/SDLC.md` | SDLC with AI |

## Out of scope unless asked

- Editing `README.md` for doc-alignment work that was scoped without it
- Implementing `TICKET-142` before delivery hardening / tests are in place if
  the current task is documentation only
