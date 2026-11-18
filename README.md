This standard disclaimer states that the work is entirely fictional and any resemblance to real people, places, or events is coincidental.

DEALERDESK v2.6
===============

Dealer management for the showroom: stock, customers, quotes and invoices.

In-app banner version comes from `version.h` (`DEALERDESK_VERSION`). CD keeps
that file aligned with each GitHub Release tag.


QUICK START (Linux, macOS, or WSL on Windows)
---------------------------------------------

You need a C compiler (gcc or clang). From the repository root:

    gcc -o dealerdesk *.c -lm
    ./dealerdesk

* Always run it from the repository root. It reads and writes the files in ./data
* Pick a menu option by typing its number and pressing ENTER. 0 goes back, and
  0 on the main menu exits.
* Every change (new vehicle, sale, ...) is saved to data/*.dat immediately.
  To put the sample data back:

      git checkout -- data


BUILD
-----

    gcc -o dealerdesk main.c dbfile.c screen.c report.c

(Windows releases are built with the Borland compiler on the old build PC.)


RUN
---

    ./dealerdesk

Data files are read from C:\DEALERDESK\DATA

    stock.dat       vehicles
    customers.dat   customers
    invoices.dat    invoices
    next_ids.dat    id counters

Do NOT edit the .dat files by hand while the program is running!


MENU
----

    1. Stock
    2. Customers
    3. Quote
    4. Sell vehicle
    5. Invoices
    6. Reports (monthly sales, margin per vehicle)
    0. Exit


PRICING RULES
-------------

* VAT (20%) is calculated on the list price. Discounts are then taken off
  the VAT inclusive price.
* Trade customers get a 5% trade discount.
* Salesman discount is up to 10%. Anything above 10% asks for a manager PIN.
* Delivery (150.00) and first registration (55.00) are added to every sale.
  Both are VAT exempt.
* Trade-in value is taken off the final total.


RELEASE
-------

See RELEASE.txt


Problems: ask Phil (ext. 214).


------------------------------------------------------------------------------
MODERN BUILD / TEST (delivery path — keep this section updated)
------------------------------------------------------------------------------

Two supported ways from the repository root. Prefer Docker when you want the
pinned toolchain (trusted quality path). Host CMake is fine for day-to-day
work if you have cmake installed.

Prerequisites
-------------

* Always run commands from the repository root (`data/` is relative).
* Host path: a C compiler plus CMake on PATH.
  - macOS (Homebrew): `brew install cmake`
  - Debian/Ubuntu: `sudo apt-get install -y build-essential cmake`
* Docker path: Docker Desktop (or Engine) + Compose v2. No host cmake needed.

Option A — host (Makefile wrappers)
-----------------------------------

    make help
    make          # cmake -S . -B build && cmake --build build
    make test     # build + ctest --output-on-failure
    make run      # ./build/dealerdesk
    make clean    # removes build/ and build-docker/

Equivalent raw cmake:

    cmake -S . -B build
    cmake --build build
    ctest --test-dir build --output-on-failure
    ./build/dealerdesk

Option B — Docker (pinned gcc + cmake)
--------------------------------------

Host and Docker use different CMake build dirs (`build/` vs `build-docker/`)
so caches with absolute paths do not clash.

    make docker-shell   # interactive shell in the image
    make docker-test    # configure + build + ctest in build-docker/

Or:

    docker compose run --rm dealerdesk bash

Inside the container, same cmake/ctest/run commands as host. Details:
`docs/DOCKER.md`.

Legacy quick build (still valid)
--------------------------------

    gcc -o dealerdesk *.c -lm
    ./dealerdesk

After mutating sample data: `git checkout -- data`.
