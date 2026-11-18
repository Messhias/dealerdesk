/*
 * BDD runner - parse feature files under BDD/features and dispatch steps
 *
 * Runs scenarios tagged @terminal only (unit/seam Gherkin stays CTest).
 */
#include "console.h"
#include "../step_definitions/steps.h"

#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char g_repo_root[1024];

static void trim(char *s)
{
    char *e;
    while (*s == ' ' || *s == '\t')
        memmove(s, s + 1, strlen(s));
    e = s + strlen(s);
    while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\n' || e[-1] == '\r'))
        *--e = 0;
}

static int line_is_tag_terminal(const char *line)
{
    return strstr(line, "@terminal") != NULL;
}

static int starts_with_kw(const char *line, const char *kw, const char **rest)
{
    size_t n = strlen(kw);
    if (strncmp(line, kw, n) != 0)
        return 0;
    if (line[n] != ' ' && line[n] != '\t')
        return 0;
    *rest = line + n;
    while (**rest == ' ' || **rest == '\t')
        (*rest)++;
    return 1;
}

static int run_feature_file(const char *path)
{
    FILE *fp;
    char line[1024];
    int in_terminal_scenario = 0;
    int scenario_failed = 0;
    int ran = 0;
    int failed = 0;
    char scenario_name[256];
    Console console;
    StepWorld world;

    fp = fopen(path, "r");
    if (!fp) {
        perror(path);
        return -1;
    }

    scenario_name[0] = 0;
    memset(&console, 0, sizeof console);
    console.master_fd = -1;

    while (fgets(line, sizeof line, fp)) {
        const char *rest = NULL;
        trim(line);
        if (line[0] == 0 || line[0] == '#')
            continue;

        if (line[0] == '@') {
            in_terminal_scenario = line_is_tag_terminal(line) ? 1 : 0;
            continue;
        }

        if (strncmp(line, "Scenario:", 9) == 0) {
            if (console.master_fd >= 0 || console.pid > 0) {
                console_stop(&console);
                memset(&console, 0, sizeof console);
                console.master_fd = -1;
            }
            scenario_name[0] = 0;
            if (!in_terminal_scenario)
                continue;
            {
                const char *title = line + 9;
                size_t n = strlen(title);
                if (n >= sizeof scenario_name)
                    n = sizeof scenario_name - 1;
                memcpy(scenario_name, title, n);
                scenario_name[n] = 0;
            }
            trim(scenario_name);
            ran++;
            scenario_failed = 0;
            memset(&world, 0, sizeof world);
            world.console = &console;
            snprintf(world.repo_root, sizeof world.repo_root, "%s", g_repo_root);
            printf("Scenario: %s\n", scenario_name);
            in_terminal_scenario = 0;
            continue;
        }

        if (scenario_name[0] == 0)
            continue;
        if (scenario_failed)
            continue;

        if (starts_with_kw(line, "Given", &rest) || starts_with_kw(line, "When", &rest) ||
            starts_with_kw(line, "Then", &rest) || starts_with_kw(line, "And", &rest) ||
            starts_with_kw(line, "But", &rest)) {
            const char *kw = line;
            if (steps_dispatch(&world, kw, rest) != 0) {
                fprintf(stderr, "  FAIL: %s\n", rest);
                scenario_failed = 1;
                failed++;
                world.failed = 1;
            } else {
                printf("  OK: %s\n", rest);
            }
        }
    }

    if (console.master_fd >= 0 || console.pid > 0)
        console_stop(&console);
    fclose(fp);

    if (ran == 0)
        return 0;
    return failed ? -1 : 0;
}

static int ends_with(const char *name, const char *suf)
{
    size_t n = strlen(name);
    size_t s = strlen(suf);
    if (n < s)
        return 0;
    return strcmp(name + n - s, suf) == 0;
}

int main(int argc, char **argv)
{
    char features_dir[1100];
    DIR *dir;
    struct dirent *ent;
    int failures = 0;
    int files = 0;

    (void)argc;
    (void)argv;

    if (!getcwd(g_repo_root, sizeof g_repo_root)) {
        perror("getcwd");
        return 2;
    }

    steps_register_all();
    snprintf(features_dir, sizeof features_dir, "%s/BDD/features", g_repo_root);

    dir = opendir(features_dir);
    if (!dir) {
        perror(features_dir);
        return 2;
    }

    while ((ent = readdir(dir)) != NULL) {
        char path[1200];
        if (!ends_with(ent->d_name, ".feature"))
            continue;
        snprintf(path, sizeof path, "%s/%s", features_dir, ent->d_name);
        files++;
        if (run_feature_file(path) != 0)
            failures++;
    }
    closedir(dir);

    if (files == 0) {
        fprintf(stderr, "no feature files in %s\n", features_dir);
        return 2;
    }

    if (failures) {
        fprintf(stderr, "BDD terminal suite: %d feature file(s) failed\n", failures);
        return 1;
    }
    printf("BDD terminal suite: OK\n");
    return 0;
}
