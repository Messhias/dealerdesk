/*
 * CTest coverage for BDD/features/pricing.feature (rnd2 / calc_quote).
 * Scenario titles map 1:1 via CTest NAME + argv dispatch.
 */
#include <stdio.h>
#include <string.h>

#include "calc.h"
#include "globals.h"
#include "test_common.h"

/* Linked because test_common.reset_tables touches these tables. */
VEHICLE  veh[MAX_VEH];
int      nveh;
CUSTOMER cust[MAX_CUST];
int      ncust;
INVOICE  inv[MAX_INV];
int      ninv;
int      next_veh_id;
int      next_cust_id;
int      next_inv_no;

static int scenario_retail_50230(void)
{
    QUOTE q;

    calc_quote(&q, 9295.00, 'R', 5.0, 1500.00);
    assert_money("trade_disc", q.trade_disc, 0.00);
    assert_money("disc", q.disc, 464.75);
    assert_money("net", q.net, 8830.25);
    assert_money("delivery", q.delivery, 150.00);
    assert_money("vat", q.vat, 1796.05);
    assert_money("reg_fee", q.reg_fee, 55.00);
    assert_money("total", q.total, 9331.30);
    return test_failed();
}

static int scenario_trade_50231(void)
{
    QUOTE q;

    calc_quote(&q, 13495.00, 'T', 0.0, 0.00);
    assert_money("trade_disc", q.trade_disc, 1349.50);
    assert_money("disc", q.disc, 0.00);
    assert_money("net", q.net, 12145.50);
    assert_money("delivery", q.delivery, 150.00);
    assert_money("vat", q.vat, 2459.10);
    assert_money("reg_fee", q.reg_fee, 55.00);
    assert_money("total", q.total, 14809.60);
    return test_failed();
}

static int scenario_retail_50232(void)
{
    QUOTE q;

    calc_quote(&q, 10495.00, 'R', 8.0, 0.00);
    assert_money("trade_disc", q.trade_disc, 0.00);
    assert_money("disc", q.disc, 839.60);
    assert_money("net", q.net, 9655.40);
    assert_money("delivery", q.delivery, 150.00);
    assert_money("vat", q.vat, 1961.08);
    assert_money("reg_fee", q.reg_fee, 55.00);
    assert_money("total", q.total, 11821.48);
    return test_failed();
}

static int scenario_salesman_below_0(void)
{
    QUOTE q;

    calc_quote(&q, 1000.00, 'R', -5.0, 0.00);
    assert_money("disc_pct", q.disc_pct, 0.0);
    assert_money("disc", q.disc, 0.00);
    assert_money("net", q.net, 1000.00);
    assert_money("total", q.total, 1435.00);
    return test_failed();
}

static int scenario_salesman_above_15(void)
{
    QUOTE q;

    calc_quote(&q, 1000.00, 'R', 40.0, 0.00);
    assert_money("disc_pct", q.disc_pct, 15.0);
    assert_money("disc", q.disc, 150.00);
    assert_money("net", q.net, 850.00);
    assert_money("total", q.total, 1255.00);
    return test_failed();
}

static int scenario_trade_then_salesman(void)
{
    QUOTE q;

    calc_quote(&q, 10000.00, 'T', 5.0, 0.00);
    assert_money("trade_disc", q.trade_disc, 1000.00);
    assert_money("disc", q.disc, 450.00);
    assert_money("net", q.net, 8550.00);
    assert_money("vat", q.vat, 1740.00);
    assert_money("total", q.total, 10495.00);
    return test_failed();
}

static int scenario_money_round(void)
{
    QUOTE q;

    calc_quote(&q, 1234.56, 'T', 0.0, 0.00);
    assert_money("trade_disc", q.trade_disc, 123.46);
    assert_money("net", q.net, 1111.10);
    assert_money("vat", q.vat, 252.22);
    assert_money("total", q.total, 1568.32);
    return test_failed();
}

static int scenario_fixed_fees(void)
{
    QUOTE q;

    calc_quote(&q, 9295.00, 'R', 5.0, 1500.00);
    assert_money("delivery", q.delivery, 150.00);
    assert_money("reg_fee", q.reg_fee, 55.00);
    return test_failed();
}

static int scenario_rnd2_unit(void)
{
    /* Prefer values that stay stable in binary floating point. */
    assert_money("rnd2_123.456", rnd2(123.456), 123.46);
    assert_money("rnd2_123.454", rnd2(123.454), 123.45);
    assert_money("rnd2_0.005", rnd2(0.005), 0.01);
    assert_money("rnd2_2.5", rnd2(2.5), 2.50);
    return test_failed();
}

typedef struct {
    const char *id;
    int (*fn)(void);
} scenario_entry;

static const scenario_entry SCENARIOS[] = {
    {"retail_50230", scenario_retail_50230},
    {"trade_50231", scenario_trade_50231},
    {"retail_50232", scenario_retail_50232},
    {"salesman_below_0", scenario_salesman_below_0},
    {"salesman_above_15", scenario_salesman_above_15},
    {"trade_then_salesman", scenario_trade_then_salesman},
    {"money_round", scenario_money_round},
    {"fixed_fees", scenario_fixed_fees},
    {"rnd2", scenario_rnd2_unit},
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
