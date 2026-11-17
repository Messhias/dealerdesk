/*
 * CTest coverage for features/stock.feature (production stock seams).
 * Scenario titles map 1:1 via CTest NAME + argv dispatch.
 *
 * Calls stock.c rules used by main.c screens (no stdin, no mirrors).
 * Mutating round-trips chdir into a private temp tree; dbfile keeps
 * its fixed "data/" path.
 */
#include <stdio.h>
#include <string.h>

#include "dbfile.h"
#include "globals.h"
#include "stock.h"
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

static void seed_one_vehicle(int id, const char *plate, double price, int status)
{
    nveh = 1;
    veh[0].id = id;
    snprintf(veh[0].plate, sizeof veh[0].plate, "%s", plate);
    snprintf(veh[0].make, sizeof veh[0].make, "Ford");
    snprintf(veh[0].model, sizeof veh[0].model, "Fiesta");
    veh[0].year = 2019;
    veh[0].mileage = 1000;
    veh[0].cost = 7000.00;
    veh[0].price = price;
    veh[0].status = status;
}

static int scenario_add_requires_plate_price(void)
{
    reset_tables();
    int rc = stock_add("", "Ford", "Fiesta", 2019, 1000, 7000.00, 10000.00);
    assert_int("empty plate rc", rc, -1);
    assert_int("nveh after empty plate", nveh, 0);

    rc = stock_add("AB12 CDE", "Ford", "Fiesta", 2019, 1000, 7000.00, 0.00);
    assert_int("zero price rc", rc, -1);
    assert_int("nveh after zero price", nveh, 0);
    return test_failed();
}

static int scenario_valid_add(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddstock") != 0)
        return 1;

    next_veh_id = 1013;
    const int rc = stock_add("AB12 CDE", "Ford", "Fiesta", 2019, 1000, 7000.00, 9295.00);
    assert_int("stock_add rc", rc, 0);
    assert_int("nveh", nveh, 1);
    assert_int("veh id", veh[0].id, 1013);
    assert_int("veh status", veh[0].status, ST_STOCK);
    assert_int("next_veh_id", next_veh_id, 1014);

    save_stock();
    save_ids();
    reset_tables();
    load_all();

    assert_int("nveh after reload", nveh, 1);
    assert_int("id after reload", veh[0].id, 1013);
    assert_int("status after reload", veh[0].status, ST_STOCK);
    assert_int("next after reload", next_veh_id, 1014);

    leave_temp_data_tree();
    return test_failed();
}

static int scenario_find_exact_case(void)
{
    reset_tables();
    seed_one_vehicle(1002, "yf20 hbn", 10995.00, ST_STOCK);
    assert_int("exact match", stock_find_first_by_plate("yf20 hbn"), 0);
    assert_int("wrong case", stock_find_first_by_plate("YF20 HBN"), -1);
    return test_failed();
}

static int scenario_change_price_positive(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddstock") != 0)
        return 1;

    seed_one_vehicle(1002, "yf20 hbn", 10995.00, ST_STOCK);
    const int changed = stock_set_price(&veh[0], 11495.00);
    assert_int("changed", changed, 1);
    assert_money("new price", veh[0].price, 11495.00);

    save_stock();
    reset_tables();
    load_all();
    assert_money("price after reload", veh[0].price, 11495.00);

    leave_temp_data_tree();
    return test_failed();
}

static int scenario_change_price_rejected(void)
{
    reset_tables();
    seed_one_vehicle(1002, "yf20 hbn", 10995.00, ST_STOCK);
    const int changed = stock_set_price(&veh[0], 0.00);
    assert_int("changed", changed, 0);
    assert_money("price unchanged", veh[0].price, 10995.00);
    return test_failed();
}

static int scenario_reserve_toggle(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddstock") != 0)
        return 1;

    seed_one_vehicle(1003, "BD68 KWP", 15995.00, ST_STOCK);
    int rc = stock_toggle_reserve(&veh[0]);
    assert_int("to reserved rc", rc, 0);
    assert_int("status reserved", veh[0].status, ST_RESERVED);

    rc = stock_toggle_reserve(&veh[0]);
    assert_int("to stock rc", rc, 0);
    assert_int("status stock", veh[0].status, ST_STOCK);

    save_stock();
    reset_tables();
    load_all();
    assert_int("status after reload", veh[0].status, ST_STOCK);

    leave_temp_data_tree();
    return test_failed();
}

static int scenario_sold_cannot_reserve(void)
{
    reset_tables();
    seed_one_vehicle(1001, "LK19 XRT", 10495.00, ST_SOLD);
    const int rc = stock_toggle_reserve(&veh[0]);
    assert_int("toggle sold rc", rc, -1);
    assert_int("status still sold", veh[0].status, ST_SOLD);
    return test_failed();
}

static int scenario_list_omits_sold(void)
{
    reset_tables();
    nveh = 3;
    veh[0].id = 1;
    veh[0].status = ST_STOCK;
    veh[1].id = 2;
    veh[1].status = ST_RESERVED;
    veh[2].id = 3;
    veh[2].status = ST_SOLD;

    assert_int("stock visible hide", stock_visible_in_list(&veh[0], 0), 1);
    assert_int("reserved visible hide", stock_visible_in_list(&veh[1], 0), 1);
    assert_int("sold hidden", stock_visible_in_list(&veh[2], 0), 0);
    assert_int("sold visible all", stock_visible_in_list(&veh[2], 1), 1);
    return test_failed();
}

typedef struct {
    const char *id;
    int (*fn)(void);
} scenario_entry;

static const scenario_entry SCENARIOS[] = {
    {"add_requires_plate_price", scenario_add_requires_plate_price},
    {"valid_add", scenario_valid_add},
    {"find_exact_case", scenario_find_exact_case},
    {"change_price_positive", scenario_change_price_positive},
    {"change_price_rejected", scenario_change_price_rejected},
    {"reserve_toggle", scenario_reserve_toggle},
    {"sold_cannot_reserve", scenario_sold_cannot_reserve},
    {"list_omits_sold", scenario_list_omits_sold},
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
