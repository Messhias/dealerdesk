/*
 * steps.h - step definition registry for the BDD runner
 */
#ifndef STEPS_H
#define STEPS_H

#include "../runner/console.h"

typedef struct {
    Console *console;
    char repo_root[1024];
    char binary[1024];
    char workdir[512];
    char screenshot_name[256];
    int failed;
} StepWorld;

typedef int (*StepFn)(StepWorld *world, const char *arg);

void steps_register_all(void);
int steps_dispatch(StepWorld *world, const char *keyword, const char *text);
void steps_register(const char *pattern, StepFn fn);

#endif
