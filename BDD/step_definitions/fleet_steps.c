/*
 * fleet_steps.c - @terminal steps for fleet.feature phrases
 */
#include "steps_common.h"

#include <stdio.h>
#include <string.h>

static int step_fleet_customer(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->cust_type = 'F';
    steps_fixture()->has_cust_type = 1;
    return 0;
}

static int step_fleet_discount(const StepWorld *world, const char *arg)
{
    return steps_assert_discount_amount(world, "Fleet discount", arg);
}

static int step_greenway_retail(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->has_mark_cust = 1;
    steps_fixture()->mark_cust_id = 2008;
    snprintf(steps_fixture()->name_mark, sizeof steps_fixture()->name_mark,
             "Greenway Taxis");
    return 0;
}

static int step_set_type_fleet(const StepWorld *world, const char *arg)
{
    char idbuf[32];
    (void)arg;

    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;

    snprintf(idbuf, sizeof idbuf, "%d", steps_fixture()->mark_cust_id);
    if (console_sendline(world->console, "2") != 0)
        return -1;
    if (console_expect(world->console, "Customers", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "4") != 0)
        return -1;
    if (console_expect(world->console, "Customer ID", 3000) != 0)
        return -1;
    if (console_sendline(world->console, idbuf) != 0)
        return -1;
    if (console_expect(world->console, "New type", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "F") != 0)
        return -1;
    if (console_expect(world->console, "is now Fleet", 3000) != 0)
        return -1;
    return 0;
}

static int step_type_is_fleet(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_customer_type_in_file(world, steps_fixture()->mark_cust_id, 'F');
}

void fleet_steps_register(void)
{
    steps_register("a fleet customer", step_fleet_customer);
    steps_register("the fleet discount is {string}", step_fleet_discount);
    steps_register("one retail customer named Greenway Taxis with id 2008",
                   step_greenway_retail);
    steps_register("that customer type is set to fleet", step_set_type_fleet);
    steps_register("that customer type is fleet", step_type_is_fleet);
}
