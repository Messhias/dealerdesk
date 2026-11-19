/*
 * sale.c - quote eligibility and confirmed-sale packing
 *
 * Extracted from main.c sell path so CTest can cover sold-blocked,
 * disc/fees packing, and confirm/cancel without stdin.
 */
#include <stdio.h>
#include <string.h>

#include "sale.h"

int vehicle_can_quote(const VEHICLE *v)
{
    if (!v)
        return 0;
    
    return v->status != ST_SOLD;
}

void invoice_from_quote(INVOICE *i, const QUOTE *q, const int inv_no,
                        const char *date, const int veh_id, const int cust_id)
{
    memset(i, 0, sizeof *i);
    
    i->no = inv_no;
    
    if (date)
        snprintf(i->date, sizeof i->date, "%s", date);
    
    i->veh_id = veh_id;
    i->cust_id = cust_id;
    i->list = q->list;
    i->disc = q->trade_disc + q->fleet_disc + q->disc;
    i->net = q->net;
    i->vat = q->vat;
    i->fees = q->delivery + q->reg_fee;
    i->tradein = q->tradein;
    i->total = q->total;
}

int complete_sale(VEHICLE *v, const CUSTOMER *c, const QUOTE *q,
                  const char *date)
{
    if (!v || !c || !q || !date)
        return -1;
    
    if (!vehicle_can_quote(v))
        return -1;
    
    if (ninv >= MAX_INV)
        return -1;

    INVOICE* i = &inv[ninv];
    
    invoice_from_quote(i, q, next_inv_no, date, v->id, c->id);
    
    next_inv_no++;
    ninv++;
    
    v->status = ST_SOLD;
    
    return 0;
}
