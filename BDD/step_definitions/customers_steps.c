/*
 * customers_steps.c - @terminal steps for customers.feature phrases
 */
#include "steps_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int step_one_retail_customer(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->has_retail_cust = 1;
    return 0;
}

static int step_one_trade_customer(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->has_trade_cust = 1;
    return 0;
}

static int step_empty_customers(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->empty_customers = 1;
    return 0;
}

static int step_full_customers(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->full_customers = 1;
    return 0;
}

static int step_next_customer_id(const StepWorld *world, const char *arg)
{
    (void)world;
    if (!arg || !arg[0])
        return -1;
    steps_fixture()->next_cust = atoi(arg);
    steps_fixture()->has_next_cust = 1;
    return 0;
}

static int step_one_customer_named(const StepWorld *world, const char *arg)
{
    (void)world;
    if (!arg || !arg[0])
        return -1;
    snprintf(steps_fixture()->custom_cust_name,
             sizeof steps_fixture()->custom_cust_name, "%s", arg);
    steps_fixture()->custom_cust_id = 2101;
    steps_fixture()->custom_cust_type = 'R';
    steps_fixture()->has_custom_cust = 1;
    return 0;
}

static int step_two_customers_named(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    snprintf(steps_fixture()->custom_cust_name,
             sizeof steps_fixture()->custom_cust_name, "Sarah Whitfield");
    steps_fixture()->custom_cust_id = 2101;
    steps_fixture()->custom_cust_type = 'R';
    steps_fixture()->has_custom_cust = 1;
    snprintf(steps_fixture()->second_cust_name,
             sizeof steps_fixture()->second_cust_name, "Tom Bradley");
    steps_fixture()->has_second_cust = 1;
    return 0;
}

static int step_customer_list_shown(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_customers_menu(world) != 0)
        return -1;
    if (console_sendline(world->console, "1") != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;
    if (!console_saw(world->console, steps_fixture()->name_retail)) {
        fprintf(stderr, "retail customer missing after list: %s\n",
                steps_fixture()->name_retail);
        return -1;
    }
    return 0;
}

static int step_both_customers_visible(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (!console_saw(world->console, steps_fixture()->name_retail)) {
        fprintf(stderr, "retail customer missing on list\n");
        return -1;
    }
    if (!console_saw(world->console, steps_fixture()->name_trade)) {
        fprintf(stderr, "trade customer missing on list\n");
        return -1;
    }
    return 0;
}

static int add_customer_flow(const StepWorld *world, const char *name, const char *phone,
                             const char *email, const char type, const int expect_ok)
{
    char tbuf[4];

    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_customers_menu(world) != 0)
        return -1;

    const size_t mark = world->console->out_len;
    if (console_sendline(world->console, "2") != 0)
        return -1;

    /* Full table exits before name prompt. */
    if (steps_fixture()->full_customers) {
        if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
            return -1;
        if (!console_saw_from(world->console, mark, "Customer file is full")) {
            fprintf(stderr, "expected full customer table\n");
            steps_fixture()->last_op_ok = 1;
            return -1;
        }
        steps_fixture()->last_op_ok = 0;
        return steps_send_pause(world);
    }

    if (console_expect(world->console, "Name", 3000) != 0)
        return -1;
    if (console_sendline(world->console, name ? name : "") != 0)
        return -1;
    if (console_expect(world->console, "Phone", 3000) != 0)
        return -1;
    if (console_sendline(world->console, phone ? phone : "") != 0)
        return -1;
    if (console_expect(world->console, "Email", 3000) != 0)
        return -1;
    if (console_sendline(world->console, email ? email : "") != 0)
        return -1;
    if (console_expect(world->console, "Type", 3000) != 0)
        return -1;
    tbuf[0] = type;
    tbuf[1] = 0;
    if (console_sendline(world->console, tbuf) != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;

    if (expect_ok) {
        if (!console_saw_from(world->console, mark, "Customer") ||
            !console_saw_from(world->console, mark, "added")) {
            fprintf(stderr, "expected customer add success\n");
            steps_fixture()->last_op_ok = 0;
            return -1;
        }
        steps_fixture()->last_op_ok = 1;
        if (steps_fixture()->has_next_cust)
            steps_fixture()->custom_cust_id = steps_fixture()->next_cust;
    } else {
        if (!console_saw_from(world->console, mark, "Name is required")) {
            fprintf(stderr, "expected customer add failure\n");
            steps_fixture()->last_op_ok = 1;
            return -1;
        }
        steps_fixture()->last_op_ok = 0;
    }
    return steps_send_pause(world);
}

