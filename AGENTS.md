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
- Do not commit unless the user asks.
- Do not force-push or rewrite shared history unless explicitly requested.

## Working rules

- Follow `docs/SDLC.md`. **BDD and TDD are mandatory** on every behaviour
  change: write scenarios first, then red → green → refactor. Do not ship
  production behaviour without that loop.
- Dealership staff rely on current behaviour. Outside an intentional ticket,
  keep quotes/invoices for existing customer types correct to the penny.
- Run from repository root (`data/` is relative).
- Working build today: `gcc -o dealerdesk *.c -lm`
- After mutating sample data during manual checks, restore with
  `git checkout -- data` when appropriate.
- Manual checks always accompany automated tests; the agent does not replace
  human verification.

## Module map

| Path | Role |
|------|------|
| `main.c` | Menus and screens |
| `calc.c` | Pricing |
| `dbfile.c` | Flat-file I/O |
| `screen.c` | Terminal helpers |
| `globals.h` | Structs and limits |
| `docs/SYSTEM.md` | Documented current behaviour |
| `docs/SDLC.md` | SDLC with AI |

## Out of scope unless asked

- Editing `README.md` for doc-alignment work that was scoped without it
- Implementing `TICKET-142` before delivery hardening / tests are in place if
  the current task is documentation only
