/*
 * steps_common.c - shared @terminal helpers (no invented Gherkin phrases)
 */
#include "steps_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_workdir_ready;
static TerminalFixture g_fx;

void steps_reset_workdir_flag(void)
{
    g_workdir_ready = 0;
}

void steps_begin_scenario(void)
{
    memset(&g_fx, 0, sizeof g_fx);
    g_workdir_ready = 0;
    g_fx.veh_id = 1101;
    g_fx.cust_id = 2101;
    g_fx.next_inv = 50233;
    g_fx.next_veh = 1102;
    g_fx.next_cust = 2102;
    g_fx.last_op_ok = -1;
    g_fx.last_match_index = -1;
    g_fx.quote_eligible = -1;
    snprintf(g_fx.plate_stock, sizeof g_fx.plate_stock, "AA11 AAA");
    snprintf(g_fx.plate_reserved, sizeof g_fx.plate_reserved, "BB22 BBB");
    snprintf(g_fx.plate_sold, sizeof g_fx.plate_sold, "CC33 CCC");
    snprintf(g_fx.name_retail, sizeof g_fx.name_retail, "Retail One");
    snprintf(g_fx.name_trade, sizeof g_fx.name_trade, "Trade One Ltd");
    snprintf(g_fx.name_fleet, sizeof g_fx.name_fleet, "Fleet One Ltd");
}

TerminalFixture *steps_fixture(void)
{
    return &g_fx;
}

int steps_ensure_workdir(const StepWorld *world)
{
    if (!world || !world->console)
        return -1;
    
    Console* c = world->console;
    
    if (g_workdir_ready)
        return 0;
    if (c->workdir[0]) {
        g_workdir_ready = 1;
        return 0;
    }
    
    if (console_prepare_workdir(c->workdir, sizeof c->workdir, world->repo_root) != 0)
        return -1;
    
    g_workdir_ready = 1;
    
    return 0;
}

int steps_ensure_empty_workdir(const StepWorld *world)
{
    if (!world || !world->console)
        return -1;
    
    Console* c = world->console;
    
    if (g_workdir_ready && c->workdir[0])
        return 0;
    
    if (console_prepare_empty_workdir(c->workdir, sizeof c->workdir) != 0)
        return -1;
    
    g_workdir_ready = 1;
    
    return 0;
}

const char *steps_data_root(const StepWorld *world)
{
    if (!world || !world->console)
        return "";
    
    return world->console->workdir;
}

int steps_data_path(const StepWorld *world, const char *name, char *out, const size_t outlen)
{
    if (!out || outlen < 8 || !name)
        return -1;
    
    snprintf(out, outlen, "%s/data/%s", steps_data_root(world), name);
    
    return 0;
}

int steps_write_ids(const StepWorld *world, const int next_veh, const int next_cust, const int next_inv)
{
    char path[600];

    if (steps_data_path(world, "next_ids.dat", path, sizeof path) != 0)
        return -1;
    FILE* fp = fopen(path, "w");
    if (!fp)
        return -1;
    fprintf(fp, "vehicle=%d\ncustomer=%d\ninvoice=%d\n", next_veh, next_cust,
            next_inv);
    fclose(fp);
    return 0;
}

static int write_empty_file(const char *path)
{
    FILE *fp = fopen(path, "w");
    if (!fp)
        return -1;
    fclose(fp);
    return 0;
}

int steps_write_empty_tables(const StepWorld *world)
{
    char path[600];

    if (steps_data_path(world, "stock.dat", path, sizeof path) != 0)
        return -1;
    if (write_empty_file(path) != 0)
        return -1;
    if (steps_data_path(world, "customers.dat", path, sizeof path) != 0)
        return -1;
    if (write_empty_file(path) != 0)
        return -1;
    if (steps_data_path(world, "invoices.dat", path, sizeof path) != 0)
        return -1;
    if (write_empty_file(path) != 0)
        return -1;
    return 0;
}

int steps_count_data_rows(const StepWorld *world, const char *filename)
{
    char path[600];
    char line[512];
    int n = 0;

    if (steps_data_path(world, filename, path, sizeof path) != 0)
        return -1;
    FILE* fp = fopen(path, "r");
    if (!fp)
        return 0;
    while (fgets(line, sizeof line, fp)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r' || line[0] == 0)
            continue;
        n++;
    }
    fclose(fp);
    return n;
}

