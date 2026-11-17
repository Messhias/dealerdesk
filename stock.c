/*
 * stock.c - stock mutation rules
 *
 * Extracted from main.c stock path (Extract Class / Move Method) so
 * CTest can cover add/find/price/reserve/list without stdin.
 * Does not call save_* or read the terminal.
 */
#include <stdio.h>
#include <string.h>

#include "stock.h"

int stock_add(const char *plate, const char *make, const char *model,
              const int year, const long mileage, const double cost, const double price)
{
    VEHICLE v;

    if (!plate || plate[0] == 0 || price <= 0.0)
        return -1;
    if (nveh >= MAX_VEH)
        return -1;

    memset(&v, 0, sizeof v);
    snprintf(v.plate, sizeof v.plate, "%s", plate);
    if (make)
        snprintf(v.make, sizeof v.make, "%s", make);
    if (model)
        snprintf(v.model, sizeof v.model, "%s", model);
    v.year = year;
    v.mileage = mileage;
    v.cost = cost;
    v.price = price;
    v.id = next_veh_id++;
    v.status = ST_STOCK;
    veh[nveh++] = v;
    return 0;
}

int stock_find_first_by_plate(const char *plate)
{
    if (!plate)
        return -1;
    for (int i = 0; i < nveh; i++) {
        if (strcmp(veh[i].plate, plate) == 0)
            return i;
    }
    
    return -1;
}

int stock_set_price(VEHICLE *v, const double new_price)
{
    if (!v || new_price <= 0.0)
        return 0;
    
    v->price = new_price;
    
    return 1;
}

int stock_toggle_reserve(VEHICLE *v)
{
    if (!v)
        return -1;
    if (v->status == ST_SOLD)
        return -1;
    v->status = (v->status == ST_STOCK) ? ST_RESERVED : ST_STOCK;
    return 0;
}

int stock_visible_in_list(const VEHICLE *v, const int show_sold)
{
    if (!v)
        return 0;
    if (!show_sold && v->status == ST_SOLD)
        return 0;
    return 1;
}