static int step_add_empty_name(const StepWorld *world, const char *arg)
{
    (void)arg;
    return add_customer_flow(world, "", "07700 900000", "x@example.com", 'R', 0);
}

static int step_add_retail(const StepWorld *world, const char *arg)
{
    (void)arg;
    return add_customer_flow(world, "Sarah Whitfield", "07700 900123",
                             "sarah@example.com", 'R', 1);
}

static int step_add_trade(const StepWorld *world, const char *arg)
{
    (void)arg;
    return add_customer_flow(world, "Fleet Motors Ltd", "01234 567890",
                             "trade@example.com", 'T', 1);
}

static int step_add_fleet(const StepWorld *world, const char *arg)
{
    (void)arg;
    return add_customer_flow(world, "Greenway Fleet New", "01632 960700",
                             "newfleet@example.com", 'F', 1);
}

static int step_add_overflow(const StepWorld *world, const char *arg)
{
    (void)arg;
    return add_customer_flow(world, "Overflow Test", "07000000000",
                             "overflow@example.com", 'R', 0);
}

static int step_add_fails(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    return steps_fixture()->last_op_ok == 0 ? 0 : -1;
}

static int step_add_succeeds(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    return steps_fixture()->last_op_ok == 1 ? 0 : -1;
}

static int step_customers_remain_empty(const StepWorld *world, const char *arg)
{
    (void)arg;
    const int n = steps_count_data_rows(world, "customers.dat");
    if (n != 0) {
        fprintf(stderr, "customer rows %d want 0\n", n);
        return -1;
    }
    return 0;
}

static int step_exactly_n_customers(const StepWorld *world, const char *arg)
{
    if (!arg || !arg[0])
        return -1;
    const int want = atoi(arg);
    const int got = steps_count_data_rows(world, "customers.dat");
    if (got != want) {
        fprintf(stderr, "customer rows %d want %d\n", got, want);
        return -1;
    }
    return 0;
}

static int step_customer_id_is(const StepWorld *world, const char *arg)
{
    char idbuf[32];

    if (!arg || !arg[0])
        return -1;
    const int want = atoi(arg);
    if (steps_customer_field(world, want, "id", idbuf, sizeof idbuf) != 0) {
        fprintf(stderr, "customer id %d missing\n", want);
        return -1;
    }
    steps_fixture()->custom_cust_id = want;
    steps_fixture()->cust_id = want;
    return 0;
}

static int cust_assert_id(void)
{
    if (steps_fixture()->has_mark_cust)
        return steps_fixture()->mark_cust_id;
    if (steps_fixture()->custom_cust_id > 0)
        return steps_fixture()->custom_cust_id;
    return steps_fixture()->cust_id;
}

static int step_type_retail(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_customer_type_in_file(world, cust_assert_id(), 'R');
}

static int step_type_trade(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_customer_type_in_file(world, cust_assert_id(), 'T');
}

static int step_type_fleet(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_customer_type_in_file(world, cust_assert_id(), 'F');
}

static int step_next_customer_id_is(const StepWorld *world, const char *arg)
{
    int got;

    if (!arg || !arg[0])
        return -1;
    const int want = atoi(arg);
    if (steps_next_id_value(world, "customer", &got) != 0)
        return -1;
    if (got != want) {
        fprintf(stderr, "next customer %d want %d\n", got, want);
        return -1;
    }
    return 0;
}