int steps_next_id_value(const StepWorld *world, const char *key, int *out)
{
    char path[600];
    char line[128];
    char prefix[32];

    if (!out || !key)
        return -1;
    snprintf(prefix, sizeof prefix, "%s=", key);
    size_t n = strlen(prefix);
    if (steps_data_path(world, "next_ids.dat", path, sizeof path) != 0)
        return -1;
    FILE* fp = fopen(path, "r");
    if (!fp)
        return -1;
    while (fgets(line, sizeof line, fp)) {
        if (strncmp(line, prefix, n) == 0) {
            *out = atoi(line + n);
            fclose(fp);
            return 0;
        }
    }
    fclose(fp);
    return -1;
}

int steps_vehicle_field(const StepWorld *world, const int id, const char *field, char *out,
                        const size_t outlen)
{
    char path[600];
    char line[512];
    char prefix[32];
    char *fields[9];
    int n = 0;
    char *save = NULL;

    if (!field || !out || outlen == 0)
        return -1;
    snprintf(prefix, sizeof prefix, "%d|", id);
    if (steps_data_path(world, "stock.dat", path, sizeof path) != 0)
        return -1;
    FILE* fp = fopen(path, "r");
    if (!fp)
        return -1;
    while (fgets(line, sizeof line, fp)) {
        if (strncmp(line, prefix, strlen(prefix)) != 0)
            continue;
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = 0;
        char* p = strtok_r(line, "|", &save);
        while (p && n < 9) {
            fields[n++] = p;
            p = strtok_r(NULL, "|", &save);
        }
        fclose(fp);
        if (n < 9)
            return -1;
        if (strcmp(field, "id") == 0)
            snprintf(out, outlen, "%s", fields[0]);
        else if (strcmp(field, "plate") == 0)
            snprintf(out, outlen, "%s", fields[1]);
        else if (strcmp(field, "price") == 0)
            snprintf(out, outlen, "%s", fields[7]);
        else if (strcmp(field, "status") == 0)
            snprintf(out, outlen, "%s", fields[8]);
        else
            return -1;
        return 0;
    }
    fclose(fp);
    return -1;
}

int steps_customer_field(const StepWorld *world, const int id, const char *field, char *out,
                         const size_t outlen)
{
    char path[600];
    char line[512];
    char prefix[32];
    char *fields[5];
    int n = 0;
    char *save = NULL;

    if (!field || !out || outlen == 0)
        return -1;
    snprintf(prefix, sizeof prefix, "%d|", id);
    if (steps_data_path(world, "customers.dat", path, sizeof path) != 0)
        return -1;
    FILE* fp = fopen(path, "r");
    if (!fp)
        return -1;
    while (fgets(line, sizeof line, fp)) {
        if (strncmp(line, prefix, strlen(prefix)) != 0)
            continue;
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r'))
            line[--len] = 0;
        char* p = strtok_r(line, "|", &save);
        while (p && n < 5) {
            fields[n++] = p;
            p = strtok_r(NULL, "|", &save);
        }
        fclose(fp);
        if (n < 5)
            return -1;
        if (strcmp(field, "id") == 0)
            snprintf(out, outlen, "%s", fields[0]);
        else if (strcmp(field, "name") == 0)
            snprintf(out, outlen, "%s", fields[1]);
        else if (strcmp(field, "type") == 0)
            snprintf(out, outlen, "%s", fields[4]);
        else
            return -1;
        return 0;
    }
    fclose(fp);
    return -1;
}

int steps_send_pause(const StepWorld *world)
{
    return console_sendline(world->console, "");
}

int steps_wait_main_menu_ready(const StepWorld *world)
{
    if (console_expect(world->console, "Option:", 5000) != 0)
        return -1;
    if (!console_saw(world->console, "Main Menu"))
        return -1;
    if (!console_saw(world->console, "1. Stock"))
        return -1;
    g_fx.in_stock_menu = 0;
    g_fx.in_cust_menu = 0;
    return 0;
}

int steps_dismiss_to_main(const StepWorld *world)
{
    if (steps_send_pause(world) != 0)
        return -1;
    if (console_sendline(world->console, "0") != 0)
        return -1;
    return steps_wait_main_menu_ready(world);
}

int steps_enter_stock_menu(const StepWorld *world)
{
    if (g_fx.in_stock_menu)
        return 0;
    if (g_fx.in_cust_menu) {
        if (console_sendline(world->console, "0") != 0)
            return -1;
        if (steps_wait_main_menu_ready(world) != 0)
            return -1;
    }
    if (console_sendline(world->console, "1") != 0)
        return -1;
    if (console_expect(world->console, "Stock", 3000) != 0)
        return -1;
    g_fx.in_stock_menu = 1;
    return 0;
}

