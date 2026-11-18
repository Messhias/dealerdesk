/*
 * CTest coverage for features/fleet.feature (fleet discount + mark type F).
 * Scenario titles map 1:1 via CTest NAME + argv dispatch.
 */
#include <stdio.h>
#include <string.h>

#include "calc.h"
#include "customer.h"
#include "dbfile.h"
#include "globals.h"
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

static int scenario_fleet_4pct_no_salesman(void)
{
    QUOTE q;

    calc_quote(&q, 10000.00, 'F', 0.0, 0.00);
    assert_money("trade_disc", q.trade_disc, 0.00);
    assert_money("fleet_disc", q.fleet_disc, 400.00);
    assert_money("disc", q.disc, 0.00);
    assert_money("net", q.net, 9600.00);
    assert_money("delivery", q.delivery, 150.00);
    assert_money("vat", q.vat, 1950.00);
    assert_money("reg_fee", q.reg_fee, 55.00);
    assert_money("total", q.total, 11755.00);
    return test_failed();
}

static int scenario_fleet_then_salesman(void)
{
    QUOTE q;

    calc_quote(&q, 10000.00, 'F', 5.0, 0.00);
    assert_money("fleet_disc", q.fleet_disc, 400.00);
    assert_money("disc", q.disc, 480.00);
    assert_money("net", q.net, 9120.00);
    assert_money("vat", q.vat, 1854.00);
    assert_money("total", q.total, 11179.00);
    return test_failed();
}

static int scenario_fleet_fixed_fees(void)
{
    QUOTE q;

    calc_quote(&q, 10495.00, 'F', 0.0, 0.00);
    assert_money("fleet_disc", q.fleet_disc, 419.80);
    assert_money("delivery", q.delivery, 150.00);
    assert_money("reg_fee", q.reg_fee, 55.00);
    assert_money("net", q.net, 10075.20);
    assert_money("total", q.total, 12325.24);
    return test_failed();
}

static int scenario_retail_unchanged(void)
{
    QUOTE q;

    calc_quote(&q, 9295.00, 'R', 5.0, 1500.00);
    assert_money("trade_disc", q.trade_disc, 0.00);
    assert_money("fleet_disc", q.fleet_disc, 0.00);
    assert_money("disc", q.disc, 464.75);
    assert_money("total", q.total, 9331.30);
    return test_failed();
}

static int scenario_trade_unchanged(void)
{
    QUOTE q;

    calc_quote(&q, 13495.00, 'T', 0.0, 0.00);
    assert_money("trade_disc", q.trade_disc, 1349.50);
    assert_money("fleet_disc", q.fleet_disc, 0.00);
    assert_money("disc", q.disc, 0.00);
    assert_money("total", q.total, 14809.60);
    return test_failed();
}

static int scenario_add_fleet_type_f(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddfleet") != 0)
        return 1;

    next_cust_id = 2201;
    const int rc = customer_add("Greenway Fleet New", "01632 960700",
                                "newfleet@example.com", 'F');
    assert_int("customer_add rc", rc, 0);
    assert_int("cust id", cust[0].id, 2201);
    assert_char("cust type", cust[0].type, 'F');
    assert_int("next_cust_id", next_cust_id, 2202);

    save_customers();
    save_ids();
    reset_tables();
    load_all();

    assert_char("type after reload", cust[0].type, 'F');
    assert_str("name after reload", cust[0].name, "Greenway Fleet New");

    leave_temp_data_tree();
    return test_failed();
}

static int scenario_mark_existing_fleet(void)
{
    reset_tables();
    ncust = 1;
    cust[0].id = 2008;
    snprintf(cust[0].name, sizeof cust[0].name, "Greenway Taxis");
    snprintf(cust[0].phone, sizeof cust[0].phone, "01632 960702");
    snprintf(cust[0].email, sizeof cust[0].email, "fleet@greenwaytaxis.example");
    cust[0].type = 'R';

    const int rc = customer_set_type(2008, 'F');
    assert_int("customer_set_type rc", rc, 0);
    assert_char("cust type", cust[0].type, 'F');
    return test_failed();
}

typedef struct {
    const char *id;
    int (*fn)(void);
} scenario_entry;

static const scenario_entry SCENARIOS[] = {
    {"fleet_4pct_no_salesman", scenario_fleet_4pct_no_salesman},
    {"fleet_then_salesman", scenario_fleet_then_salesman},
    {"fleet_fixed_fees", scenario_fleet_fixed_fees},
    {"retail_unchanged", scenario_retail_unchanged},
    {"trade_unchanged", scenario_trade_unchanged},
    {"add_fleet_type_f", scenario_add_fleet_type_f},
    {"mark_existing_fleet", scenario_mark_existing_fleet},
};

int main(int argc, char **argv)
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
