/*
 * sale.h - quote eligibility and invoice packing for sell
 */
#ifndef SALE_H
#define SALE_H

#include "calc.h"
#include "globals.h"

/* Same rule as build_quote: sold vehicles cannot be quoted or sold. */
int vehicle_can_quote(const VEHICLE *v);

/* Pack quote fields into an invoice record (combined disc and fees). */
void invoice_from_quote(INVOICE *i, const QUOTE *q, int inv_no,
                        const char *date, int veh_id, int cust_id);

/*
 * Append a packed invoice, bump next_inv_no, mark vehicle sold.
 * Does not call save_*. Returns 0 on success, -1 on error.
 */
int complete_sale(VEHICLE *v, const CUSTOMER *c, const QUOTE *q,
                  const char *date);

#endif