int steps_enter_customers_menu(const StepWorld *world)
{
    if (g_fx.in_cust_menu)
        return 0;
    if (g_fx.in_stock_menu) {
        if (console_sendline(world->console, "0") != 0)
            return -1;
        if (steps_wait_main_menu_ready(world) != 0)
            return -1;
    }
    if (console_sendline(world->console, "2") != 0)
        return -1;
    if (console_expect(world->console, "Customers", 3000) != 0)
        return -1;
    g_fx.in_cust_menu = 1;
    return 0;
}

static int write_full_customers(const char *workdir, const int next_cust)
{
    char path[600];

    snprintf(path, sizeof path, "%s/data/customers.dat", workdir);
    FILE* fp = fopen(path, "w");
    if (!fp)
        return -1;
    for (int i = 0; i < 500; i++) {
        fprintf(fp, "%d|Full Cust %03d|0700000%03d|full%03d@example.com|R\n",
                1000 + i, i, i % 1000, i);
    }
    fclose(fp);
    (void)next_cust;
    return 0;
}

int steps_flush_seed(const StepWorld *world)
{
    char path[600];
    FILE *fp;
    TerminalFixture *fx = &g_fx;
    int next_veh;
    int next_cust;
    double price;

    if (fx->seed_ready)
        return 0;

    if (fx->empty_data_dir) {
        if (steps_ensure_empty_workdir(world) != 0)
            return -1;
        fx->seed_ready = 1;
        return 0;
    }

    if (steps_ensure_workdir(world) != 0)
        return -1;

    const char* root = steps_data_root(world);

    if (fx->empty_stock || fx->empty_customers || fx->full_customers ||
        fx->has_custom_plate || fx->has_custom_veh || fx->has_custom_cust) {
        if (steps_write_empty_tables(world) != 0)
            return -1;

        if (fx->full_customers) {
            if (write_full_customers(root,
                                     fx->has_next_cust ? fx->next_cust : 2999) != 0)
                return -1;
        } else if (fx->has_custom_cust || fx->has_second_cust) {
            snprintf(path, sizeof path, "%s/data/customers.dat", root);
            fp = fopen(path, "w");
            if (!fp)
                return -1;
            if (fx->has_custom_cust) {
                fprintf(fp, "%d|%s|07700 900001|seed@example.com|%c\n",
                        fx->custom_cust_id > 0 ? fx->custom_cust_id : 2101,
                        fx->custom_cust_name[0] ? fx->custom_cust_name
                                               : "Sarah Whitfield",
                        fx->custom_cust_type ? fx->custom_cust_type : 'R');
            }
            if (fx->has_second_cust) {
                fprintf(fp, "%d|%s|07700 900002|tom@example.com|R\n",
                        (fx->custom_cust_id > 0 ? fx->custom_cust_id : 2101) + 1,
                        fx->second_cust_name);
            }
            fclose(fp);
        }

        if (fx->has_custom_plate || fx->has_custom_veh || fx->has_veh_stock ||
            fx->has_veh_reserved || fx->has_veh_sold) {
            snprintf(path, sizeof path, "%s/data/stock.dat", root);
            fp = fopen(path, "w");
            if (!fp)
                return -1;
            if (fx->has_custom_plate || fx->has_custom_veh) {
                price = fx->has_list_price         ? fx->list_price
                        : fx->custom_veh_price > 0 ? fx->custom_veh_price
                                                   : 10995.00;
                fprintf(fp, "%d|%s|Ford|Focus|2020|1000|5000.00|%.2f|%d\n",
                        fx->custom_veh_id > 0 ? fx->custom_veh_id : 1101,
                        fx->custom_plate[0] ? fx->custom_plate : fx->plate_stock,
                        price, fx->custom_veh_status);
            } else {
                price = fx->has_list_price ? fx->list_price : 10000.00;
                if (fx->has_veh_stock)
                    fprintf(fp, "1101|%s|Ford|Focus|2020|1000|5000.00|%.2f|0\n",
                            fx->plate_stock, price);
                if (fx->has_veh_reserved)
                    fprintf(fp, "1102|%s|Ford|Fiesta|2020|1000|5000.00|10000.00|1\n",
                            fx->plate_reserved);
                if (fx->has_veh_sold)
                    fprintf(fp, "1103|%s|Ford|Ka|2020|1000|5000.00|10000.00|2\n",
                            fx->plate_sold);
            }
            fclose(fp);
        }

        next_veh = fx->has_next_veh ? fx->next_veh : 1104;
        next_cust = fx->has_next_cust ? fx->next_cust : 2101;
        if (fx->full_customers)
            next_cust = fx->has_next_cust ? fx->next_cust : 2999;
        if (steps_write_ids(world, next_veh, next_cust, fx->next_inv) != 0)
            return -1;
        fx->seed_ready = 1;
        return 0;
    }

    /* Stock list fixture: three vehicles with known plates. */
    if (fx->has_veh_stock || fx->has_veh_reserved || fx->has_veh_sold) {
        price = fx->has_list_price ? fx->list_price : 10000.00;
        snprintf(path, sizeof path, "%s/data/stock.dat", root);
        fp = fopen(path, "w");
        if (!fp)
            return -1;
        if (fx->has_veh_stock)
            fprintf(fp, "1101|%s|Ford|Focus|2020|1000|5000.00|%.2f|0\n",
                    fx->plate_stock, price);
        if (fx->has_veh_reserved)
            fprintf(fp, "1102|%s|Ford|Fiesta|2020|1000|5000.00|10000.00|1\n",
                    fx->plate_reserved);
        if (fx->has_veh_sold)
            fprintf(fp, "1103|%s|Ford|Ka|2020|1000|5000.00|10000.00|2\n",
                    fx->plate_sold);
        fclose(fp);

        snprintf(path, sizeof path, "%s/data/customers.dat", root);
        fp = fopen(path, "w");
        if (!fp)
            return -1;
        fprintf(fp, "2101|%s|07700 900001|seed@example.com|R\n", fx->name_retail);
        fclose(fp);

        snprintf(path, sizeof path, "%s/data/invoices.dat", root);
        if (write_empty_file(path) != 0)
            return -1;
        fx->seed_ready = 1;
        return steps_write_ids(world, 1104, 2102, fx->next_inv);
    }

    /* Customer list fixture: retail + trade only. */
    if (fx->has_retail_cust && fx->has_trade_cust && !fx->has_list_price &&
        !fx->has_mark_cust && fx->veh_id == 1101 && !fx->has_cust_type) {
        snprintf(path, sizeof path, "%s/data/customers.dat", root);
        fp = fopen(path, "w");
        if (!fp)
            return -1;
        fprintf(fp, "2101|%s|07700 900001|retail@example.com|R\n", fx->name_retail);
        fprintf(fp, "2102|%s|01234 567890|trade@example.com|T\n", fx->name_trade);
        fclose(fp);

        snprintf(path, sizeof path, "%s/data/stock.dat", root);
        if (write_empty_file(path) != 0)
            return -1;

        snprintf(path, sizeof path, "%s/data/invoices.dat", root);
        if (write_empty_file(path) != 0)
            return -1;
        fx->seed_ready = 1;
        return steps_write_ids(world, 1101, 2103, fx->next_inv);
    }

    /* Mark-as-fleet fixture: named retail customer. */
    if (fx->has_mark_cust) {
        snprintf(path, sizeof path, "%s/data/customers.dat", root);
        fp = fopen(path, "w");
        if (!fp)
            return -1;
        fprintf(fp, "%d|%s|01632 960702|fleet@greenwaytaxis.example|R\n",
                fx->mark_cust_id, fx->name_mark);
        fclose(fp);

        snprintf(path, sizeof path, "%s/data/stock.dat", root);
        if (write_empty_file(path) != 0)
            return -1;
        snprintf(path, sizeof path, "%s/data/invoices.dat", root);
        if (write_empty_file(path) != 0)
            return -1;
        fx->seed_ready = 1;
        return steps_write_ids(world, 1101, fx->mark_cust_id + 1, fx->next_inv);
    }

    /* Quote / sell fixture: one vehicle + one customer. */
    {
        const char *cname = fx->name_retail;
        char ctype = 'R';

        price = fx->has_list_price ? fx->list_price : 9295.00;

        if (fx->has_cust_type) {
            ctype = fx->cust_type;
            if (ctype == 'T')
                cname = fx->name_trade;
            else if (ctype == 'F')
                cname = fx->name_fleet;
        }

        snprintf(path, sizeof path, "%s/data/customers.dat", root);
        fp = fopen(path, "w");
        if (!fp)
            return -1;
        fprintf(fp, "%d|%s|07700 900001|seed@example.com|%c\n", fx->cust_id, cname,
                ctype);
        fclose(fp);

        snprintf(path, sizeof path, "%s/data/stock.dat", root);
        fp = fopen(path, "w");
        if (!fp)
            return -1;
        fprintf(fp, "%d|AB12 CDE|Test|Seed|2020|1000|8000.00|%.2f|%d\n", fx->veh_id,
                price, fx->custom_veh_status);
        fclose(fp);

        snprintf(path, sizeof path, "%s/data/invoices.dat", root);
        if (write_empty_file(path) != 0)
            return -1;

        next_veh = fx->next_veh > 0 ? fx->next_veh : fx->veh_id + 1;
        next_cust = fx->next_cust > 0 ? fx->next_cust : fx->cust_id + 1;
        fx->seed_ready = 1;
        return steps_write_ids(world, next_veh, next_cust, fx->next_inv);
    }
}

