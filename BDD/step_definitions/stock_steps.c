/*
 * stock_steps.c - @terminal steps for stock.feature phrases
 */
#include "steps_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int step_one_in_stock(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->has_veh_stock = 1;
    return 0;
}

static int step_one_reserved(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->has_veh_reserved = 1;
    return 0;
}

static int step_one_sold(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->has_veh_sold = 1;
    return 0;
}

static int step_empty_stock(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->empty_stock = 1;
    return 0;
}

static int step_next_vehicle_id(const StepWorld *world, const char *arg)
{
    (void)world;
    if (!arg || !arg[0])
        return -1;
    steps_fixture()->next_veh = atoi(arg);
    steps_fixture()->has_next_veh = 1;
    return 0;
}

static int step_one_vehicle_plate(const StepWorld *world, const char *arg)
{
    (void)world;
    if (!arg || !arg[0])
        return -1;
    snprintf(steps_fixture()->custom_plate, sizeof steps_fixture()->custom_plate,
             "%s", arg);
    steps_fixture()->has_custom_plate = 1;
    steps_fixture()->custom_veh_id = 1101;
    steps_fixture()->custom_veh_status = 0;
    steps_fixture()->custom_veh_price = 10995.00;
    return 0;
}

static int step_one_in_stock_priced(const StepWorld *world, const char *arg)
{
    (void)world;
    if (!arg || !arg[0])
        return -1;
    steps_fixture()->has_veh_stock = 1;
    steps_fixture()->list_price = atof(arg);
    steps_fixture()->has_list_price = 1;
    steps_fixture()->veh_id = 1101;
    return 0;
}

static int step_stock_list_hides_sold(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_stock_menu(world) != 0)
        return -1;
    if (console_sendline(world->console, "1") != 0)
        return -1;
    /* List header and pause prompt arrive in one screen flush. */
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;
    if (!console_saw(world->console, "Stock List")) {
        fprintf(stderr, "Stock List header missing\n");
        return -1;
    }
    return 0;
}

static int step_in_stock_visible(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (!console_saw(world->console, steps_fixture()->plate_stock)) {
        fprintf(stderr, "in-stock plate missing: %s\n", steps_fixture()->plate_stock);
        return -1;
    }
    return 0;
}

static int step_reserved_visible(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (!console_saw(world->console, steps_fixture()->plate_reserved)) {
        fprintf(stderr, "reserved plate missing: %s\n",
                steps_fixture()->plate_reserved);
        return -1;
    }
    return 0;
}

static int step_sold_hidden(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (console_saw(world->console, steps_fixture()->plate_sold)) {
        fprintf(stderr, "sold plate should be hidden: %s\n",
                steps_fixture()->plate_sold);
        return -1;
    }
    return 0;
}

static int step_stock_list_shows_all(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (steps_send_pause(world) != 0)
        return -1;
    if (console_expect(world->console, "Option:", 5000) != 0)
        return -1;
    if (console_sendline(world->console, "2") != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;
    if (!console_saw(world->console, "All Vehicles")) {
        fprintf(stderr, "All Vehicles header missing\n");
        return -1;
    }
    return 0;
}

static int step_sold_visible(const StepWorld *world, const char *arg)
{
    (void)arg;
    if (!console_saw(world->console, steps_fixture()->plate_sold)) {
        fprintf(stderr, "sold plate missing on all-vehicles: %s\n",
                steps_fixture()->plate_sold);
        return -1;
    }
    return 0;
}

static int add_vehicle_flow(const StepWorld *world, const char *plate, const char *make,
                            const char *model, const char *year,
                            const char *mileage, const char *cost,
                            const char *price, const int expect_ok)
{
    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_stock_menu(world) != 0)
        return -1;

    const size_t mark = world->console->out_len;
    
    if (console_sendline(world->console, "3") != 0)
        return -1;
    
    if (console_expect(world->console, "Plate", 3000) != 0)
        return -1;
    
    if (console_sendline(world->console, plate ? plate : "") != 0)
        return -1;
    
    if (console_expect(world->console, "Make", 3000) != 0)
        return -1;
    
    if (console_sendline(world->console, make ? make : "Ford") != 0)
        return -1;
    
    if (console_expect(world->console, "Model", 3000) != 0)
        return -1;
    
    if (console_sendline(world->console, model ? model : "Fiesta") != 0)
        return -1;
    
    if (console_expect(world->console, "Year", 3000) != 0)
        return -1;
    
    if (console_sendline(world->console, year ? year : "2019") != 0)
        return -1;
    
    if (console_expect(world->console, "Mileage", 3000) != 0)
        return -1;
    
    if (console_sendline(world->console, mileage ? mileage : "1000") != 0)
        return -1;
    
    if (console_expect(world->console, "Cost", 3000) != 0)
        return -1;
    
    if (console_sendline(world->console, cost ? cost : "7000.00") != 0)
        return -1;
    
    if (console_expect(world->console, "List price", 3000) != 0)
        return -1;
    
    if (console_sendline(world->console, price ? price : "9295.00") != 0)
        return -1;
    
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;

    if (expect_ok) {
        if (!console_saw_from(world->console, mark, "Vehicle") ||
            !console_saw_from(world->console, mark, "added")) {
            fprintf(stderr, "expected add success\n");
            steps_fixture()->last_op_ok = 0;
            return -1;
        }
        steps_fixture()->last_op_ok = 1;
    } else {
        if (!console_saw_from(world->console, mark, "Plate and list price are required")) {
            fprintf(stderr, "expected add failure\n");
            steps_fixture()->last_op_ok = 1;
            return -1;
        }
        steps_fixture()->last_op_ok = 0;
    }
    
    return steps_send_pause(world);
}

