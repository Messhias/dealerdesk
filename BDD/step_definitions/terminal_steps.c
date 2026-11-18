/*
 * terminal_steps.c - Given/When/Then for terminal UI flows
 */
#include "steps.h"

#include "../runner/screenshot.h"

#include <stdio.h>
#include <string.h>

#define MAX_STEPS 64

typedef struct {
    char pattern[256];
    StepFn fn;
} StepEntry;

static StepEntry g_steps[MAX_STEPS];
static int g_nsteps;
static int g_workdir_ready;

void steps_register(const char *pattern, StepFn fn)
{
    if (g_nsteps >= MAX_STEPS || !pattern || !fn)
        return;
    snprintf(g_steps[g_nsteps].pattern, sizeof g_steps[g_nsteps].pattern, "%s", pattern);
    g_steps[g_nsteps].fn = fn;
    g_nsteps++;
}

static int match_pattern(const char *pattern, const char *text, char *arg, size_t arglen)
{
    const char *ph = strstr(pattern, "{string}");
    if (!ph) {
        if (arg)
            arg[0] = 0;
        return strcmp(pattern, text) == 0;
    }
    {
        size_t prefix = (size_t)(ph - pattern);
        const char *suffix = ph + strlen("{string}");
        size_t suffix_len = strlen(suffix);
        size_t text_len = strlen(text);
        if (strncmp(pattern, text, prefix) != 0)
            return 0;
        if (text_len < prefix + suffix_len)
            return 0;
        if (strcmp(text + text_len - suffix_len, suffix) != 0)
            return 0;
        if (arg && arglen) {
            size_t al = text_len - prefix - suffix_len;
            if (al >= arglen)
                al = arglen - 1;
            memcpy(arg, text + prefix, al);
            arg[al] = 0;
            if (al >= 2 && arg[0] == '"' && arg[al - 1] == '"') {
                memmove(arg, arg + 1, al - 2);
                arg[al - 2] = 0;
            }
        }
        return 1;
    }
}

int steps_dispatch(StepWorld *world, const char *keyword, const char *text)
{
    int i;
    char arg[256];
    (void)keyword;
    for (i = 0; i < g_nsteps; i++) {
        if (match_pattern(g_steps[i].pattern, text, arg, sizeof arg))
            return g_steps[i].fn(world, arg);
    }
    fprintf(stderr, "undefined step: %s\n", text);
    return -1;
}

static int ensure_workdir(StepWorld *world)
{
    if (g_workdir_ready)
        return 0;
    if (console_prepare_workdir(world->workdir, sizeof world->workdir, world->repo_root) != 0)
        return -1;
    g_workdir_ready = 1;
    return 0;
}

static int step_app_started(StepWorld *world, const char *arg)
{
    (void)arg;
    if (console_resolve_binary(world->binary, sizeof world->binary) != 0)
        return -1;
    if (ensure_workdir(world) != 0)
        return -1;
    if (console_start(world->console, world->binary, world->workdir) != 0)
        return -1;
    /* Temp tree is owned by console_stop after start. */
    g_workdir_ready = 0;
    if (console_expect(world->console, "Main Menu", 5000) != 0)
        return -1;
    return 0;
}

static int step_screen_shows(StepWorld *world, const char *arg)
{
    if (!arg || !arg[0])
        return -1;
    if (console_expect(world->console, arg, 3000) != 0)
        return -1;
    return 0;
}

static int step_mark_customer_fleet(StepWorld *world, const char *arg)
{
    (void)arg;
    if (console_sendline(world->console, "2") != 0)
        return -1;
    if (console_expect(world->console, "Customers", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "4") != 0)
        return -1;
    if (console_expect(world->console, "Customer ID", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "2008") != 0)
        return -1;
    if (console_expect(world->console, "New type", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "F") != 0)
        return -1;
    if (console_expect(world->console, "is now Fleet", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "") != 0)
        return -1;
    if (console_sendline(world->console, "0") != 0)
        return -1;
    if (console_expect(world->console, "Main Menu", 3000) != 0)
        return -1;
    return 0;
}

