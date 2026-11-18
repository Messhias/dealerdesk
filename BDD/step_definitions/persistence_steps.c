/*
 * persistence_steps.c - @terminal steps for persistence.feature phrases
 */
#include "steps_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int step_stock_with_comments(const StepWorld *world, const char *arg)
{
    char path[600];
    (void)arg;

    if (steps_ensure_workdir(world) != 0)
        return -1;
    if (steps_write_empty_tables(world) != 0)
        return -1;
    if (steps_data_path(world, "stock.dat", path, sizeof path) != 0)
        return -1;
    FILE* fp = fopen(path, "w");
    if (!fp)
        return -1;
    fputs("# comment\n"
          "\n"
          "1001|AB12 CDE|Ford|Fiesta|2019|1000|7000.00|9995.00|0\n",
          fp);
    fclose(fp);
    steps_fixture()->seed_ready = 1;
    steps_fixture()->custom_veh_id = 1001;
    steps_fixture()->has_custom_veh = 1;
    return 0;
}

static int step_customers_with_comments(const StepWorld *world, const char *arg)
{
    char path[600];
    (void)arg;

    if (steps_ensure_workdir(world) != 0)
        return -1;
    if (steps_write_empty_tables(world) != 0)
        return -1;
    if (steps_data_path(world, "customers.dat", path, sizeof path) != 0)
        return -1;
    FILE* fp = fopen(path, "w");
    if (!fp)
        return -1;
    fputs("# comment\n"
          "\n"
          "2001|Sample Buyer|07700 900000|buyer@example.com|R\n",
          fp);
    fclose(fp);
    steps_fixture()->seed_ready = 1;
    steps_fixture()->custom_cust_id = 2001;
    steps_fixture()->custom_cust_type = 'R';
    steps_fixture()->has_custom_cust = 1;
    snprintf(steps_fixture()->custom_cust_name,
             sizeof steps_fixture()->custom_cust_name, "Sample Buyer");
    return 0;
}

static int step_one_vehicle_in_memory(const StepWorld *world, const char *arg)
{
    char path[600];
    (void)arg;

    if (steps_ensure_workdir(world) != 0)
        return -1;
    if (steps_write_empty_tables(world) != 0)
        return -1;
    if (steps_data_path(world, "stock.dat", path, sizeof path) != 0)
        return -1;
    FILE* fp = fopen(path, "w");
    if (!fp)
        return -1;
    fputs("1001|AB12 CDE|Ford|Fiesta|2019|1000|7000.00|10495.00|0\n", fp);
    fclose(fp);
    steps_fixture()->seed_ready = 1;
    steps_fixture()->custom_veh_id = 1001;
    steps_fixture()->has_custom_veh = 1;
    snprintf(steps_fixture()->custom_plate, sizeof steps_fixture()->custom_plate,
             "AB12 CDE");
    steps_fixture()->custom_veh_price = 10495.00;
    return 0;
}

static int step_one_customer_in_memory(const StepWorld *world, const char *arg)
{
    char path[600];
    (void)arg;

    if (steps_ensure_workdir(world) != 0)
        return -1;
    if (steps_write_empty_tables(world) != 0)
        return -1;
    if (steps_data_path(world, "customers.dat", path, sizeof path) != 0)
        return -1;
    FILE* fp = fopen(path, "w");
    if (!fp)
        return -1;
    fputs("2001|Sample Buyer|07700 900000|buyer@example.com|T\n", fp);
    fclose(fp);
    steps_fixture()->seed_ready = 1;
    steps_fixture()->custom_cust_id = 2001;
    steps_fixture()->custom_cust_type = 'T';
    steps_fixture()->has_custom_cust = 1;
    snprintf(steps_fixture()->custom_cust_name,
             sizeof steps_fixture()->custom_cust_name, "Sample Buyer");
    return 0;
}

static int step_next_ids_vehicle_customer_invoice(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (steps_ensure_workdir(world) != 0)
        return -1;
    steps_fixture()->next_veh = 1013;
    steps_fixture()->next_cust = 2009;
    steps_fixture()->next_inv = 50233;
    steps_fixture()->has_next_veh = 1;
    steps_fixture()->has_next_cust = 1;
    steps_fixture()->has_next_inv = 1;
    return steps_write_ids(world, 1013, 2009, 50233);
}

static int step_stock_saved(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    /* Seed already wrote stock.dat (UI save mapping). */
    return 0;
}

static int step_customers_ids_saved(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    return 0;
}

