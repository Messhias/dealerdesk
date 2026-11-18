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

static int customer_type_ok(const char type)
{
    return type == 'R' || type == 'T' || type == 'F';
}

int customer_add(const char *name, const char *phone, const char *email,
                 const char type)
{
    CUSTOMER c;

    if (!name || name[0] == 0)
        return -1;
    if (!customer_type_ok(type))
        return -1;
    if (ncust >= MAX_CUST)
        return -1;

    memset(&c, 0, sizeof c);
    snprintf(c.name, sizeof c.name, "%s", name);
    if (phone)
        snprintf(c.phone, sizeof c.phone, "%s", phone);
    if (email)
        snprintf(c.email, sizeof c.email, "%s", email);
    c.type = type;
    c.id = next_cust_id++;
    cust[ncust++] = c;
    return 0;
}

int customer_set_type(const int id, const char type)
{
    if (!customer_type_ok(type))
        return -1;
    
    for (int i = 0; i < ncust; i++) {
        if (cust[i].id == id) {
            cust[i].type = type;
            return 0;
        }
    }
    
    return -1;
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

    if (!needle)
        return 0;
    
    for (int i = 0; i < ncust; i++) {
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
