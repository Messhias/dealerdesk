/*
 * BDD runner - parse feature files under BDD/features and dispatch steps
 *
 * Runs scenarios tagged @terminal only (unit/seam Gherkin stays CTest).
 * After each successful step, writes a PNG under
 * BDD/artifacts/<feature_stem>/<scenario>/<NN>_<step>.png
 */
#include "console.h"
#include "screenshot.h"
#include "../step_definitions/steps.h"

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static char g_repo_root[1024];

static void trim(char *s)
{
    while (*s == ' ' || *s == '\t')
        memmove(s, s + 1, strlen(s));
    
    char* e = s + strlen(s);
    
    while (e > s && (e[-1] == ' ' || e[-1] == '\t' || e[-1] == '\n' || e[-1] == '\r'))
        *--e = 0;
}

static void slugify(const char *in, char *out, const size_t outlen)
{
    size_t i = 0;
    
    int prev_us = 1;

    if (!out || outlen == 0)
        return;
    
    out[0] = 0;
    
    if (!in)
        return;

    for (; *in && i + 1 < outlen; in++) {
        const unsigned char c = (unsigned char)*in;
        if (isalnum(c)) {
            out[i++] = (char)tolower(c);
            prev_us = 0;
        } else if (!prev_us) {
            out[i++] = '_';
            prev_us = 1;
        }
    }
    
    while (i > 0 && out[i - 1] == '_')
        i--;
    
    out[i] = 0;
    
    if (i == 0)
        snprintf(out, outlen, "step");
}

static int mkdir_p(const char *path)
{
    /* Must hold worst-case artifact dir (repo_root + feature + scenario). */
    char tmp[2048];

    if (!path || !path[0])
        return -1;

    if (strlen(path) >= sizeof tmp)
        return -1;

    snprintf(tmp, sizeof tmp, "%s", path);

    const size_t len = strlen(tmp);
    
    if (len == 0)
        return -1;
    
    if (tmp[len - 1] == '/')
        tmp[len - 1] = 0;

    for (size_t i = 1; tmp[i]; i++) {
        if (tmp[i] != '/')
            continue;
        
        tmp[i] = 0;
        
        if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
            return -1;
        
        tmp[i] = '/';
    }
    
    if (mkdir(tmp, 0755) != 0 && errno != EEXIST)
        return -1;
    
    return 0;
}

static unsigned long g_last_screen_hash;

static int save_step_screenshot(StepWorld *world, const char *step_text)
{
    /* repo_root[1024] + feature_stem[128] + scenario_slug[128] + literals. */
    char dir[2048];
    char slug[200];
    /* dir + "/NN_" + slug[200] + ".png". */
    char path[2560];

    if (!world || !world->console)
        return 0;
    
    /* Seed-only steps have no live console buffer yet. */
    if (world->console->master_fd < 0 && world->console->out_len == 0)
        return 0;

    const char* text = world->console->out;
    
    if (!text)
        text = "";

    slugify(step_text, slug, sizeof slug);
    
    snprintf(dir, sizeof dir, "%s/BDD/artifacts/%s/%s",
             world->repo_root, world->feature_stem, world->scenario_slug);
    
    if (mkdir_p(dir) != 0) {
        
        fprintf(stderr, "mkdir failed: %s\n", dir);
        return -1;
    }
    
    world->step_index++;
    
    snprintf(path, sizeof path, "%s/%02d_%s.png", dir, world->step_index, slug);

    const int wr = screenshot_write_png_unique(text, path, &g_last_screen_hash);
    
    if (wr < 0) {
        
        fprintf(stderr, "screenshot failed: %s\n", path);
        
        return -1;
    }
    
    if (wr == 1) {
        
        printf("  PNG: (skip duplicate screen)\n");
        
        world->step_index--;
        return 0;
    }
    
    printf("  PNG: %s\n", path);
    
    return 0;
}

static int line_is_tag_terminal(const char *line)
{
    return strstr(line, "@terminal") != NULL;
}

static int starts_with_kw(const char *line, const char *kw, const char **rest)
{
    const size_t n = strlen(kw);
    if (strncmp(line, kw, n) != 0)
        return 0;
    if (line[n] != ' ' && line[n] != '\t')
        return 0;
    *rest = line + n;
    while (**rest == ' ' || **rest == '\t')
        (*rest)++;
    return 1;
}

static void feature_stem_from_path(const char *path, char *stem, const size_t stemlen)
{
    const char* slash = strrchr(path, '/');
    const char* base = slash ? slash + 1 : path;
    
    snprintf(stem, stemlen, "%s", base);

    const size_t n = strlen(stem);
    
    if (n > 8 && strcmp(stem + n - 8, ".feature") == 0)
        stem[n - 8] = 0;
}

static int run_feature_file(const char *path)
{
    char line[1024];
    int in_terminal_scenario = 0;
    int scenario_failed = 0;
    int ran = 0;
    int failed = 0;
    char scenario_name[256];
    char feature_stem[128];
    Console console;
    StepWorld world;

    FILE* fp = fopen(path, "r");
    
    if (!fp) {
        perror(path);
        return -1;
    }

    feature_stem_from_path(path, feature_stem, sizeof feature_stem);
    
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
            snprintf(world.feature_stem, sizeof world.feature_stem, "%s", feature_stem);
            
            slugify(scenario_name, world.scenario_slug, sizeof world.scenario_slug);
            
            world.step_index = 0;
            g_last_screen_hash = 0;
            steps_begin_scenario();
            
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
            
            if (steps_dispatch(&world, line, rest) != 0) {
                
                fprintf(stderr, "  FAIL: %s\n", rest);
                
                scenario_failed = 1;
                failed++;
                world.failed = 1;
                
                (void)save_step_screenshot(&world, rest);
            } else {
                
                printf("  OK: %s\n", rest);
                
                if (save_step_screenshot(&world, rest) != 0) {
                
                    scenario_failed = 1;
                    failed++;
                    world.failed = 1;
                }
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
    const size_t n = strlen(name);
    const size_t s = strlen(suf);
    if (n < s)
        return 0;
    return strcmp(name + n - s, suf) == 0;
}

int main(const int argc, char **argv)
{
    /* g_repo_root[1024] + "/BDD/features". */
    char features_dir[1200];
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

    DIR* dir = opendir(features_dir);
    if (!dir) {
        perror(features_dir);
        return 2;
    }

    while ((ent = readdir(dir)) != NULL) {
        /* features_dir + "/" + d_name (NAME_MAX ~255). */
        char path[1536];
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