static int step_add_empty_plate(const StepWorld *world, const char *arg)
{
    (void)arg;
    return add_vehicle_flow(world, "", "Ford", "Fiesta", "2019", "1000", "7000.00",
                            "10000.00", 0);
}

static int step_add_zero_price(const StepWorld *world, const char *arg)
{
    (void)arg;
    return add_vehicle_flow(world, "AB12 CDE", "Ford", "Fiesta", "2019", "1000",
                            "7000.00", "0.00", 0);
}

static int step_add_vehicle_full(const StepWorld *world, const char *arg)
{
    (void)arg;
    return add_vehicle_flow(world, "AB12 CDE", "Ford", "Fiesta", "2019", "1000",
                            "7000.00", "9295.00", 1);
}

static int step_add_fails(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    if (steps_fixture()->last_op_ok != 0) {
        fprintf(stderr, "expected add fail\n");
        return -1;
    }
    return 0;
}

static int step_add_succeeds(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    if (steps_fixture()->last_op_ok != 1) {
        fprintf(stderr, "expected add success\n");
        return -1;
    }
    return 0;
}

static int step_stock_remains_empty(const StepWorld *world, const char *arg)
{
    (void)arg;
    const int n = steps_count_data_rows(world, "stock.dat");
    if (n != 0) {
        fprintf(stderr, "stock rows %d want 0\n", n);
        return -1;
    }
    return 0;
}

static int step_vehicle_status_in_stock(const StepWorld *world, const char *arg)
{
    int id = steps_fixture()->veh_id;
    (void)arg;
    /* After a successful add, the new vehicle received the prior next id. */
    if (steps_fixture()->last_op_ok == 1 && steps_fixture()->has_next_veh)
        id = steps_fixture()->next_veh;
    else if (steps_fixture()->custom_veh_id > 0)
        id = steps_fixture()->custom_veh_id;
    return steps_stock_status_for_id(world, id, 0);
}

static int step_vehicle_status_reserved(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_stock_status_for_id(world, steps_fixture()->veh_id, 1);
}

static int step_vehicle_status_remains_sold(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_stock_status_for_id(world, 1103, 2);
}

static int step_find_by_plate(const StepWorld *world, const char *arg)
{
    char needle[64];

    if (!arg)
        return -1;
    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_stock_menu(world) != 0)
        return -1;

    const size_t mark = world->console->out_len;
    if (console_sendline(world->console, "4") != 0)
        return -1;
    if (console_expect(world->console, "Plate", 3000) != 0)
        return -1;
    if (console_sendline(world->console, arg) != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;

    snprintf(needle, sizeof needle, "No vehicle with plate %s", arg);
    if (console_saw_from(world->console, mark, needle)) {
        steps_fixture()->last_has_match = 0;
        steps_fixture()->last_match_index = -1;
    } else if (console_saw_from(world->console, mark, "ID")) {
        steps_fixture()->last_has_match = 1;
        steps_fixture()->last_match_index = 0;
    } else {
        fprintf(stderr, "find by plate produced no recognisable result\n");
        return -1;
    }
    return steps_send_pause(world);
}

