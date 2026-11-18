/*
 * steps.h - step definition registry for the BDD runner
 */
#ifndef STEPS_H
#define STEPS_H

#include "../runner/console.h"

typedef struct {
    Console *console;
    char repo_root[1024];
    char feature_stem[128];
    char scenario_slug[128];
    int step_index;
    int failed;
} StepWorld;

typedef int (*StepFn)(const StepWorld *world, const char *arg);

void steps_register_all(void);
void steps_begin_scenario(void);
int steps_dispatch(const StepWorld *world, const char *keyword, const char *text);
void steps_register(const char *pattern, StepFn fn);

void common_steps_register(void);
void stock_steps_register(void);
void customers_steps_register(void);
void pricing_steps_register(void);
void quote_sell_steps_register(void);
void fleet_steps_register(void);
void persistence_steps_register(void);

#endif