int steps_ensure_app(const StepWorld *world)
{
    char binary[1024];
    char workdir[512];

    if (!world || !world->console)
        return -1;
    Console* c = world->console;
    if (g_fx.app_started)
        return 0;
    if (!c->binary[0]) {
        if (console_resolve_binary(c->binary, sizeof c->binary) != 0)
            return -1;
    }
    if (!c->workdir[0]) {
        if (g_fx.empty_data_dir) {
            if (steps_ensure_empty_workdir(world) != 0)
                return -1;
        } else if (steps_ensure_workdir(world) != 0) {
            return -1;
        }
    }
    /* console_start clears Console; keep path copies on the stack. */
    snprintf(binary, sizeof binary, "%s", c->binary);
    snprintf(workdir, sizeof workdir, "%s", c->workdir);
    if (console_start(c, binary, workdir) != 0)
        return -1;
    if (steps_wait_main_menu_ready(world) != 0)
        return -1;
    g_fx.app_started = 1;
    return 0;
}

int steps_stop_app_keep_workdir(const StepWorld *world)
{
    char saved_workdir[512];
    char saved_binary[1024];

    if (!g_fx.app_started)
        return 0;
    if (world && world->console) {
        snprintf(saved_workdir, sizeof saved_workdir, "%s", world->console->workdir);
        snprintf(saved_binary, sizeof saved_binary, "%s", world->console->binary);
        world->console->owns_workdir = 0;
        console_stop(world->console);
        memset(world->console, 0, sizeof *world->console);
        world->console->master_fd = -1;
        snprintf(world->console->workdir, sizeof world->console->workdir, "%s",
                 saved_workdir);
        snprintf(world->console->binary, sizeof world->console->binary, "%s",
                 saved_binary);
    }
    g_fx.app_started = 0;
    g_fx.in_stock_menu = 0;
    g_fx.in_cust_menu = 0;
    return 0;
}