static int step_first_match_index_0(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    if (!steps_fixture()->last_has_match || steps_fixture()->last_match_index != 0) {
        fprintf(stderr, "expected first match index 0\n");
        return -1;
    }
    return 0;
}

static int step_no_match(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    if (steps_fixture()->last_has_match) {
        fprintf(stderr, "expected no plate match\n");
        return -1;
    }
    return 0;
}

static int change_price_flow(const StepWorld *world, const char *new_price, const int expect_ok)
{
    char idbuf[32];

    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_stock_menu(world) != 0)
        return -1;

    snprintf(idbuf, sizeof idbuf, "%d", steps_fixture()->veh_id);
    const size_t mark = world->console->out_len;
    if (console_sendline(world->console, "5") != 0)
        return -1;
    if (console_expect(world->console, "Vehicle ID", 3000) != 0)
        return -1;
    if (console_sendline(world->console, idbuf) != 0)
        return -1;
    if (console_expect(world->console, "New price", 3000) != 0)
        return -1;
    if (console_sendline(world->console, new_price) != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;

    if (expect_ok) {
        if (!console_saw_from(world->console, mark, "Price updated")) {
            fprintf(stderr, "expected price update\n");
            steps_fixture()->last_op_ok = 0;
            return -1;
        }
        steps_fixture()->last_op_ok = 1;
    } else {
        if (!console_saw_from(world->console, mark, "Price not changed")) {
            fprintf(stderr, "expected price rejection\n");
            steps_fixture()->last_op_ok = 1;
            return -1;
        }
        steps_fixture()->last_op_ok = 0;
    }
    return steps_send_pause(world);
}

static int step_set_price(const StepWorld *world, const char *arg)
{
    if (!arg || !arg[0])
        return -1;
    if (strcmp(arg, "0.00") == 0)
        return change_price_flow(world, arg, 0);
    return change_price_flow(world, arg, 1);
}

static int step_price_change_succeeds(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    return steps_fixture()->last_op_ok == 1 ? 0 : -1;
}

static int step_price_change_rejected(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    return steps_fixture()->last_op_ok == 0 ? 0 : -1;
}

static int step_vehicle_price_is(const StepWorld *world, const char *arg)
{
    char price[64];
    const int id = steps_fixture()->veh_id;

    if (!arg || !arg[0])
        return -1;
    if (steps_vehicle_field(world, id, "price", price, sizeof price) != 0)
        return -1;
    if (strcmp(price, arg) != 0) {
        fprintf(stderr, "price got %s want %s\n", price, arg);
        return -1;
    }
    return 0;
}

static int step_vehicle_price_still(const StepWorld *world, const char *arg)
{
    return step_vehicle_price_is(world, arg);
}