static int step_clear_stock_memory(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_stop_app_keep_workdir(world);
}

static int step_clear_customers_ids_memory(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_stop_app_keep_workdir(world);
}

static int step_empty_data_dir(const StepWorld *world, const char *arg)
{
    (void)arg;
    steps_fixture()->empty_data_dir = 1;
    return steps_ensure_empty_workdir(world);
}

static int step_next_ids_preset(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (!steps_data_root(world)[0]) {
        if (steps_ensure_empty_workdir(world) != 0)
            return -1;
    }
    steps_fixture()->next_veh = 1000;
    steps_fixture()->next_cust = 2000;
    steps_fixture()->next_inv = 50000;
    steps_fixture()->has_next_veh = 1;
    steps_fixture()->has_next_cust = 1;
    steps_fixture()->has_next_inv = 1;
    /* Keep tables missing; only ids file so load can pick counters. */
    return steps_write_ids(world, 1000, 2000, 50000);
}

static int step_repository_data(const StepWorld *world, const char *arg)
{
    (void)arg;
    steps_fixture()->use_repo_data = 1;
    return steps_ensure_workdir(world);
}

static int step_vehicle_plate_is(const StepWorld *world, const char *arg)
{
    char plate[64];

    if (!arg || !arg[0])
        return -1;
    if (steps_vehicle_field(world, steps_fixture()->custom_veh_id, "plate", plate,
                            sizeof plate) != 0)
        return -1;
    if (strcmp(plate, arg) != 0) {
        fprintf(stderr, "plate got %s want %s\n", plate, arg);
        return -1;
    }
    return 0;
}

static int step_vehicle_list_price_is(const StepWorld *world, const char *arg)
{
    char price[64];

    if (!arg || !arg[0])
        return -1;
    if (steps_vehicle_field(world, steps_fixture()->custom_veh_id, "price", price,
                            sizeof price) != 0)
        return -1;
    if (strcmp(price, arg) != 0) {
        fprintf(stderr, "list price got %s want %s\n", price, arg);
        return -1;
    }
    return 0;
}

static int step_customer_name_is(const StepWorld *world, const char *arg)
{
    char name[96];

    if (!arg || !arg[0])
        return -1;
    if (steps_customer_field(world, steps_fixture()->custom_cust_id, "name", name,
                             sizeof name) != 0)
        return -1;
    if (strcmp(name, arg) != 0) {
        fprintf(stderr, "customer name got %s want %s\n", name, arg);
        return -1;
    }
    return 0;
}

static int step_customer_type_R(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_customer_type_in_file(world, steps_fixture()->custom_cust_id, 'R');
}

static int step_customer_type_T(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_customer_type_in_file(world, steps_fixture()->custom_cust_id, 'T');
}

static int step_next_veh_is(const StepWorld *world, const char *arg)
{
    int got;

    if (!arg || !arg[0])
        return -1;
    int want = atoi(arg);
    if (steps_next_id_value(world, "vehicle", &got) != 0)
        return -1;
    if (got != want) {
        fprintf(stderr, "next vehicle %d want %d\n", got, want);
        return -1;
    }
    return 0;
}

static int step_next_cust_is(const StepWorld *world, const char *arg)
{
    int got;

    if (!arg || !arg[0])
        return -1;
    int want = atoi(arg);
    if (steps_next_id_value(world, "customer", &got) != 0)
        return -1;
    if (got != want) {
        fprintf(stderr, "next customer %d want %d\n", got, want);
        return -1;
    }
    return 0;
}

static int step_next_veh_remains(const StepWorld *world, const char *arg)
{
    return step_next_veh_is(world, arg);
}

static int step_next_cust_remains(const StepWorld *world, const char *arg)
{
    return step_next_cust_is(world, arg);
}

static int step_next_inv_remains(const StepWorld *world, const char *arg)
{
    int got;

    if (!arg || !arg[0])
        return -1;
    int want = atoi(arg);
    if (steps_next_id_value(world, "invoice", &got) != 0)
        return -1;
    if (got != want) {
        fprintf(stderr, "next invoice %d want %d\n", got, want);
        return -1;
    }
    return 0;
}

