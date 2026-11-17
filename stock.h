/*
 * stock.h - stock mutation rules (between UI and persistence)
 */
#ifndef STOCK_H
#define STOCK_H

#include "globals.h"

/*
 * Validate plate and list price, append vehicle, bump next_veh_id.
 * Does not call save_*. Returns 0 on success, -1 on error.
 */
int stock_add(const char *plate, const char *make, const char *model,
              int year, long mileage, double cost, double price);

/* Exact plate match (case-sensitive). Returns first index or -1. */
int stock_find_first_by_plate(const char *plate);

/* Update list price when new_price > 0. Returns 1 if changed, 0 otherwise. */
int stock_set_price(VEHICLE *v, double new_price);

/*
 * Toggle ST_STOCK <-> ST_RESERVED.
 * Returns 0 on success, -1 if null or sold.
 */
int stock_toggle_reserve(VEHICLE *v);

/* Same filter as list_stock: hide sold when show_sold is 0. */
int stock_visible_in_list(const VEHICLE *v, int show_sold);

#endif
