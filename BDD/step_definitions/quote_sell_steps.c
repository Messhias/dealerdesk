/*
 * quote_sell_steps.c - @terminal steps for quote_sell.feature phrases
 */
#include "steps_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int step_private_temp_tree(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_ensure_workdir(world);
}

static int step_vehicle_id_priced(const StepWorld *world, const char *arg)
{
    /* Phrase: one in-stock vehicle id 1101 priced 9295.00 — matched exact below. */
    (void)world;
    (void)arg;
    steps_fixture()->veh_id = 1101;
    steps_fixture()->list_price = 9295.00;
    steps_fixture()->has_list_price = 1;
    return 0;
}

static int step_retail_customer_id(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->cust_id = 2101;
    steps_fixture()->cust_type = 'R';
    steps_fixture()->has_cust_type = 1;
    return 0;
}

static int step_next_invoice_number(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->next_inv = 50233;
    return 0;
}

static int step_sale_completed(const StepWorld *world, const char *arg)
{
    char veh[32];
    char cust[32];
    (void)arg;

    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;

    snprintf(veh, sizeof veh, "%d", steps_fixture()->veh_id);
    snprintf(cust, sizeof cust, "%d", steps_fixture()->cust_id);
    if (steps_quote_or_sell_inputs(world, "4", veh, cust, "5", "1500.00") != 0)
        return -1;
    if (console_expect(world->console, "Confirm sale", 5000) != 0)
        return -1;
    if (console_sendline(world->console, "Y") != 0)
        return -1;
    if (console_expect(world->console, "Invoice 50233 created", 5000) != 0)
        return -1;
    return 0;
}

static int step_sale_cancelled(const StepWorld *world, const char *arg)
{
    char veh[32];
    char cust[32];
    (void)arg;

    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;

    snprintf(veh, sizeof veh, "%d", steps_fixture()->veh_id);
    snprintf(cust, sizeof cust, "%d", steps_fixture()->cust_id);
    if (steps_quote_or_sell_inputs(world, "4", veh, cust, "5", "1500.00") != 0)
        return -1;
    if (console_expect(world->console, "Confirm sale", 5000) != 0)
        return -1;
    if (console_sendline(world->console, "N") != 0)
        return -1;
    if (console_expect(world->console, "Sale cancelled", 5000) != 0)
        return -1;
    return 0;
}

static int step_noop(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    /* App already persisted; terminal proof reads files directly. */
    return 0;
}

static int step_exactly_n_invoices(const StepWorld *world, const char *arg)
{
    if (!arg || !arg[0])
        return -1;
    const int want = atoi(arg);
    const int got = steps_count_invoice_rows(world);
    if (got < 0)
        return -1;
    if (got != want) {
        fprintf(stderr, "invoice rows %d want %d\n", got, want);
        return -1;
    }
    return 0;
}

static int step_invoice_disc(const StepWorld *world, const char *arg)
{
    return steps_invoices_file_contains(world, arg);
}

static int step_invoice_fees(const StepWorld *world, const char *arg)
{
    return steps_invoices_file_contains(world, arg);
}

static int step_invoice_total(const StepWorld *world, const char *arg)
{
    return steps_invoices_file_contains(world, arg);
}

static int step_vehicle_sold(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_stock_status_for_id(world, steps_fixture()->veh_id, 2);
}

static int step_vehicle_in_stock(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_stock_status_for_id(world, steps_fixture()->veh_id, 0);
}

static int step_next_inv_50234(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_next_invoice_is(world, 50234);
}

static int step_next_inv_50233(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_next_invoice_is(world, 50233);
}

static int step_vehicle_status_sold_seed(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->has_veh_sold = 1;
    steps_fixture()->veh_id = 1103;
    steps_fixture()->custom_veh_status = 2;
    return 0;
}

static int step_vehicle_status_stock_seed(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    steps_fixture()->has_veh_stock = 1;
    steps_fixture()->veh_id = 1101;
    steps_fixture()->custom_veh_status = 0;
    return 0;
}

static int step_quote_eligibility(const StepWorld *world, const char *arg)
{
    char veh[32];
    (void)arg;

    if (steps_flush_seed(world) != 0)
        return -1;
    if (steps_ensure_app(world) != 0)
        return -1;

    snprintf(veh, sizeof veh, "%d", steps_fixture()->veh_id);
    const size_t mark = world->console->out_len;
    if (console_sendline(world->console, "3") != 0)
        return -1;
    if (console_expect(world->console, "Vehicle ID", 3000) != 0)
        return -1;
    if (console_sendline(world->console, veh) != 0)
        return -1;

    if (steps_fixture()->veh_id == 1103 || steps_fixture()->custom_veh_status == 2) {
        if (console_expect(world->console, "already sold", 5000) != 0)
            return -1;
        steps_fixture()->quote_eligible = 0;
        /*
         * pause_enter is printed in the same burst as "already sold".
         * console_expect only matches text after the call, so a second
         * expect would miss Press ENTER already in the buffer.
         */
        if (!console_saw_from(world->console, mark, "Press ENTER to continue")) {
            if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
                return -1;
        }
        return steps_send_pause(world);
    }

    if (console_expect(world->console, "Customer ID", 5000) != 0)
        return -1;
    steps_fixture()->quote_eligible = 1;
    /* Finish quote path with seed customer so screen stays usable. */
    if (console_sendline(world->console, "2101") != 0)
        return -1;
    if (console_expect(world->console, "Discount", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "0") != 0)
        return -1;
    if (console_expect(world->console, "Trade-in", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "0") != 0)
        return -1;
    if (console_expect(world->console, "Press ENTER to continue", 5000) != 0)
        return -1;
    if (!console_saw_from(world->console, mark, "TOTAL")) {
        fprintf(stderr, "expected quote TOTAL for eligible vehicle\n");
        return -1;
    }
    return steps_send_pause(world);
}

