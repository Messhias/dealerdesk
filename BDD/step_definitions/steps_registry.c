/*
 * steps_registry.c - pattern match, dispatch, and register-all entrypoint
 */
#include "steps.h"

#include <stdio.h>
#include <string.h>

#define MAX_STEPS 256

typedef struct {
    char pattern[256];
    StepFn fn;
} StepEntry;

static StepEntry g_steps[MAX_STEPS];
static int g_nsteps;

void steps_register(const char *pattern, const StepFn fn)
{
    if (g_nsteps >= MAX_STEPS || !pattern || !fn)
        return;
    snprintf(g_steps[g_nsteps].pattern, sizeof g_steps[g_nsteps].pattern, "%s", pattern);
    g_steps[g_nsteps].fn = fn;
    g_nsteps++;
}

static int match_pattern(const char *pattern, const char *text, char *arg, const size_t arglen)
{
    const char *ph = strstr(pattern, "{string}");
    if (!ph) {
        if (arg)
            arg[0] = 0;
        return strcmp(pattern, text) == 0;
    }
    {
        const size_t prefix = (size_t)(ph - pattern);
        const char *suffix = ph + strlen("{string}");
        const size_t suffix_len = strlen(suffix);
        const size_t text_len = strlen(text);
        if (strncmp(pattern, text, prefix) != 0)
            return 0;
        if (text_len < prefix + suffix_len)
            return 0;
        if (strcmp(text + text_len - suffix_len, suffix) != 0)
            return 0;
        if (arg && arglen) {
            size_t al = text_len - prefix - suffix_len;
            if (al >= arglen)
                al = arglen - 1;
            memcpy(arg, text + prefix, al);
            arg[al] = 0;
            if (al >= 2 && arg[0] == '"' && arg[al - 1] == '"') {
                memmove(arg, arg + 1, al - 2);
                arg[al - 2] = 0;
            }
        }
        return 1;
    }
}

int steps_dispatch(const StepWorld *world, const char *keyword, const char *text)
{
    char arg[256];
    
    (void)keyword;
    
    for (int i = 0; i < g_nsteps; i++) {
        if (match_pattern(g_steps[i].pattern, text, arg, sizeof arg))
            return g_steps[i].fn(world, arg);
    }
    fprintf(stderr, "undefined step: %s\n", text);
    return -1;
}

void steps_register_all(void)
{
    g_nsteps = 0;
    steps_begin_scenario();

    common_steps_register();
    stock_steps_register();
    customers_steps_register();
    pricing_steps_register();
    quote_sell_steps_register();
    fleet_steps_register();
    persistence_steps_register();
}
