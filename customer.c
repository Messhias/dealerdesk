/*
 * customer.c - customer mutation rules
 *
 * Extracted from main.c customer path (Extract Class / Move Method) so
 * CTest can cover add/find/list without stdin.
 * Does not call save_* or read the terminal.
 */
#include <stdio.h>
#include <string.h>

#include "customer.h"

int customer_add(const char *name, const char *phone, const char *email,
                 const int trade_yes)
{
    CUSTOMER c;

    if (!name || name[0] == 0)
        return -1;
    if (ncust >= MAX_CUST)
        return -1;

    memset(&c, 0, sizeof c);
    snprintf(c.name, sizeof c.name, "%s", name);
    if (phone)
        snprintf(c.phone, sizeof c.phone, "%s", phone);
    if (email)
        snprintf(c.email, sizeof c.email, "%s", email);
    c.type = trade_yes ? 'T' : 'R';
    c.id = next_cust_id++;
    cust[ncust++] = c;
    return 0;
}

int customer_name_matches(const CUSTOMER *c, const char *needle)
{
    if (!c)
        return 0;
    if (!needle)
        return 0;
    return strstr(c->name, needle) != NULL;
}

int customer_count_name_matches(const char *needle)
{
    int n = 0;
    int i;

    if (!needle)
        return 0;
    for (i = 0; i < ncust; i++) {
        if (customer_name_matches(&cust[i], needle))
            n++;
    }
    return n;
}

int customer_visible_in_list(const CUSTOMER *c)
{
    if (!c)
        return 0;
    return 1;
}
