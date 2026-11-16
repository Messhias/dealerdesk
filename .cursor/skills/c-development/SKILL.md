---
name: c-development
description: Best practices, memory management, security, and compilation guidelines for C projects (C11/C17), adapted for legacy DealerDesk code. Use when writing, reviewing, debugging, or structuring C code in this repository.
---

# Professional C Development (DealerDesk)

When creating, refactoring, or debugging C code in this project, follow the guidelines below. Prefer safety and clarity, but do not rewrite working legacy style for style alone. Dealership staff must not notice unrelated behavior changes.

## 0. Legacy Fit (This Repo)

* Match existing module layout: `main.c`, `dbfile.c`, `screen.c`, `calc.c` and their `.h` files; shared types live in `globals.h`.
* Keep existing typedef names (`VEHICLE`, `CUSTOMER`, `INVOICE`) and field types that mirror `data/*.dat` layouts. Do not rename to `_t` or switch persisted fields to `<stdint.h>` unless a ticket explicitly requires a format change.
* Prefer fixed-size tables and stack buffers already used in the app. Introduce heap allocation only when necessary, with clear ownership.
* New helpers: `snake_case`, `static` in `.c` when internal. New macros/constants: `UPPER_SNAKE_CASE`.

## 1. Coding Standards and Style

* **Language Standard:** Use **C11** or **C17** as a baseline. Avoid proprietary compiler extensions unless strictly necessary.
* **Explicit Typing:** For new protocol/size/index locals that are not part of on-disk records, prefer fixed-width types from `<stdint.h>` (`uint8_t`, `int32_t`) or `size_t` where appropriate. Leave existing `int` / `long` / `double` record fields as-is.
* **Const-Correctness:** Mark input pointers and read-only locals with `const`.
* **Naming:** `snake_case` for functions and variables; `UPPER_SNAKE_CASE` for macros and constants. Do not add `_t` to existing public typedefs.

## 2. Memory Management and Pointers

* **Allocation Checks:** If using `malloc()`, `calloc()`, or `realloc()`, always check for `NULL` before dereference.
* **Dangling Pointer Prevention:** Set pointers to `NULL` immediately after `free()`.
* **Clear Ownership:** In header comments, document who allocates and who frees when heap is used.
* **Zero Leaks:** Every heap block needs a cleanup path, including early errors (`goto cleanup;` when managing multiple resources).
* **Default path:** Prefer the project's existing global tables and fixed buffers over new dynamic allocation.

## 3. Security and Bug Prevention

* **Safe Functions:** Never use `gets()`, `strcpy()`, `strcat()`, or `sprintf()`. Use `fgets()`, `snprintf()`, or bounds-checked `memcpy()`.
* **Bounds Checking:** Validate buffer sizes, array indices, and table capacities (`MAX_VEH`, `MAX_CUST`, `MAX_INV`) before read/write.
* **Initialization:** Initialize locals and structs at declaration (`struct foo bar = {0};` or field-by-field as the surrounding code does).
* **Input:** Treat menu and file input as untrusted; keep plate/name/email copies length-safe.

## 4. File Structure and Headers

* **Header Guards:** Every `.h` uses traditional include guards (`#ifndef NAME_H` / `#define` / `#endif`), consistent with this repo. `#pragma once` is acceptable only in brand-new headers if the rest of a new module already uses it; prefer guards here.
* **Encapsulation:** Public API in `.h` only. Internal helpers in `.c` as `static`.

## 5. Compilation and Verification (Agent Workflow)

Whenever generating or modifying C code, run or suggest compilation with strict warnings and sanitizers:

```bash
gcc -std=c17 -Wall -Wextra -Wpedantic -Werror -g -fsanitize=address,undefined *.c -lm -o dealerdesk
```

* If the project has a `Makefile` or `CMakeLists.txt`, use that build instead of ad-hoc `gcc`.
* Until a build file exists, the historical quick build remains valid for smoke checks: `gcc -o dealerdesk *.c -lm` (run from repo root so `data/` resolves).
* After behavior changes, smoke-test the menu paths that touch stock, customers, quotes/invoices, and persistence under `data/`.
)