static int reserve_toggle_flow(const StepWorld *world, const int expect_ok)
{
    char idbuf[32];

    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;
    if (steps_enter_stock_menu(world) != 0)
        return -1;

    const int id = steps_fixture()->has_veh_sold && !steps_fixture()->has_veh_stock
                 ? 1103
                 : 1101;
    steps_fixture()->veh_id = id;
    snprintf(idbuf, sizeof idbuf, "%d", id);

    const size_t mark = world->console->out_len;
    if (console_sendline(world->console, "6") != 0)
        return -1;
    if (console_expect(world->console, "Vehicle ID", 3000) != 0)
        return -1;
    if (console_sendline(world->console, idbuf) != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;

    if (expect_ok) {
        if (!console_saw_from(world->console, mark, "is now")) {
            fprintf(stderr, "expected reserve toggle success\n");
            steps_fixture()->last_op_ok = 0;
            return -1;
        }
        steps_fixture()->last_op_ok = 1;
    } else {
        if (!console_saw_from(world->console, mark, "already sold")) {
            fprintf(stderr, "expected reserve toggle fail\n");
            steps_fixture()->last_op_ok = 1;
            return -1;
        }
        steps_fixture()->last_op_ok = 0;
    }
    return steps_send_pause(world);
}

static int step_reserve_toggled(const StepWorld *world, const char *arg)
{
    (void)arg;
    return reserve_toggle_flow(world, steps_fixture()->has_veh_sold &&
                                              !steps_fixture()->has_veh_stock
                                          ? 0
                                          : 1);
}

static int step_reserve_toggled_again(const StepWorld *world, const char *arg)
{
    (void)arg;
    return reserve_toggle_flow(world, 1);
}

static int step_toggle_succeeds(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    return steps_fixture()->last_op_ok == 1 ? 0 : -1;
}

static int step_toggle_fails(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    return steps_fixture()->last_op_ok == 0 ? 0 : -1;
}

static int step_next_vehicle_id_is(const StepWorld *world, const char *arg)
{
    int got;

    if (!arg || !arg[0])
        return -1;
    const int want = atoi(arg);
    if (steps_next_id_value(world, "vehicle", &got) != 0)
        return -1;
    if (got != want) {
        fprintf(stderr, "next vehicle %d want %d\n", got, want);
        return -1;
    }
    return 0;
}

static int step_that_vehicle_id_is(const StepWorld *world, const char *arg)
{
    char idbuf[32];

    if (!arg || !arg[0])
        return -1;
    const int want = atoi(arg);
    if (steps_vehicle_field(world, want, "id", idbuf, sizeof idbuf) != 0) {
        fprintf(stderr, "vehicle id %d missing\n", want);
        return -1;
    }
    steps_fixture()->veh_id = want;
    steps_fixture()->custom_veh_id = want;
    return 0;
}

static int step_exactly_n_vehicles(const StepWorld *world, const char *arg)
{
    if (!arg || !arg[0])
        return -1;
    const int want = atoi(arg);
    const int got = steps_count_data_rows(world, "stock.dat");
    if (got != want) {
        fprintf(stderr, "stock rows %d want %d\n", got, want);
        return -1;
    }
    return 0;
}

void stock_steps_register(void)
{
    steps_register("one in-stock vehicle", step_one_in_stock);
    steps_register("one reserved vehicle", step_one_reserved);
    steps_register("one sold vehicle", step_one_sold);
    steps_register("an empty stock table", step_empty_stock);
    steps_register("next vehicle id {string}", step_next_vehicle_id);
    steps_register("one vehicle with plate {string}", step_one_vehicle_plate);
    steps_register("one in-stock vehicle priced {string}", step_one_in_stock_priced);
    steps_register("the stock list hides sold vehicles", step_stock_list_hides_sold);
    steps_register("the in-stock vehicle is visible", step_in_stock_visible);
    steps_register("the reserved vehicle is visible", step_reserved_visible);
    steps_register("the sold vehicle is hidden", step_sold_hidden);
    steps_register("the stock list shows all vehicles", step_stock_list_shows_all);
    steps_register("the sold vehicle is visible", step_sold_visible);
    steps_register("add is attempted with an empty plate and list price 10000.00",
                   step_add_empty_plate);
    steps_register("add is attempted with plate AB12 CDE and list price 0.00",
                   step_add_zero_price);
    steps_register(
        "a vehicle is added with plate AB12 CDE make Ford model Fiesta year 2019 "
        "mileage 1000 cost 7000.00 list price 9295.00",
        step_add_vehicle_full);
    steps_register("the add fails", step_add_fails);
    steps_register("the add succeeds", step_add_succeeds);
    steps_register("the stock table remains empty", step_stock_remains_empty);
    steps_register("exactly {string} vehicle is in memory", step_exactly_n_vehicles);
    steps_register("that vehicle id is {string}", step_that_vehicle_id_is);
    steps_register("that vehicle status is in stock", step_vehicle_status_in_stock);
    steps_register("that vehicle status is reserved", step_vehicle_status_reserved);
    steps_register("that vehicle status remains sold",
                   step_vehicle_status_remains_sold);
    steps_register("next vehicle id is {string}", step_next_vehicle_id_is);
    steps_register("find by plate uses {string}", step_find_by_plate);
    steps_register("the first match index is 0", step_first_match_index_0);
    steps_register("there is no match", step_no_match);
    steps_register("the list price is set to {string}", step_set_price);
    steps_register("the price change succeeds", step_price_change_succeeds);
    steps_register("the price change is rejected", step_price_change_rejected);
    /* Register "still" before the generic price phrase (prefix match). */
    steps_register("that vehicle price is still {string}", step_vehicle_price_still);
    steps_register("that vehicle price is {string}", step_vehicle_price_is);
    steps_register("reserve is toggled", step_reserve_toggled);
    steps_register("reserve is toggled again", step_reserve_toggled_again);
    steps_register("the toggle succeeds", step_toggle_succeeds);
    steps_register("the toggle fails", step_toggle_fails);
}
