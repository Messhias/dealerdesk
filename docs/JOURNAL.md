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
| 2026-11-17 | P2 | P2-6 | b027a8a | features/customers.feature + customer.c seams + test_customer; R/T add/find/list; 37/37 green on host |
| 2026-11-17 | P2 | P2-7 | c2b2331 | Full baseline suite green in Docker via make docker-test; 37/37 Passed; data/ untouched |
| 2026-11-17 | P3 | P3-1 | 8e1dafa | Docker entrypoint pre-run gate + .dockerignore; make docker-run; docker-test 37/37; gated run reaches Main Menu |
| 2026-11-17 | P3 | P3-2 | f010ad4 | scripts/run-with-tests.sh host wrapper; make docker-run delegates; DOCKER.md; 37/37 + Main Menu |
| 2026-11-17 | P3 | P3-3 | 41e4c09 | GHA ci.yml: debian:bookworm-slim (same as Dockerfile) + CMake/CTest; no compose in CI |
| 2026-11-17 | P3 | P3-4 | e2f6b22 | cd-release.yml: tag v2.4+ + Win/macOS/Linux Release; workflow_dispatch dry_run; UNIX-only libm |
| 2026-11-17 | P3 | P3-5 | 3e63551 | RELEASE.txt + CD notes automation; drop macOS from CD matrix (Win+Linux); catch-up Last release 2.4 |
| 2026-11-17 | P3 | ISSUE-002 | bafe0c7 | Fix CD gh release without .git (checkout + --repo); CI PR-only; recover v2.5 Release; ISSUES.md + TASKS follow-up |
| 2026-11-18 | P4 | P4-1 | 3394dee | features/fleet.feature + retail/trade regression comments in pricing/customers features |
| 2026-11-18 | P4 | P4-2 | fce4332 | test_fleet.c + CMake registration; scenarios 1:1 with fleet.feature |
| 2026-11-18 | P4 | P4-3 | 84fd08b | type F, fleet_disc 4%, set-type UX, quote line, invoice packing; SYSTEM.md; 44/44 Docker |
| 2026-11-18 | P4 | P4-4 | 6dbc516 | TASKS Phase 4 checkboxes done; app-version standing decision on the board |
| 2026-11-18 | — | version | e5df27f | version.h single source; CD bumps with RELEASE.txt; AGENTS/rules/README aligned |
| 2026-11-18 | P5 | P5-1..P5-6 | 2c7d362 | BDD/ tree (features+runner+steps); @terminal fleet flows; PNGs; make bdd/docker-bdd; host/Docker binary split |
| 2026-11-18 | P5 | P5-7..P5-10 | 52ff510 | All existing scenarios @terminal; per-feature steps; CMake bdd_runner; PNGs under artifacts/<feature>/; board aligned |
| 2026-11-19 | P6 | P6-1 | 113310b | NOTES.md ≤2 pages: priorities, agent miss, assumptions, next |
| 2026-11-19 | P6 | P6-2 | pending-sha | Agent setup in-repo; JOURNAL complete; origin Messhias/dealerdesk only; TASKS P6 done |