static int step_cannot_quote(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    return steps_fixture()->quote_eligible == 0 ? 0 : -1;
}

static int step_can_quote(const StepWorld *world, const char *arg)
{
    (void)world;
    (void)arg;
    return steps_fixture()->quote_eligible == 1 ? 0 : -1;
}

static int step_pack_invoice(const StepWorld *world, const char *arg)
{
    char veh[32];
    char cust[32];
    char disc[32];
    char tradein[32];
    (void)arg;

    /* Dismiss quote screen if still open, then sell+confirm to pack invoice. */
    if (steps_fixture()->quote_ready) {
        if (steps_send_pause(world) != 0)
            return -1;
        if (steps_wait_main_menu_ready(world) != 0)
            return -1;
    }

    snprintf(veh, sizeof veh, "%d", steps_fixture()->veh_id);
    snprintf(cust, sizeof cust, "%d", steps_fixture()->cust_id);
    snprintf(disc, sizeof disc, "%.0f",
             steps_fixture()->has_salesman ? steps_fixture()->salesman_pct : 0.0);
    snprintf(tradein, sizeof tradein, "%.2f",
             steps_fixture()->has_tradein ? steps_fixture()->tradein : 0.0);

    if (steps_quote_or_sell_inputs(world, "4", veh, cust, disc, tradein) != 0)
        return -1;
    if (console_expect(world->console, "Confirm sale", 5000) != 0)
        return -1;
    if (console_sendline(world->console, "Y") != 0)
        return -1;
    /* One expect: "Invoice N created" arrives with the pause prompt. */
    if (console_expect(world->console, "created", 5000) != 0)
        return -1;
    return 0;
}

static int step_packed_disc(const StepWorld *world, const char *arg)
{
    return steps_invoices_file_contains(world, arg);
}

static int step_packed_fees(const StepWorld *world, const char *arg)
{
    return steps_invoices_file_contains(world, arg);
}

static int step_load_all(const StepWorld *world, const char *arg)
{
    (void)arg;
    /* Start (or restart) so seed files are loaded into the live app. */
    if (steps_fixture()->app_started)
        return steps_restart_app(world);
    return steps_ensure_app(world);
}

static int step_clear_tables(const StepWorld *world, const char *arg)
{
    (void)arg;
    return steps_stop_app_keep_workdir(world);
}

void quote_sell_steps_register(void)
{
    steps_register("a private temp working tree with a data directory",
                   step_private_temp_tree);
    steps_register("one in-stock vehicle id 1101 priced 9295.00",
                   step_vehicle_id_priced);
    steps_register("one retail customer id 2101", step_retail_customer_id);
    steps_register("next invoice number 50233", step_next_invoice_number);
    steps_register(
        "a sale is completed for that vehicle and customer with salesman 5% and trade-in 1500.00",
        step_sale_completed);
    steps_register("the sale is cancelled without completing", step_sale_cancelled);
    steps_register("invoices stock and ids are saved", step_noop);
    steps_register("in-memory tables are cleared", step_clear_tables);
    steps_register("all data files are loaded", step_load_all);
    steps_register("exactly {string} invoice is in memory", step_exactly_n_invoices);
    steps_register("exactly {string} invoices are in memory", step_exactly_n_invoices);
    steps_register("that invoice disc is {string}", step_invoice_disc);
    steps_register("that invoice fees are {string}", step_invoice_fees);
    steps_register("that invoice total is {string}", step_invoice_total);
    steps_register("the vehicle status is sold", step_vehicle_sold);
    steps_register("the vehicle status is in stock", step_vehicle_in_stock);
    steps_register("next invoice number is 50234", step_next_inv_50234);
    steps_register("next invoice number is 50233", step_next_inv_50233);
    steps_register("a vehicle with status sold", step_vehicle_status_sold_seed);
    steps_register("a vehicle with status in stock", step_vehicle_status_stock_seed);
    steps_register("quote eligibility is checked", step_quote_eligibility);
    steps_register("the vehicle cannot be quoted", step_cannot_quote);
    steps_register("the vehicle can be quoted", step_can_quote);
    steps_register("an invoice is packed from that quote", step_pack_invoice);
    steps_register("the packed invoice disc is {string}", step_packed_disc);
    steps_register("the packed invoice fees are {string}", step_packed_fees);
}
