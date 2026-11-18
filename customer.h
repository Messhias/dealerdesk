/*
 * customer.h - customer mutation rules (between UI and persistence)
 */
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "globals.h"

/*
 * Validate name and type (R/T/F), append customer, bump next_cust_id.
 * Does not call save_*. Returns 0 on success, -1 on error.
 */
int customer_add(const char *name, const char *phone, const char *email,
                 char type);

/*
 * Set type of an existing customer by id. type must be R, T, or F.
 * Returns 0 on success, -1 if id missing or type invalid.
 */
int customer_set_type(int id, char type);

/* Case-sensitive substring match (strstr). Empty needle matches. */
int customer_name_matches(const CUSTOMER *c, const char *needle);

/* Count customers whose name contains needle (case-sensitive). */
int customer_count_name_matches(const char *needle);

/* Same filter as list_customers: every stored customer is visible. */
int customer_visible_in_list(const CUSTOMER *c);

#endif
