/*
 * customer.h - customer mutation rules (between UI and persistence)
 */
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "globals.h"

/*
 * Validate name, append customer, bump next_cust_id.
 * trade_yes non-zero -> type 'T', else 'R'.
 * Does not call save_*. Returns 0 on success, -1 on error.
 */
int customer_add(const char *name, const char *phone, const char *email,
                 int trade_yes);

/* Case-sensitive substring match (strstr). Empty needle matches. */
int customer_name_matches(const CUSTOMER *c, const char *needle);

/* Count customers whose name contains needle (case-sensitive). */
int customer_count_name_matches(const char *needle);

/* Same filter as list_customers: every stored customer is visible. */
int customer_visible_in_list(const CUSTOMER *c);

#endif
