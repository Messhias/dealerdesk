# DealerDesk — delivery journal

Running log for the `docs/TASKS.md` workstream. Each completed todo should
appear here with its git commit SHA so history stays 100% traceable.

This file feeds `NOTES.md` at the end of the delivery path.

## Entry format

```text
| date | phase | task id | commit SHA | notes |
```

Use short ISO dates (`YYYY-MM-DD`). SHA may be abbreviated (`git rev-parse --short`).

## Entries

| date | phase | task id | commit SHA | notes |
|------|-------|---------|------------|-------|
| 2026-11-16 | P0 | P0-1 | 50d6718 | Encoded challenge ground rules, commit-per-todo, release disclaimer in AGENTS.md and .cursor/rules |
| 2026-11-16 | P0 | P0-2 | 842577c | Added docs/TASKS.md board (chosen delivery sequence, not a brief-imposed checklist) |
| 2026-11-16 | P0 | P0-3 | ee7cfd0 | Created JOURNAL scaffold and logged Phase 0 commits |
| 2026-11-16 | P0 | P0-4 | 23d808d | Pointed docs/SDLC.md at TASKS, JOURNAL, commit-per-todo, trusted gate |
| 2026-11-16 | P0 | P0-5 | e6d470f | Aligned BRIEF.md with artifact gaps (no upstream PR; what they look at) |
| 2026-11-16 | P0 | P0-6 | 3b2c7d8 | Expanded TASKS.md with what/why/how per phase; docs only; reset P1 checkboxes
| 2026-11-16 | P1 | P1-1 | f650996 | Added Dockerfile (debian bookworm-slim, build-essential, cmake)
| 2026-11-16 | P1 | P1-2 | 994203e | Root CMakeLists + tests hook + build/ gitignore
| 2026-11-16 | P1 | P1-3 | 86e659d | docker-compose.yml + docs/DOCKER.md shell/build/ctest/run
| 2026-11-16 | P1 | P1-4 | 3346384 | test_smoke green in container (1/1 Passed)
| 2026-11-16 | P3 | P3-5 | 1ac5cd8 | Board: RELEASE.txt + Release notes on each CD release (2.3 → v2.4)
| 2026-11-16 | P1 | P1-host | 465b12e | Makefile + README/DOCKER host&docker paths; build vs build-docker
| 2026-11-16 | P2 | P2-0 | 461b14a | Plain-language Pricing in SYSTEM.md with three sample invoice walkthroughs
| 2026-11-16 | P2 | P2-1 | 129ccac | features/pricing.feature with sample invoice goldens and clamp/rounding cases
| 2026-11-16 | P2 | P2-2 | ca6e23f | CTest test_pricing: rnd2 + 8 pricing.feature scenarios 1:1; green in Docker |
| 2026-11-17 | P2 | P2-3 | 12785cd | features/persistence.feature + test_persistence via chdir temp data/; zero dbfile change. Lesson: first tried a db_set_data_dir production seam; wrong. Same isolation without touching production persistence code. |
| 2026-11-17 | P2 | P2-4 | c87fd1f | features/quote_sell.feature + sale.c seams + test_quote_sell; sold blocked, disc/fees packing, confirm/cancel without stdin; 22/22 green in Docker |
| 2026-11-17 | P2 | P2-5 | 6d15e16 | features/stock.feature + stock.c seams + test_stock; test_common harness extract; drop test_smoke; 29/29 green on host |
| 2026-11-17 | P2 | P2-6 | PENDING | features/customers.feature + customer.c seams + test_customer; R/T add/find/list; 37/37 green on host |
