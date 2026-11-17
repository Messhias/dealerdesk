/*
 * test_common.c - shared CTest harness (Duplicate Code extract)
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "test_common.h"

int g_failures;
static char g_home_cwd[512];
static char g_temp_root[256];

void test_reset_failures(void)
{
    g_failures = 0;
}

int test_failed(void)
{
    return g_failures != 0;
}

void assert_int(const char *label, int got, int want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got %d want %d\n", label, got, want);
        g_failures++;
    }
}

void assert_money(const char *label, double got, double want)
{
    if (fabs(got - want) > MONEY_EPS) {
        fprintf(stderr, "FAIL %s: got %.4f want %.2f\n", label, got, want);
        g_failures++;
    }
}

void assert_true(const char *label, int cond)
{
    if (!cond) {
        fprintf(stderr, "FAIL %s: expected true\n", label);
        g_failures++;
    }
}

void assert_false(const char *label, int cond)
{
    if (cond) {
        fprintf(stderr, "FAIL %s: expected false\n", label);
        g_failures++;
    }
}

void assert_char(const char *label, char got, char want)
{
    if (got != want) {
        fprintf(stderr, "FAIL %s: got '%c' want '%c'\n", label, got, want);
        g_failures++;
    }
}

void assert_str(const char *label, const char *got, const char *want)
{
    if (strcmp(got ? got : "", want ? want : "") != 0) {
        fprintf(stderr, "FAIL %s: got \"%s\" want \"%s\"\n",
                label, got ? got : "", want ? want : "");
        g_failures++;
    }
}

void reset_tables(void)
{
    memset(veh, 0, sizeof veh);
    memset(cust, 0, sizeof cust);
    memset(inv, 0, sizeof inv);
    nveh = 0;
    ncust = 0;
    ninv = 0;
    next_veh_id = 1000;
    next_cust_id = 2000;
    next_inv_no = 50000;
}

int enter_temp_data_tree(const char *prefix)
{
    char tmpl[64];
    char data_dir[300];

    if (!prefix || prefix[0] == 0)
        prefix = "ddtest";

    snprintf(tmpl, sizeof tmpl, "/tmp/%sXXXXXX", prefix);

    if (!getcwd(g_home_cwd, sizeof g_home_cwd)) {
        perror("getcwd");
        return -1;
    }

    char* root = mkdtemp(tmpl);
    if (!root) {
        perror("mkdtemp");
        return -1;
    }
    
    snprintf(g_temp_root, sizeof g_temp_root, "%s", root);
    snprintf(data_dir, sizeof data_dir, "%s/data", g_temp_root);
    
    if (mkdir(data_dir, 0700) != 0) {
        perror("mkdir data");
        return -1;
    }
    
    if (chdir(g_temp_root) != 0) {
        perror("chdir temp");
        return -1;
    }
    
    return 0;
}

void leave_temp_data_tree(void)
{
    if (g_temp_root[0] == 0)
        return;

    unlink("data/stock.dat");
    unlink("data/customers.dat");
    unlink("data/invoices.dat");
    unlink("data/next_ids.dat");
    rmdir("data");

    if (g_home_cwd[0] != 0 && chdir(g_home_cwd) != 0)
        perror("chdir home");

    rmdir(g_temp_root);
    g_temp_root[0] = 0;
}