int steps_restart_app(const StepWorld *world)
{
    if (steps_stop_app_keep_workdir(world) != 0)
        return -1;
    return steps_ensure_app(world);
}

int steps_quote_or_sell_inputs(const StepWorld *world, const char *menu,
                               const char *veh_id, const char *cust_id,
                               const char *disc, const char *tradein)
{
    if (console_sendline(world->console, menu) != 0)
        return -1;
    if (console_expect(world->console, "Vehicle ID", 3000) != 0)
        return -1;
    if (console_sendline(world->console, veh_id) != 0)
        return -1;
    if (console_expect(world->console, "Customer ID", 3000) != 0)
        return -1;
    if (console_sendline(world->console, cust_id) != 0)
        return -1;
    if (console_expect(world->console, "Discount", 3000) != 0)
        return -1;
    if (console_sendline(world->console, disc) != 0)
        return -1;
    if (console_expect(world->console, "Trade-in", 3000) != 0)
        return -1;
    if (console_sendline(world->console, tradein) != 0)
        return -1;
    return 0;
}

int steps_open_quote_from_fixture(const StepWorld *world)
{
    char veh[32];
    char cust[32];
    char disc[32];
    char tradein[32];
    TerminalFixture *fx = &g_fx;

    snprintf(veh, sizeof veh, "%d", fx->veh_id);
    snprintf(cust, sizeof cust, "%d", fx->cust_id);
    snprintf(disc, sizeof disc, "%.0f", fx->has_salesman ? fx->salesman_pct : 0.0);
    snprintf(tradein, sizeof tradein, "%.2f", fx->has_tradein ? fx->tradein : 0.0);

    if (steps_quote_or_sell_inputs(world, "3", veh, cust, disc, tradein) != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;
    fx->quote_ready = 1;
    return 0;
}

int steps_screen_has_money(const StepWorld *world, const char *amount)
{
    if (!amount || !amount[0])
        return -1;
    if (console_saw(world->console, amount))
        return 0;
    fprintf(stderr, "amount not on screen: %s\n", amount);
    return -1;
}

int steps_screen_line_absent(const StepWorld *world, const char *needle)
{
    if (!needle || !needle[0])
        return -1;
    if (console_saw(world->console, needle)) {
        fprintf(stderr, "expected absent on screen: %s\n", needle);
        return -1;
    }
    return 0;
}

int steps_assert_discount_amount(const StepWorld *world, const char *label,
                                 const char *amount)
{
    if (!amount || !amount[0])
        return -1;
    /* Zero automatic discounts are omitted from the quote printout. */
    if (strcmp(amount, "0.00") == 0 || strcmp(amount, "0") == 0)
        return steps_screen_line_absent(world, label);
    if (!console_saw(world->console, amount)) {
        fprintf(stderr, "%s amount missing: %s\n", label, amount);
        return -1;
    }
    return 0;
}

int steps_invoices_file_contains(const StepWorld *world, const char *needle)
{
    char path[600];
    char line[512];
    int found = 0;

    snprintf(path, sizeof path, "%s/data/invoices.dat", steps_data_root(world));
    FILE* fp = fopen(path, "r");
    if (!fp)
        return -1;
    while (fgets(line, sizeof line, fp)) {
        if (strstr(line, needle)) {
            found = 1;
            break;
        }
    }
    fclose(fp);
    return found ? 0 : -1;
}

int steps_count_invoice_rows(const StepWorld *world)
{
    return steps_count_data_rows(world, "invoices.dat");
}

int steps_stock_status_for_id(const StepWorld *world, const int id, const int want_status)
{
    char path[600];
    char line[512];
    char prefix[32];

    snprintf(prefix, sizeof prefix, "%d|", id);
    snprintf(path, sizeof path, "%s/data/stock.dat", steps_data_root(world));
    FILE* fp = fopen(path, "r");
    if (!fp)
        return -1;
    while (fgets(line, sizeof line, fp)) {
        if (strncmp(line, prefix, strlen(prefix)) != 0)
            continue;
        char* p = strrchr(line, '|');
        if (!p) {
            fclose(fp);
            return -1;
        }
        int st = atoi(p + 1);
        fclose(fp);
        if (st != want_status) {
            fprintf(stderr, "vehicle %d status %d want %d: %s", id, st, want_status,
                    line);
            return -1;
        }
        return 0;
    }
    fclose(fp);
    return -1;
}

int steps_next_invoice_is(const StepWorld *world, const int want)
{
    char path[600];
    char line[128];
    int got = -1;

    snprintf(path, sizeof path, "%s/data/next_ids.dat", steps_data_root(world));
    FILE* fp = fopen(path, "r");
    if (!fp)
        return -1;
    while (fgets(line, sizeof line, fp)) {
        if (strncmp(line, "invoice=", 8) == 0) {
            got = atoi(line + 8);
            break;
        }
    }
    fclose(fp);
    if (got != want) {
        fprintf(stderr, "next invoice %d want %d\n", got, want);
        return -1;
    }
    return 0;
}

int steps_customer_type_in_file(const StepWorld *world, const int id, const char want_type)
{
    char path[600];
    char line[256];
    char prefix[32];

    snprintf(prefix, sizeof prefix, "%d|", id);
    snprintf(path, sizeof path, "%s/data/customers.dat", steps_data_root(world));
    FILE* fp = fopen(path, "r");
    if (!fp)
        return -1;
    while (fgets(line, sizeof line, fp)) {
        if (strncmp(line, prefix, strlen(prefix)) != 0)
            continue;
        size_t n = strlen(line);
        while (n > 0 && (line[n - 1] == '\n' || line[n - 1] == '\r'))
            line[--n] = 0;
        char* p = strrchr(line, '|');
        fclose(fp);
        if (!p || p[1] != want_type) {
            fprintf(stderr, "customer %d type want %c: %s\n", id, want_type, line);
            return -1;
        }
        return 0;
    }
    fclose(fp);
    return -1;
}

void common_steps_register(void)
{
    /* Shared helpers only; Gherkin phrases live in per-feature step files. */
}
