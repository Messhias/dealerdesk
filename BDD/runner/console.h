/*
 * console.h - drive the real dealerdesk binary over a terminal session
 */
#ifndef CONSOLE_H
#define CONSOLE_H

#include <stddef.h>

typedef struct {
    int master_fd;
    int pid;
    char workdir[512];
    char binary[1024];
    char out[65536];
    size_t out_len;
    int owns_workdir;
} Console;

int console_resolve_binary(char *buf, size_t buflen);
int console_prepare_workdir(char *workdir, size_t buflen, const char *repo_root);
int console_prepare_empty_workdir(char *workdir, size_t buflen);
int console_start(Console *c, const char *binary, const char *workdir);
void console_stop(Console *c);
int console_expect(Console *c, const char *needle, int timeout_ms);
int console_sendline(Console *c, const char *line);
int console_saw(const Console *c, const char *needle);
int console_saw_from(const Console *c, size_t from, const char *needle);

#endif
