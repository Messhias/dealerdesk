/*
 * CTest coverage for features/quote_sell.feature (quote/sell seams).
 * Scenario titles map 1:1 via CTest NAME + argv dispatch.
 *
 * Confirm = complete_sale + save_*. Cancel = skip complete_sale.
 * Production dbfile keeps a fixed "data/" path; mutating scenarios
 * chdir into a private temp tree.
 */
#include <stdio.h>
#include <string.h>

#include "calc.h"
#include "dbfile.h"
#include "globals.h"
#include "sale.h"
#include "test_common.h"

VEHICLE  veh[MAX_VEH];
int      nveh;
CUSTOMER cust[MAX_CUST];
int      ncust;
INVOICE  inv[MAX_INV];
int      ninv;
int      next_veh_id;
int      next_cust_id;
int      next_inv_no;

static void seed_sale_fixture(void)
{
    nveh = 1;
    veh[0].id = 1101;
    snprintf(veh[0].plate, sizeof veh[0].plate, "AA11 AAA");
    snprintf(veh[0].make, sizeof veh[0].make, "Ford");
    snprintf(veh[0].model, sizeof veh[0].model, "Fiesta");
    veh[0].year = 2019;
    veh[0].mileage = 1000;
    veh[0].cost = 7000.00;
    veh[0].price = 9295.00;
    veh[0].status = ST_STOCK;

    ncust = 1;
    cust[0].id = 2101;
    snprintf(cust[0].name, sizeof cust[0].name, "Retail Buyer");
    snprintf(cust[0].phone, sizeof cust[0].phone, "07700 900001");
    snprintf(cust[0].email, sizeof cust[0].email, "buyer@example.com");
    cust[0].type = 'R';

    next_inv_no = 50233;
}

static int scenario_sold_cannot_quote(void)
{
    VEHICLE v;

    memset(&v, 0, sizeof v);
    v.id = 1001;
    v.status = ST_SOLD;
    assert_false("vehicle_can_quote sold", vehicle_can_quote(&v));
    return test_failed();
}

static int scenario_available_can_quote(void)
{
    VEHICLE v;

    memset(&v, 0, sizeof v);
    v.id = 1002;
    v.status = ST_STOCK;
    assert_true("vehicle_can_quote stock", vehicle_can_quote(&v));
    return test_failed();
}

static int scenario_packs_disc(void)
{
    QUOTE q;
    INVOICE i;

    calc_quote(&q, 10000.00, 'T', 5.0, 0.0);
    invoice_from_quote(&i, &q, 50240, "2026-11-17", 1101, 2101);
    assert_money("packed disc", i.disc, 1450.00);
    return test_failed();
}

static int scenario_packs_fees(void)
{
    QUOTE q;
    INVOICE i;

    calc_quote(&q, 9295.00, 'R', 5.0, 1500.00);
    invoice_from_quote(&i, &q, 50230, "2024-03-12", 1009, 2003);
    assert_money("packed fees", i.fees, 205.00);
    assert_money("packed disc", i.disc, 464.75);
    return test_failed();
}

static int scenario_confirm_sale(void)
{
    QUOTE q;
    int rc;

    reset_tables();
    if (enter_temp_data_tree("ddquotesell") != 0)
        return 1;

    seed_sale_fixture();
    calc_quote(&q, veh[0].price, cust[0].type, 5.0, 1500.00);
    rc = complete_sale(&veh[0], &cust[0], &q, "2026-11-17");
    assert_int("complete_sale rc", rc, 0);

    save_invoices();
    save_stock();
    save_ids();

    reset_tables();
    load_all();

    assert_int("ninv", ninv, 1);
    assert_money("inv disc", inv[0].disc, 464.75);
    assert_money("inv fees", inv[0].fees, 205.00);
    assert_money("inv total", inv[0].total, 9331.30);
    assert_int("veh status", veh[0].status, ST_SOLD);
    assert_int("next_inv_no", next_inv_no, 50234);

    leave_temp_data_tree();
    return test_failed();
}

static int scenario_cancel_sale(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddquotesell") != 0)
        return 1;

    seed_sale_fixture();
    /* Cancel: do not call complete_sale. */
    save_invoices();
    save_stock();
    save_ids();

    reset_tables();
    load_all();

    assert_int("ninv", ninv, 0);
    assert_int("veh status", veh[0].status, ST_STOCK);
    assert_int("next_inv_no", next_inv_no, 50233);

    leave_temp_data_tree();
    return test_failed();
}

typedef struct {
    const char *id;
    int (*fn)(void);
} scenario_entry;

static const scenario_entry SCENARIOS[] = {
    {"sold_cannot_quote", scenario_sold_cannot_quote},
    {"available_can_quote", scenario_available_can_quote},
    {"packs_disc", scenario_packs_disc},
    {"packs_fees", scenario_packs_fees},
    {"confirm_sale", scenario_confirm_sale},
    {"cancel_sale", scenario_cancel_sale},
};

int main(const int argc, char **argv)
{
    size_t i;
    int failed = 0;

    if (argc >= 2) {
        for (i = 0; i < sizeof(SCENARIOS) / sizeof(SCENARIOS[0]); i++) {
            if (strcmp(argv[1], SCENARIOS[i].id) == 0) {
                test_reset_failures();
                return SCENARIOS[i].fn();
            }
        }
        fprintf(stderr, "unknown scenario id: %s\n", argv[1]);
        return 2;
    }

    for (i = 0; i < sizeof(SCENARIOS) / sizeof(SCENARIOS[0]); i++) {
        test_reset_failures();
        if (SCENARIOS[i].fn() != 0) {
            fprintf(stderr, "scenario failed: %s\n", SCENARIOS[i].id);
            failed = 1;
        }
    }
    return failed;
}