static int step_next_customer_still(const StepWorld *world, const char *arg)
{
    return step_next_customer_id_is(world, arg);
}

static int find_by_name_flow(const StepWorld *world, const char *needle)
{
    int count;

    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_customers_menu(world) != 0)
        return -1;

    const size_t mark = world->console->out_len;
    if (console_sendline(world->console, "3") != 0)
        return -1;
    if (console_expect(world->console, "Name contains", 3000) != 0)
        return -1;
    if (console_sendline(world->console, needle ? needle : "") != 0)
        return -1;
    /* Wait once: match line and pause prompt arrive together. */
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;

    const char* p = strstr(world->console->out + mark, "match(es)");
    if (!p) {
        fprintf(stderr, "match count line missing\n");
        return -1;
    }
    {
        const char *q = p;
        while (q > world->console->out + mark && *(q - 1) != '\n')
            q--;
        count = atoi(q);
    }
    steps_fixture()->last_match_count = count;
    if (steps_send_pause(world) != 0)
        return -1;
    return console_expect(world->console, "Option:", 5000);
}

static int step_find_empty_substring(const StepWorld *world, const char *arg)
{
    (void)arg;
    return find_by_name_flow(world, "");
}

static int step_find_by_name(const StepWorld *world, const char *arg)
{
    if (!arg)
        return -1;
    return find_by_name_flow(world, arg);
}

static int step_match_count(const StepWorld *world, const char *arg)
{
    (void)world;
    if (!arg || !arg[0])
        return -1;
    const int want = atoi(arg);
    if (steps_fixture()->last_match_count != want) {
        fprintf(stderr, "match count %d want %d\n",
                steps_fixture()->last_match_count, want);
        return -1;
    }
    return 0;
}

void customers_steps_register(void)
{
    steps_register("one retail customer", step_one_retail_customer);
    steps_register("one trade customer", step_one_trade_customer);
    steps_register("an empty customer table", step_empty_customers);
    steps_register("a full customer table", step_full_customers);
    steps_register("next customer id {string}", step_next_customer_id);
    steps_register("one customer named {string}", step_one_customer_named);
    steps_register("two customers named Sarah Whitfield and Tom Bradley",
                   step_two_customers_named);
    steps_register("the customer list is shown", step_customer_list_shown);
    steps_register("both customers are visible", step_both_customers_visible);
    steps_register("add is attempted with an empty name", step_add_empty_name);
    steps_register(
        "a retail customer is added with name Sarah Whitfield phone 07700 900123 "
        "email sarah@example.com",
        step_add_retail);
    steps_register(
        "a trade customer is added with name Fleet Motors Ltd phone 01234 567890 "
        "email trade@example.com",
        step_add_trade);
    steps_register(
        "a fleet customer is added with name Greenway Fleet New phone 01632 960700 "
        "email newfleet@example.com",
        step_add_fleet);
    steps_register("add is attempted with name Overflow Test", step_add_overflow);
    steps_register("the add fails", step_add_fails);
    steps_register("the add succeeds", step_add_succeeds);
    steps_register("the customer table remains empty", step_customers_remain_empty);
    steps_register("exactly {string} customer is in memory",
                   step_exactly_n_customers);
    steps_register("that customer id is {string}", step_customer_id_is);
    steps_register("that customer type is retail", step_type_retail);
    steps_register("that customer type is trade", step_type_trade);
    steps_register("that customer type is fleet", step_type_fleet);
    steps_register("next customer id is {string}", step_next_customer_id_is);
    steps_register("next customer id is still {string}", step_next_customer_still);
    steps_register("find by name uses an empty substring", step_find_empty_substring);
    steps_register("find by name uses {string}", step_find_by_name);
    steps_register("the match count is {string}", step_match_count);
}
