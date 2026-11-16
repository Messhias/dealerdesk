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

### Order of calculation (`calc_quote`)

1. `list` = vehicle list price
2. Trade discount: if customer type is `T`, `10%` of list (rounded to 2
   decimals)
3. Salesman discount: percent forced into `[0, 15]`, then applied to
   `(list - trade_disc)`
4. `net` = list − trade_disc − salesman_disc
5. `delivery` = 150.00
6. `vat` = 20% of `(net + delivery)` (rounded to 2 decimals)
7. `reg_fee` = 55.00
8. `total` = net + delivery + vat + reg_fee − tradein

### How a price is built (plain words)

Start with the car's list price.

If the customer is Trade (type `T`), take 10% off that list price.
Retail customers (type `R`) skip this step.

Then the salesman may give extra % off. That % can only be from 0 to 15.
If someone types -3, the program uses 0. If someone types 40, the program
uses 15. This second discount is calculated on what is left after the trade
discount (not on the full list again).

What is left is the net price.

Add delivery: always 150.00.
Add VAT: 20% of (net + delivery), rounded to two decimals.
Add registration fee: always 55.00.
Subtract any trade-in amount.

That sum is the total.

Rounding: when the code rounds money (`rnd2`), it keeps two digits after the
decimal (cents). Halfway rounds up. Example: 1.235 becomes 1.24.

There is no manager PIN in the code. The salesman % is only capped at 15.

### Example: invoice 50230 (retail customer)

What we know:

- List price: 9295.00
- Customer type: Retail (no trade discount)
- Salesman discount: 5%
- Trade-in: 1500.00

Do the sums:

1. Trade discount = 0.00
2. Salesman discount = 5% of 9295.00 = 464.75
3. Net = 9295.00 - 0.00 - 464.75 = 8830.25
4. Delivery = 150.00
5. VAT = 20% of (8830.25 + 150.00) = 20% of 8980.25 = 1796.05
6. Registration = 55.00
7. Total = 8830.25 + 150.00 + 1796.05 + 55.00 - 1500.00 = 9331.30

Customer pays: 9331.30

### Example: invoice 50231 (trade customer)

What we know:

- List price: 13495.00
- Customer type: Trade (10% off list)
- Salesman discount: 0%
- Trade-in: 0.00

Do the sums:

1. Trade discount = 10% of 13495.00 = 1349.50
2. Salesman discount = 0.00
3. Net = 13495.00 - 1349.50 - 0.00 = 12145.50
4. Delivery = 150.00
5. VAT = 20% of (12145.50 + 150.00) = 20% of 12295.50 = 2459.10
6. Registration = 55.00
7. Total = 12145.50 + 150.00 + 2459.10 + 55.00 - 0.00 = 14809.60

Customer pays: 14809.60

### Example: invoice 50232 (retail customer)

What we know:

- List price: 10495.00
- Customer type: Retail (no trade discount)
- Salesman discount: 8%
- Trade-in: 0.00

Do the sums:

1. Trade discount = 0.00
2. Salesman discount = 8% of 10495.00 = 839.60
3. Net = 10495.00 - 0.00 - 839.60 = 9655.40
4. Delivery = 150.00
5. VAT = 20% of (9655.40 + 150.00) = 20% of 9805.40 = 1961.08
6. Registration = 55.00
7. Total = 9655.40 + 150.00 + 1961.08 + 55.00 - 0.00 = 11821.48

Customer pays: 11821.48

These three examples match the sample rows in `data/invoices.dat`.

### Screen vs saved invoice

On screen, a quote shows the trade discount and the salesman discount on
separate lines.

In `data/invoices.dat` those two discounts are saved as one number (`disc` =
trade discount + salesman discount). Delivery and registration are saved as
one fees number: 150.00 + 55.00 = 205.00.

### Notes

- Help text in the menu and the comment block at the top of `calc.c` can be
  wrong about trade %, VAT base, and manager PIN. Trust this section and the
  code in `calc_quote`.
- Plate upper-case / space normalisation is mentioned in comments and help
  text but is not applied when adding a vehicle.

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
