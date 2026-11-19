/*
 * test_common.h - shared CTest harness helpers (asserts, tables, temp data/)
 */
#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#define MONEY_EPS 0.001

extern int g_failures;

void test_reset_failures(void);
int test_failed(void);

void assert_int(const char *label, int got, int want);
void assert_money(const char *label, double got, double want);
void assert_true(const char *label, int cond);
void assert_false(const char *label, int cond);
void assert_char(const char *label, char got, char want);
void assert_str(const char *label, const char *got, const char *want);

void reset_tables(void);
int enter_temp_data_tree(const char *prefix);
void leave_temp_data_tree(void);

#endif
