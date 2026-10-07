#define _XOPEN_SOURCE 700
#include "ls.h"

#include <errno.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

void print_error(const char *path) {
    fprintf(stderr, "ls: %s: %s\n", path, strerror(errno));
}

int is_directory(const struct stat *st) {
    return st != NULL && S_ISDIR(st->st_mode);
}

char *join_path(const char *dir, const char *name) {
    if (dir == NULL || name == NULL) {
        return NULL;
    }

    const size_t dir_len = strlen(dir);
    const size_t name_len = strlen(name);
    const bool need_slash = dir_len == 0 || dir[dir_len - 1] != '/';

    if (dir_len > LS_MAX_PATH || name_len > LS_MAX_PATH ||
        dir_len + name_len + (need_slash ? 1U : 0U) + 1U > LS_MAX_PATH) {
        errno = ENAMETOOLONG;
        return NULL;
    }

    const size_t total = dir_len + name_len + (need_slash ? 1U : 0U) + 1U;
    char *result = malloc(total);
    if (result == NULL) {
        return NULL;
    }

    if (need_slash) {
        snprintf(result, total, "%s/%s", dir, name);
    } else {
        snprintf(result, total, "%s%s", dir, name);
    }
    return result;
}

char *get_link_target(const char *path) {
    if (path == NULL) {
        return NULL;
    }

    size_t size = 256;
    for (;;) {
        char *buffer = malloc(size);
        if (buffer == NULL) {
            return NULL;
        }

        const ssize_t len = readlink(path, buffer, size - 1U);
        if (len < 0) {
            free(buffer);
            return NULL;
        }
        if ((size_t)len < size - 1U) {
            buffer[len] = '\0';
            return buffer;
        }

        free(buffer);
        if (size >= LS_MAX_PATH) {
            errno = ENAMETOOLONG;
            return NULL;
        }
        size *= 2U;
        if (size > LS_MAX_PATH) {
            size = LS_MAX_PATH;
        }
    }
}

char *format_mode(mode_t mode) {
    char *out = calloc(11, sizeof(char));
    if (out == NULL) {
        return NULL;
    }

    out[0] = S_ISDIR(mode) ? 'd' :
             S_ISLNK(mode) ? 'l' :
             S_ISCHR(mode) ? 'c' :
             S_ISBLK(mode) ? 'b' :
             S_ISFIFO(mode) ? 'p' :
             S_ISSOCK(mode) ? 's' : '-';

    const char symbols[] = "rwxrwxrwx";
    for (int i = 0; i < 9; ++i) {
        out[i + 1] = (mode & (1U << (8 - i))) ? symbols[i] : '-';
    }

    if (mode & S_ISUID) out[3] = (out[3] == 'x') ? 's' : 'S';
    if (mode & S_ISGID) out[6] = (out[6] == 'x') ? 's' : 'S';
    if (mode & S_ISVTX) out[9] = (out[9] == 'x') ? 't' : 'T';

    return out;
}

char *format_human_size(off_t size) {
    const char *units[] = { "B", "K", "M", "G", "T", "P" };
    double value = (double)size;
    size_t unit = 0;

    while (value >= 1024.0 && unit < sizeof(units) / sizeof(units[0]) - 1U) {
        value /= 1024.0;
        ++unit;
    }

    char *out = malloc(32);
    if (out == NULL) {
        return NULL;
    }

    if (unit == 0) {
        snprintf(out, 32, "%lld%s", (long long)size, units[unit]);
    } else if (value < 10.0) {
        snprintf(out, 32, "%.1f%s", value, units[unit]);
    } else {
        snprintf(out, 32, "%.0f%s", value, units[unit]);
    }
    return out;
}

char *format_time(time_t value) {
    char *out = malloc(64);
    if (out == NULL) {
        return NULL;
    }

    struct tm tm_value;
    if (localtime_r(&value, &tm_value) == NULL || strftime(out, 64, "%b %d %H:%M", &tm_value) == 0) {
        free(out);
        return NULL;
    }
    return out;
}

void print_total_blocks(const EntryList *list) {
    unsigned long long total = 0;
    if (list == NULL) {
        return;
    }

    for (size_t i = 0; i < list->size; ++i) {
        total += (unsigned long long)list->items[i].st.st_blocks;
    }
    printf("total %llu\n", total / 2ULL);
}

/* Internal helpers used by format.c. */
const char *owner_name(uid_t uid) {
    struct passwd *pw = getpwuid(uid);
    return pw != NULL ? pw->pw_name : "?";
}

const char *group_name(gid_t gid) {
    struct group *gr = getgrgid(gid);
    return gr != NULL ? gr->gr_name : "?";
}
