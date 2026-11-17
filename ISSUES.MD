# Issues (local notes)

Local findings before opening anything upstream. Not a tracker; just notes.

---

## ISSUE-001 — Non-numeric menu input exits (or picks wrong option)

**Status:** open (not filed upstream yet)  
**Area:** menus / input (`screen.c`, `main.c`)  
**Severity:** medium (easy to quit by accident; bad UX)

### What happens

In any menu that uses `read_int("  Option: ")` (main, stock, customers, invoices):

| Input | Expected | Actual |
|-------|----------|--------|
| `X` or `x` or `abc` | "Invalid option", stay on menu | Treated as `0` → main menu exits ("Goodbye.") |
| `2x` or `1abc` | "Invalid option", stay on menu | Parsed as `2` / `1` → wrong submenu opens |

Repro (main menu):

1. Run DealerDesk, land on Main Menu.
2. At `Option:`, type `X` and Enter.
3. Program prints `Goodbye.` and exits instead of rejecting the input.

Same pattern with `2x`: opens Customers (option 2) instead of rejecting.

### Why

`read_int` in `screen.c` does:

```c
read_line(prompt, b, sizeof b);
return atoi(b);
```

`atoi` returns `0` when the string has no leading digits, and stops at the first non-digit when it does (`"2x"` → `2`). Main menu maps `0` to Exit:

```c
case 0:
    printf("\n  Goodbye.\n");
    return 0;
```

Submenus treat `0` as "back", so `X` there leaves the submenu instead of saying invalid.

Related: `read_line` calls `exit(0)` when `fgets` returns `NULL` (EOF / Ctrl+D), which also closes without a menu message. Separate path, same "unexpected quit" feel.

### Affected call sites

All `opt = read_int("  Option: ")` loops in `main.c` (main, stock, customer, invoice menus). Same helper is used for IDs and other ints; those may need a different policy (reject vs zero), but the menu case is the clear bug.

### Suggested fix (when tackled)

- Reject input that is not a pure integer (or not entirely digits after trim).
- On reject: print "Invalid option", pause, redisplay menu.
- Do not map failed parse to `0`.
- Keep existing valid numeric options unchanged (penny-safe / staff-visible behaviour only for intentional changes).

### Notes

Found during manual use. Decision pending: open as project issue or fold into delivery hardening / BDD scenarios later.

---

## ISSUE-002 — CD GitHub Release failed without git checkout; CI re-ran on master merge

**Status:** fixed  
**Area:** GitHub Actions (`.github/workflows/cd-release.yml`, `.github/workflows/ci.yml`)  
**Severity:** high (blocked GitHub Release after tag + builds; wasted CI on every merge)  
**Related:** Phase 3 / P3-3 / P3-5 — see `docs/TASKS.md` (Phase 3 follow-ups)

### What happened

After merge of P3-5, CD created tag `v2.5`, built Win/Linux artifacts, then failed in **GitHub Release**:

```text
failed to run git: fatal: not a git repository (or any of the parent directories): .git
```

`gh release create` ran in a job that only downloaded artifacts (no `actions/checkout`), so `gh` could not discover the repo via git. Tag and `RELEASE.txt` had already been updated; the Release itself was missing until recovered manually.

Separately, CI (Debian Bookworm CTest) ran again on push to `master` after the PR was already green — redundant with the PR check.

### Fix

- CD release job: `actions/checkout` at the release tag + `gh release create --repo ${{ github.repository }}`.
- CI triggers: `pull_request` to `master` only (no `push` to `master`).
- Recovered missing GitHub Release for `v2.5` from the failed run’s artifacts.

### Notes

Tracked for delivery history; linked from `docs/TASKS.md` Phase 3 follow-ups.
