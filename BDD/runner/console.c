/*
 * console.c - terminal session driver for dealerdesk (POSIX)
 */
#if defined(__APPLE__)
#define _DARWIN_C_SOURCE
#else
#define _DEFAULT_SOURCE
#endif

#include "console.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#if defined(__APPLE__)
#include <util.h>
#else
#include <pty.h>
#endif

static int path_is_file(const char *path)
{
    struct stat st;
    return path && stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

int console_resolve_binary(char *buf, size_t buflen)
{
    const char *env = getenv("DEALERDESK_BIN");
    const char *candidates[4];
    int n = 0;
    int i;

    if (!buf || buflen < 8)
        return -1;

    if (env && env[0]) {
        if (!path_is_file(env)) {
            fprintf(stderr, "DEALERDESK_BIN not found: %s\n", env);
            return -1;
        }
        snprintf(buf, buflen, "%s", env);
        return 0;
    }

    if (access("/.dockerenv", F_OK) == 0 || getenv("DEALERDESK_BDD_DOCKER")) {
        candidates[n++] = "build-docker/dealerdesk";
        candidates[n++] = "build/dealerdesk";
    } else {
        candidates[n++] = "build/dealerdesk";
        candidates[n++] = "build-docker/dealerdesk";
    }

    for (i = 0; i < n; i++) {
        if (path_is_file(candidates[i])) {
            snprintf(buf, buflen, "%s", candidates[i]);
            return 0;
        }
    }

    fprintf(stderr,
            "dealerdesk binary not found. Build with make (host) or "
            "cmake in build-docker, or set DEALERDESK_BIN.\n");
    return -1;
}

static int copy_file(const char *src, const char *dst)
{
    FILE *in;
    FILE *out;
    char block[4096];
    size_t n;

    in = fopen(src, "rb");
    if (!in)
        return -1;
    out = fopen(dst, "wb");
    if (!out) {
        fclose(in);
        return -1;
    }
    while ((n = fread(block, 1, sizeof block, in)) > 0) {
        if (fwrite(block, 1, n, out) != n) {
            fclose(in);
            fclose(out);
            return -1;
        }
    }
    fclose(in);
    fclose(out);
    return 0;
}

int console_prepare_workdir(char *workdir, size_t buflen, const char *repo_root)
{
    char tmpl[] = "/tmp/ddbddXXXXXX";
    char *root;
    char src[768];
    char dst[768];
    char data_dir[600];
    static const char *files[] = {
        "stock.dat", "customers.dat", "invoices.dat", "next_ids.dat", NULL
    };
    int i;

    root = mkdtemp(tmpl);
    if (!root)
        return -1;

    snprintf(workdir, buflen, "%s", root);
    snprintf(data_dir, sizeof data_dir, "%s/data", workdir);
    if (mkdir(data_dir, 0700) != 0)
        return -1;

    for (i = 0; files[i]; i++) {
        snprintf(src, sizeof src, "%s/data/%s", repo_root, files[i]);
        snprintf(dst, sizeof dst, "%s/data/%s", workdir, files[i]);
        if (copy_file(src, dst) != 0) {
            fprintf(stderr, "failed to copy %s\n", src);
            return -1;
        }
    }
    return 0;
}

static void clear_console(Console *c)
{
    memset(c, 0, sizeof *c);
    c->master_fd = -1;
    c->pid = -1;
}

int console_start(Console *c, const char *binary, const char *workdir)
{
    int master = -1;
    pid_t pid;
    char abs_bin[1024];

    clear_console(c);
    if (!binary || !workdir)
        return -1;

    if (binary[0] == '/') {
        snprintf(abs_bin, sizeof abs_bin, "%s", binary);
    } else {
        char cwd[512];
        if (!getcwd(cwd, sizeof cwd))
            return -1;
        snprintf(abs_bin, sizeof abs_bin, "%s/%s", cwd, binary);
    }

    snprintf(c->workdir, sizeof c->workdir, "%s", workdir);
    c->owns_workdir = 1;

    {
        struct winsize ws;
        memset(&ws, 0, sizeof ws);
        ws.ws_row = 36;
        ws.ws_col = 100;
        pid = forkpty(&master, NULL, NULL, &ws);
    }
    if (pid < 0) {
        perror("forkpty");
        return -1;
    }

    if (pid == 0) {
        if (chdir(workdir) != 0)
            _exit(127);
        execl(abs_bin, abs_bin, (char *)NULL);
        _exit(127);
    }

    c->master_fd = master;
    c->pid = (int)pid;
    return 0;
}

void console_stop(Console *c)
{
    if (!c)
        return;

    if (c->master_fd >= 0) {
        const char quit[] = "0\n";
        (void)write(c->master_fd, quit, sizeof quit - 1);
        close(c->master_fd);
        c->master_fd = -1;
    }

    if (c->pid > 0) {
        int status = 0;
        int i;
        for (i = 0; i < 20; i++) {
            pid_t r = waitpid(c->pid, &status, WNOHANG);
            if (r == c->pid || (r < 0 && errno == ECHILD))
                break;
            usleep(50000);
        }
        if (waitpid(c->pid, &status, WNOHANG) == 0) {
            kill(c->pid, SIGKILL);
            waitpid(c->pid, &status, 0);
        }
        c->pid = -1;
    }

    if (c->owns_workdir && c->workdir[0]) {
        char path[600];
        static const char *files[] = {
            "stock.dat", "customers.dat", "invoices.dat", "next_ids.dat", NULL
        };
        int i;
        for (i = 0; files[i]; i++) {
            snprintf(path, sizeof path, "%s/data/%s", c->workdir, files[i]);
            unlink(path);
        }
        snprintf(path, sizeof path, "%s/data", c->workdir);
        rmdir(path);
        rmdir(c->workdir);
        c->workdir[0] = 0;
    }
}

int console_saw(const Console *c, const char *needle)
{
    if (!c || !needle || !needle[0])
        return 0;
    return strstr(c->out, needle) != NULL;
}

static int append_read(Console *c)
{
    char buf[2048];
    ssize_t n;

    n = read(c->master_fd, buf, sizeof buf);
    if (n < 0) {
        if (errno == EAGAIN || errno == EINTR)
            return 0;
        return -1;
    }
    if (n == 0)
        return 0;
    if (c->out_len + (size_t)n >= sizeof c->out)
        n = (ssize_t)(sizeof c->out - c->out_len - 1);
    if (n <= 0)
        return 0;
    memcpy(c->out + c->out_len, buf, (size_t)n);
    c->out_len += (size_t)n;
    c->out[c->out_len] = 0;
    return 1;
}

int console_expect(Console *c, const char *needle, int timeout_ms)
{
    int elapsed = 0;
    const int slice = 50;

    if (!c || c->master_fd < 0 || !needle)
        return -1;
    if (console_saw(c, needle))
        return 0;

    while (elapsed <= timeout_ms) {
        fd_set rfds;
        struct timeval tv;
        int ready;

        FD_ZERO(&rfds);
        FD_SET(c->master_fd, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = slice * 1000;

        ready = select(c->master_fd + 1, &rfds, NULL, NULL, &tv);
        if (ready > 0 && FD_ISSET(c->master_fd, &rfds)) {
            if (append_read(c) < 0)
                return -1;
            if (console_saw(c, needle))
                return 0;
        } else if (ready == 0) {
            elapsed += slice;
        } else if (errno != EINTR) {
            return -1;
        }
    }

    fprintf(stderr, "expect timeout waiting for: %s\n", needle);
    return -1;
}

int console_sendline(Console *c, const char *line)
{
    char buf[512];
    size_t len;
    ssize_t n;

    if (!c || c->master_fd < 0 || !line)
        return -1;

    snprintf(buf, sizeof buf, "%s\n", line);
    len = strlen(buf);
    n = write(c->master_fd, buf, len);
    if (n < 0 || (size_t)n != len)
        return -1;
    return 0;
}
