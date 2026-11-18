/*
 * steps_common.h - shared helpers for @terminal step definition files
 */
#ifndef STEPS_COMMON_H
#define STEPS_COMMON_H

#include "steps.h"

typedef struct {
    double list_price;
    int has_list_price;
    char cust_type; /* R, T, or F */
    int has_cust_type;
    double salesman_pct;
    int has_salesman;
    double tradein;
    int has_tradein;

    int veh_id;
    int cust_id;
    int next_inv;
    int next_veh;
    int next_cust;
    int has_next_veh;
    int has_next_cust;
    int has_next_inv;

    char plate_stock[32];
    char plate_reserved[32];
    char plate_sold[32];
    int has_veh_stock;
    int has_veh_reserved;
    int has_veh_sold;

    char name_retail[64];
    char name_trade[64];
    char name_fleet[64];
    char name_mark[64];
    int mark_cust_id;
    int has_retail_cust;
    int has_trade_cust;
    int has_fleet_cust;
    int has_mark_cust;

    int empty_stock;
    int empty_customers;
    int full_customers;
    int empty_data_dir;
    int use_repo_data;
    int seed_ready; /* files already written; flush is a no-op */

    char custom_plate[32];
    int has_custom_plate;
    int custom_veh_id;
    int custom_veh_status;
    double custom_veh_price;
    int has_custom_veh;

    char custom_cust_name[64];
    int custom_cust_id;
    char custom_cust_type;
    int has_custom_cust;
    int has_second_cust;
    char second_cust_name[64];

    int last_op_ok; /* 1 ok, 0 fail, -1 unset */
    int last_match_count;
    int last_match_index;
    int last_has_match;
    int quote_eligible; /* 1 can, 0 cannot, -1 unset */

    int quote_ready; /* set after "a quote is calculated" / sale path */
    int app_started;
    int in_stock_menu;
    int in_cust_menu;
} TerminalFixture;

void steps_begin_scenario(void);
void steps_reset_workdir_flag(void);
TerminalFixture *steps_fixture(void);

int steps_ensure_workdir(const StepWorld *world);
int steps_ensure_empty_workdir(const StepWorld *world);
const char *steps_data_root(const StepWorld *world);
int steps_write_ids(const StepWorld *world, int next_veh, int next_cust, int next_inv);
int steps_write_empty_tables(const StepWorld *world);
int steps_data_path(const StepWorld *world, const char *name, char *out, size_t outlen);
int steps_count_data_rows(const StepWorld *world, const char *filename);
int steps_next_id_value(const StepWorld *world, const char *key, int *out);
int steps_vehicle_field(const StepWorld *world, int id, const char *field, char *out,
                        size_t outlen);
int steps_customer_field(const StepWorld *world, int id, const char *field, char *out,
                         size_t outlen);

int steps_send_pause(const StepWorld *world);
int steps_wait_main_menu_ready(const StepWorld *world);
int steps_dismiss_to_main(const StepWorld *world);
int steps_ensure_app(const StepWorld *world);
int steps_stop_app_keep_workdir(const StepWorld *world);
int steps_restart_app(const StepWorld *world);
int steps_flush_seed(const StepWorld *world);
int steps_quote_or_sell_inputs(const StepWorld *world, const char *menu,
                               const char *veh_id, const char *cust_id,
                               const char *disc, const char *tradein);
int steps_open_quote_from_fixture(const StepWorld *world);
int steps_enter_stock_menu(const StepWorld *world);
int steps_enter_customers_menu(const StepWorld *world);
int steps_screen_has_money(const StepWorld *world, const char *amount);
int steps_screen_line_absent(const StepWorld *world, const char *needle);
int steps_assert_discount_amount(const StepWorld *world, const char *label,
                                 const char *amount);
int steps_invoices_file_contains(const StepWorld *world, const char *needle);
int steps_count_invoice_rows(const StepWorld *world);
int steps_stock_status_for_id(const StepWorld *world, int id, int want_status);
int steps_next_invoice_is(const StepWorld *world, int want);
int steps_customer_type_in_file(const StepWorld *world, int id, char want_type);

#endif
