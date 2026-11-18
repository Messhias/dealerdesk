/*
 * pricing_steps.c - @terminal steps for pricing.feature (shared quote phrases)
 */
#include "steps_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int step_list_price(const StepWorld *world, const char *arg)
{
    (void)world;
    if (!arg || !arg[0])
        return -1;
    steps_fixture()->list_price = atof(arg);
    steps_fixture()->has_list_price = 1;
    return 0;
}

static int step_retail_customer(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->cust_type = 'R';
    steps_fixture()->has_cust_type = 1;
    return 0;
}

static int step_trade_customer(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->cust_type = 'T';
    steps_fixture()->has_cust_type = 1;
    return 0;
}

static int step_salesman_discount(const StepWorld *world, const char *arg)
{
    char buf[64];
    (void)world;

    if (!arg || !arg[0])
        return -1;
    snprintf(buf, sizeof buf, "%s", arg);
    char* p = strchr(buf, '%');
    if (p)
        *p = 0;
    steps_fixture()->salesman_pct = atof(buf);
    steps_fixture()->has_salesman = 1;
    return 0;
}

static int step_trade_in(const StepWorld *world, const char *arg)
{
    (void)world;
    if (!arg || !arg[0])
        return -1;
    steps_fixture()->tradein = atof(arg);
    steps_fixture()->has_tradein = 1;
    return 0;
}

static int step_quote_calculated(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;
    return steps_open_quote_from_fixture(world);
}

static int step_trade_discount(const StepWorld *world, const char *arg)
{
    return steps_assert_discount_amount(world, "Trade discount", arg);
}

static int step_salesman_amount(const StepWorld *world, const char *arg)
{
    if (!arg || !arg[0])
        return -1;
    if (strcmp(arg, "0.00") == 0 || strcmp(arg, "0") == 0) {
        /* Salesman line omitted when disc is 0. */
        return steps_screen_line_absent(world, "Discount (");
    }
    return steps_screen_has_money(world, arg);
}

static int step_net_price(const StepWorld *world, const char *arg)
{
    return steps_screen_has_money(world, arg);
}

static int step_delivery_fee(const StepWorld *world, const char *arg)
{
    return steps_screen_has_money(world, arg);
}

static int step_vat(const StepWorld *world, const char *arg)
{
    return steps_screen_has_money(world, arg);
}

static int step_registration_fee(const StepWorld *world, const char *arg)
{
    return steps_screen_has_money(world, arg);
}

static int step_total(const StepWorld *world, const char *arg)
{
    return steps_screen_has_money(world, arg);
}

static int step_salesman_percent_used(const StepWorld *world, const char *arg)
{
    char needle[64];

    if (!arg || !arg[0])
        return -1;
    /* Quote screen prints Discount (x.x%) only when salesman disc > 0. */
    if (strcmp(arg, "0") == 0)
        return steps_screen_line_absent(world, "Discount (");
    /* Screen format: Discount (15.0%) via printf %%4.1f. */
    snprintf(needle, sizeof needle, "Discount (%.1f%%)", atof(arg));
    if (!console_saw(world->console, needle)) {
        fprintf(stderr, "salesman percent used missing: %s\n", arg);
        return -1;
    }
    return 0;
}

void pricing_steps_register(void)
{
    steps_register("a list price of {string}", step_list_price);
    steps_register("a retail customer", step_retail_customer);
    steps_register("a trade customer", step_trade_customer);
    steps_register("a salesman discount of {string}", step_salesman_discount);
    steps_register("a trade-in of {string}", step_trade_in);
    steps_register("a quote is calculated", step_quote_calculated);
    steps_register("the trade discount is {string}", step_trade_discount);
    steps_register("the salesman discount amount is {string}", step_salesman_amount);
    steps_register("the salesman discount percent used is {string}",
                   step_salesman_percent_used);
    steps_register("the net price is {string}", step_net_price);
    steps_register("the delivery fee is {string}", step_delivery_fee);
    steps_register("the VAT is {string}", step_vat);
    steps_register("the registration fee is {string}", step_registration_fee);
    steps_register("the total is {string}", step_total);
}
