# DealerDesk — system behaviour (as implemented)

This document describes what the codebase does today. The C sources and
`data/*.dat` files are the source of truth. Where older docs disagree with the
code, trust the code.

## Purpose

Terminal menu application for a car showroom. Staff manage individual vehicles,
customers, quotes, and invoices.

## Domain model

### Vehicles (stock)

Each vehicle is one record (not a quantity). Status values:

| Code | Meaning |
|------|---------|
| 0 | IN STOCK |
| 1 | RESERVED |
| 2 | SOLD |

Fields include id, plate, make, model, year, mileage, cost, list price, status.
Stored in `data/stock.dat`. Limits: `MAX_VEH` 500 (`globals.h`).

### Customers

| `type` | Meaning |
|--------|---------|
| `R` | Retail |
| `T` | Trade |

Fields: id, name, phone, email, type. Stored in `data/customers.dat`.
Limits: `MAX_CUST` 500.

### Invoices

Created only when a sale is confirmed. Stored in `data/invoices.dat`.
Limits: `MAX_INV` 2000. Discount amounts are stored as a single combined
`disc` field on the invoice record.

### ID counters

`data/next_ids.dat` holds `vehicle=`, `customer=`, and `invoice=` counters.

## Main flows

### Quote

1. Operator enters vehicle id, customer id, salesman discount %, trade-in.
2. `calc_quote` builds a `QUOTE` in memory.
3. Result is printed. Quotes are not persisted.

Sold vehicles cannot be quoted.

### Sell vehicle

1. Same inputs as quote; preview is shown.
2. On confirm: append invoice, set vehicle status to SOLD, save stock,
   invoices, and id counters.

### Stock / customers / invoices menus

List, add, find, change price, reserve/release, list/view invoices. See
`main.c` for the menu tree. There is no Reports module in the current sources.

## Pricing (`calc.c`)

Order of calculation in `calc_quote`:

1. `list` = vehicle list price
2. Trade discount: if customer type is `T`, `10%` of list (rounded to 2 dp)
3. Salesman discount: `disc_pct` clamped to `[0, 15]`, applied to
   `(list - trade_disc)`
4. `net` = list − trade_disc − salesman_disc
5. `delivery` = 150.00
6. `vat` = 20% of `(net + delivery)` (rounded to 2 dp)
7. `reg_fee` = 55.00
8. `total` = net + delivery + vat + reg_fee − tradein

Notes:

- Manager PIN for high discounts is not implemented; the code only caps at 15%.
- Plate upper-case / space normalisation is mentioned in comments and help text
  but is not applied when adding a vehicle.
- Sample invoices in `data/invoices.dat` match this formula.

## Persistence (`dbfile.c`)

- Directory: `data/` relative to the process working directory.
- Format: one record per line, fields separated by `|`.
- Lines starting with `#` or empty lines are skipped on load.
- Input `|` characters are replaced with `/` in `screen.c` so fields stay intact.
- Each save rewrites the whole file.

## Modules

| File | Role |
|------|------|
| `main.c` | Menus and business screens |
| `calc.c` / `calc.h` | Quote pricing |
| `dbfile.c` / `dbfile.h` | Load/save flat files |
| `screen.c` / `screen.h` | Terminal I/O helpers |
| `globals.h` | Shared structs and limits |

## Build (what works today)

From the repository root:

```text
gcc -o dealerdesk *.c -lm
./dealerdesk
```

Run from the repo root so `data/` resolves correctly.

## Challenge context (not implemented here)

- `BRIEF.md` — take-home goals (delivery hardening + feature work)
- `TICKET-142.md` — fleet discount (pending)

## Known doc drift

The shipped `README.md` still describes paths, VAT base, trade %, manager PIN,
reports/`report.c`, and version strings that do not match the current code.
Do not treat the README as behavioural truth without checking the sources.