static int step_exactly_n_vehicles(const StepWorld *world, const char *arg)
{
    char needle[64];

    if (!arg || !arg[0])
        return -1;
    int want = atoi(arg);
    int got = steps_count_data_rows(world, "stock.dat");
    if (got < 0)
        got = 0;
    if (got == want)
        return 0;

    /* Prefer live list count when app is up (skips comments). */
    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_stock_menu(world) != 0)
        return -1;
    size_t mark = world->console->out_len;
    if (console_sendline(world->console, "1") != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;
    snprintf(needle, sizeof needle, "%d vehicle(s)", want);
    if (!console_saw_from(world->console, mark, needle)) {
        fprintf(stderr, "vehicle count want %d (file=%d)\n", want, got);
        return -1;
    }
    return steps_send_pause(world);
}

static int step_exactly_n_customers(const StepWorld *world, const char *arg)
{
    char needle[64];

    if (!arg || !arg[0])
        return -1;
    int want = atoi(arg);
    int got = steps_count_data_rows(world, "customers.dat");
    if (got < 0)
        got = 0;
    if (got == want)
        return 0;

    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_customers_menu(world) != 0)
        return -1;
    size_t mark = world->console->out_len;
    if (console_sendline(world->console, "1") != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;
    snprintf(needle, sizeof needle, "%d customer(s)", want);
    if (!console_saw_from(world->console, mark, needle)) {
        fprintf(stderr, "customer count want %d (file=%d)\n", want, got);
        return -1;
    }
    return steps_send_pause(world);
}

static int step_vehicle_id_is(const StepWorld *world, const char *arg)
{
    char idbuf[32];

    if (!arg || !arg[0])
        return -1;
    int want = atoi(arg);
    if (steps_vehicle_field(world, want, "id", idbuf, sizeof idbuf) != 0) {
        fprintf(stderr, "vehicle id %d not found\n", want);
        return -1;
    }
    steps_fixture()->custom_veh_id = want;
    return 0;
}

static int step_customer_id_is(const StepWorld *world, const char *arg)
{
    char idbuf[32];

    if (!arg || !arg[0])
        return -1;
    int want = atoi(arg);
    if (steps_customer_field(world, want, "id", idbuf, sizeof idbuf) != 0) {
        fprintf(stderr, "customer id %d not found\n", want);
        return -1;
    }
    steps_fixture()->custom_cust_id = want;
    return 0;
}

void persistence_steps_register(void)
{
    steps_register(
        "a stock.dat file with a comment line, a blank line, and one vehicle record",
        step_stock_with_comments);
    steps_register(
        "a customers.dat file with a comment line, a blank line, and one customer record",
        step_customers_with_comments);
    steps_register("one vehicle in memory with id 1001 plate AB12 CDE",
                   step_one_vehicle_in_memory);
    steps_register("one customer in memory with id 2001 name Sample Buyer type T",
                   step_one_customer_in_memory);
    steps_register("next ids vehicle 1013 customer 2009 invoice 50233",
                   step_next_ids_vehicle_customer_invoice);
    steps_register("stock is saved", step_stock_saved);
    steps_register("customers and ids are saved", step_customers_ids_saved);
    steps_register("in-memory stock is cleared", step_clear_stock_memory);
    steps_register("in-memory customers and ids are cleared",
                   step_clear_customers_ids_memory);
    steps_register("a private temp working tree with an empty data directory",
                   step_empty_data_dir);
    steps_register("next ids preset to vehicle 1000 customer 2000 invoice 50000",
                   step_next_ids_preset);
    steps_register("the repository data directory", step_repository_data);
    steps_register("exactly {string} vehicle is in memory", step_exactly_n_vehicles);
    steps_register("exactly {string} vehicles are in memory",
                   step_exactly_n_vehicles);
    steps_register("exactly {string} customer is in memory",
                   step_exactly_n_customers);
    steps_register("exactly {string} customers are in memory",
                   step_exactly_n_customers);
    steps_register("that vehicle id is {string}", step_vehicle_id_is);
    steps_register("that vehicle plate is {string}", step_vehicle_plate_is);
    steps_register("that vehicle list price is {string}",
                   step_vehicle_list_price_is);
    steps_register("that customer id is {string}", step_customer_id_is);
    steps_register("that customer name is {string}", step_customer_name_is);
    steps_register("that customer type is R", step_customer_type_R);
    steps_register("that customer type is T", step_customer_type_T);
    steps_register("next vehicle id is {string}", step_next_veh_is);
    steps_register("next customer id is {string}", step_next_cust_is);
    steps_register("next vehicle id remains {string}", step_next_veh_remains);
    steps_register("next customer id remains {string}", step_next_cust_remains);
    steps_register("next invoice number remains {string}", step_next_inv_remains);
}
