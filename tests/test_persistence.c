/*
 * CTest coverage for BDD/features/persistence.feature (dbfile load/save).
 * Scenario titles map 1:1 via CTest NAME + argv dispatch.
 *
 * Production dbfile keeps a fixed "data/" path. Tests chdir into a private
 * temp tree with its own data/ so repo sample files are never written.
 */
#include <stdio.h>
#include <string.h>

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

static void write_data_file(const char *name, const char *contents)
{
    char path[320];
    
    snprintf(path, sizeof path, "data/%s", name);
    
    FILE* f = fopen(path, "w");
    
    if (!f) {
        perror(path);
        g_failures++;
        return;
    }
    
    fputs(contents, f);
    fclose(f);
}

static int scenario_skip_stock_comments(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddpersist") != 0)
        return 1;
    write_data_file("stock.dat",
                    "# comment\n"
                    "\n"
                    "1001|AB12 CDE|Ford|Fiesta|2019|1000|7000.00|9995.00|0\n");
    load_all();
    assert_int("nveh", nveh, 1);
    assert_int("veh[0].id", veh[0].id, 1001);
    leave_temp_data_tree();
    return test_failed();
}

static int scenario_skip_customer_comments(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddpersist") != 0)
        return 1;
    write_data_file("customers.dat",
                    "# comment\n"
                    "\n"
                    "2001|Sample Buyer|07700 900000|buyer@example.com|R\n");
    load_all();
    assert_int("ncust", ncust, 1);
    assert_int("cust[0].id", cust[0].id, 2001);
    assert_char("cust[0].type", cust[0].type, 'R');
    leave_temp_data_tree();
    return test_failed();
}

static int scenario_stock_round_trip(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddpersist") != 0)
        return 1;

    nveh = 1;
    veh[0].id = 1001;
    snprintf(veh[0].plate, sizeof veh[0].plate, "AB12 CDE");
    snprintf(veh[0].make, sizeof veh[0].make, "Ford");
    snprintf(veh[0].model, sizeof veh[0].model, "Fiesta");
    veh[0].year = 2019;
    veh[0].mileage = 1000;
    veh[0].cost = 7000.00;
    veh[0].price = 10495.00;
    veh[0].status = 0;

    save_stock();
    memset(veh, 0, sizeof veh);
    nveh = 0;
    load_all();

    assert_int("nveh", nveh, 1);
    assert_str("plate", veh[0].plate, "AB12 CDE");
    assert_money("price", veh[0].price, 10495.00);
    leave_temp_data_tree();
    return test_failed();
}

static int scenario_customers_ids_round_trip(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddpersist") != 0)
        return 1;

    ncust = 1;
    cust[0].id = 2001;
    snprintf(cust[0].name, sizeof cust[0].name, "Sample Buyer");
    snprintf(cust[0].phone, sizeof cust[0].phone, "07700 900000");
    snprintf(cust[0].email, sizeof cust[0].email, "buyer@example.com");
    cust[0].type = 'T';
    next_veh_id = 1013;
    next_cust_id = 2009;
    next_inv_no = 50233;

    save_customers();
    save_ids();

    memset(cust, 0, sizeof cust);
    ncust = 0;
    next_veh_id = 0;
    next_cust_id = 0;
    next_inv_no = 0;

    load_all();

    assert_int("ncust", ncust, 1);
    assert_str("name", cust[0].name, "Sample Buyer");
    assert_char("type", cust[0].type, 'T');
    assert_int("next_veh_id", next_veh_id, 1013);
    assert_int("next_cust_id", next_cust_id, 2009);
    assert_int("next_inv_no", next_inv_no, 50233);
    leave_temp_data_tree();
    return test_failed();
}

static int scenario_missing_files(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddpersist") != 0)
        return 1;

    next_veh_id = 1000;
    next_cust_id = 2000;
    next_inv_no = 50000;

    load_all();

    assert_int("nveh", nveh, 0);
    assert_int("ncust", ncust, 0);
    assert_int("ninv", ninv, 0);
    assert_int("next_veh_id", next_veh_id, 1000);
    assert_int("next_cust_id", next_cust_id, 2000);
    assert_int("next_inv_no", next_inv_no, 50000);
    leave_temp_data_tree();
    return test_failed();
}

static int scenario_sample_repo_data(void)
{
    reset_tables();
    load_all();

    assert_int("nveh", nveh, 12);
    assert_int("ncust", ncust, 8);
    assert_int("ninv", ninv, 3);
    assert_int("next_veh_id", next_veh_id, 1013);
    assert_int("next_cust_id", next_cust_id, 2009);
    assert_int("next_inv_no", next_inv_no, 50233);
    return test_failed();
}

typedef struct {
    const char *id;
    int (*fn)(void);
} scenario_entry;

static const scenario_entry SCENARIOS[] = {
    {"skip_stock_comments", scenario_skip_stock_comments},
    {"skip_customer_comments", scenario_skip_customer_comments},
    {"stock_round_trip", scenario_stock_round_trip},
    {"customers_ids_round_trip", scenario_customers_ids_round_trip},
    {"missing_files", scenario_missing_files},
    {"sample_repo_data", scenario_sample_repo_data},
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
