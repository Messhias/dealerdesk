/*
 * CTest coverage for features/customers.feature (production customer seams).
 * Scenario titles map 1:1 via CTest NAME + argv dispatch.
 *
 * Calls customer.c rules used by main.c screens (no stdin, no mirrors).
 * Mutating round-trips chdir into a private temp tree; dbfile keeps
 * its fixed "data/" path.
 */
#include <stdio.h>
#include <string.h>

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

static void seed_one_customer(int id, const char *name, char type)
{
    ncust = 1;
    cust[0].id = id;
    snprintf(cust[0].name, sizeof cust[0].name, "%s", name);
    snprintf(cust[0].phone, sizeof cust[0].phone, "07700 900000");
    snprintf(cust[0].email, sizeof cust[0].email, "a@example.com");
    cust[0].type = type;
}

static int scenario_add_requires_name(void)
{
    reset_tables();
    const int rc = customer_add("", "07700 900123", "sarah@example.com", 0);
    assert_int("empty name rc", rc, -1);
    assert_int("ncust after empty name", ncust, 0);
    return test_failed();
}

static int scenario_valid_add_retail(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddcust") != 0)
        return 1;

    next_cust_id = 2101;
    const int rc = customer_add("Sarah Whitfield", "07700 900123",
                                "sarah@example.com", 0);
    assert_int("customer_add rc", rc, 0);
    assert_int("ncust", ncust, 1);
    assert_int("cust id", cust[0].id, 2101);
    assert_char("cust type", cust[0].type, 'R');
    assert_int("next_cust_id", next_cust_id, 2102);
    assert_str("name", cust[0].name, "Sarah Whitfield");
    assert_str("phone", cust[0].phone, "07700 900123");
    assert_str("email", cust[0].email, "sarah@example.com");

    save_customers();
    save_ids();
    reset_tables();
    load_all();

    assert_int("ncust after reload", ncust, 1);
    assert_int("id after reload", cust[0].id, 2101);
    assert_char("type after reload", cust[0].type, 'R');
    assert_int("next after reload", next_cust_id, 2102);

    leave_temp_data_tree();
    return test_failed();
}

static int scenario_valid_add_trade(void)
{
    reset_tables();
    if (enter_temp_data_tree("ddcust") != 0)
        return 1;

    next_cust_id = 2105;
    const int rc = customer_add("Fleet Motors Ltd", "01234 567890",
                                "trade@example.com", 1);
    assert_int("customer_add rc", rc, 0);
    assert_int("cust id", cust[0].id, 2105);
    assert_char("cust type", cust[0].type, 'T');
    assert_int("next_cust_id", next_cust_id, 2106);

    save_customers();
    save_ids();
    reset_tables();
    load_all();

    assert_char("type after reload", cust[0].type, 'T');
    assert_str("name after reload", cust[0].name, "Fleet Motors Ltd");

    leave_temp_data_tree();
    return test_failed();
}

static int scenario_add_full_table(void)
{
    int i;

    reset_tables();
    ncust = MAX_CUST;
    for (i = 0; i < MAX_CUST; i++) {
        cust[i].id = 2000 + i;
        snprintf(cust[i].name, sizeof cust[i].name, "Cust %d", i);
        cust[i].type = 'R';
    }
    next_cust_id = 2999;

    const int rc = customer_add("Overflow Test", "1", "o@example.com", 0);
    assert_int("full table rc", rc, -1);
    assert_int("ncust unchanged", ncust, MAX_CUST);
    assert_int("next_cust_id unchanged", next_cust_id, 2999);
    return test_failed();
}

static int scenario_find_case_sensitive(void)
{
    reset_tables();
    seed_one_customer(2001, "Sarah Whitfield", 'R');
    assert_int("Whit matches", customer_count_name_matches("Whit"), 1);
    assert_int("whit no match", customer_count_name_matches("whit"), 0);
    return test_failed();
}

static int scenario_find_empty_matches_all(void)
{
    reset_tables();
    ncust = 2;
    cust[0].id = 2001;
    snprintf(cust[0].name, sizeof cust[0].name, "Sarah Whitfield");
    cust[0].type = 'R';
    cust[1].id = 2002;
    snprintf(cust[1].name, sizeof cust[1].name, "Tom Bradley");
    cust[1].type = 'T';

    assert_int("empty needle", customer_count_name_matches(""), 2);
    return test_failed();
}

static int scenario_find_no_match(void)
{
    reset_tables();
    seed_one_customer(2001, "Sarah Whitfield", 'R');
    assert_int("no match", customer_count_name_matches("ZZZZ"), 0);
    return test_failed();
}

static int scenario_list_includes_all(void)
{
    reset_tables();
    ncust = 2;
    cust[0].id = 1;
    cust[0].type = 'R';
    cust[1].id = 2;
    cust[1].type = 'T';

    assert_int("retail visible", customer_visible_in_list(&cust[0]), 1);
    assert_int("trade visible", customer_visible_in_list(&cust[1]), 1);
    assert_int("null hidden", customer_visible_in_list(NULL), 0);
    return test_failed();
}

typedef struct {
    const char *id;
    int (*fn)(void);
} scenario_entry;

static const scenario_entry SCENARIOS[] = {
    {"add_requires_name", scenario_add_requires_name},
    {"valid_add_retail", scenario_valid_add_retail},
    {"valid_add_trade", scenario_valid_add_trade},
    {"add_full_table", scenario_add_full_table},
    {"find_case_sensitive", scenario_find_case_sensitive},
    {"find_empty_matches_all", scenario_find_empty_matches_all},
    {"find_no_match", scenario_find_no_match},
    {"list_includes_all", scenario_list_includes_all},
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