static int step_customer_file_is_fleet(StepWorld *world, const char *arg)
{
    char path[600];
    char line[256];
    FILE *fp;
    int found = 0;
    (void)arg;

    snprintf(path, sizeof path, "%s/data/customers.dat", world->console->workdir);
    fp = fopen(path, "r");
    if (!fp)
        return -1;
    while (fgets(line, sizeof line, fp)) {
        if (strncmp(line, "2008|", 5) == 0) {
            found = 1;
            if (!strstr(line, "|F")) {
                fclose(fp);
                fprintf(stderr, "customer 2008 not fleet: %s", line);
                return -1;
            }
            break;
        }
    }
    fclose(fp);
    return found ? 0 : -1;
}

static int step_seed_fleet_quote_data(StepWorld *world, const char *arg)
{
    char path[600];
    FILE *fp;
    (void)arg;

    if (ensure_workdir(world) != 0)
        return -1;

    snprintf(path, sizeof path, "%s/data/customers.dat", world->workdir);
    fp = fopen(path, "w");
    if (!fp)
        return -1;
    fprintf(fp, "2100|Fleet Quote Co|01632 960000|fleetquote@example.com|F\n");
    fclose(fp);

    snprintf(path, sizeof path, "%s/data/stock.dat", world->workdir);
    fp = fopen(path, "w");
    if (!fp)
        return -1;
    fprintf(fp, "1100|AA00 AAA|Test|Fleet Seed|2020|1000|8000.00|10000.00|0\n");
    fclose(fp);
    return 0;
}

static int step_open_fleet_quote(StepWorld *world, const char *arg)
{
    (void)arg;
    if (console_sendline(world->console, "3") != 0)
        return -1;
    if (console_expect(world->console, "Vehicle ID", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "1100") != 0)
        return -1;
    if (console_expect(world->console, "Customer ID", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "2100") != 0)
        return -1;
    if (console_expect(world->console, "Discount", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "0") != 0)
        return -1;
    if (console_expect(world->console, "Trade-in", 3000) != 0)
        return -1;
    if (console_sendline(world->console, "0") != 0)
        return -1;
    return 0;
}

static int step_quote_shows_fleet_discount(StepWorld *world, const char *arg)
{
    (void)arg;
    if (console_expect(world->console, "Fleet discount", 3000) != 0)
        return -1;
    if (console_expect(world->console, "400.00", 1000) != 0)
        return -1;
    if (console_expect(world->console, "11755.00", 1000) != 0)
        return -1;
    return 0;
}

static int step_save_screenshot(StepWorld *world, const char *arg)
{
    char path[512];
    if (!arg || !arg[0])
        return -1;
    snprintf(world->screenshot_name, sizeof world->screenshot_name, "%s", arg);
    if (snprintf(path, sizeof path, "%s/BDD/artifacts/%s.png",
                 world->repo_root, arg) >= (int)sizeof path) {
        fprintf(stderr, "screenshot path too long\n");
        return -1;
    }
    if (screenshot_write_png(world->console->out, path) != 0) {
        fprintf(stderr, "screenshot failed: %s\n", path);
        return -1;
    }
    printf("screenshot: %s\n", path);
    return 0;
}

void steps_register_all(void)
{
    g_nsteps = 0;
    g_workdir_ready = 0;
    steps_register("the application starts against isolated data", step_app_started);
    steps_register("the screen shows {string}", step_screen_shows);
    steps_register("staff marks customer 2008 as fleet via the menus", step_mark_customer_fleet);
    steps_register("customer 2008 is persisted as fleet", step_customer_file_is_fleet);
    steps_register("isolated data has a fleet customer and a 10000 list vehicle", step_seed_fleet_quote_data);
    steps_register("staff opens a quote for that fleet vehicle with zero discounts", step_open_fleet_quote);
    steps_register("the quote shows Fleet discount 400.00 and total 11755.00", step_quote_shows_fleet_discount);
    steps_register("a screenshot is saved as {string}", step_save_screenshot);
}
