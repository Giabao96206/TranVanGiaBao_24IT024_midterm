#include "ls.h"

#include <stdio.h>
#include <string.h>

void options_init(Options *opts) {
    if (opts == NULL) {
        return;
    }
    opts->show_all = false;
    opts->long_format = false;
    opts->human_readable = false;
    opts->recursive = false;
}

void print_usage(const char *program) {
    fprintf(stderr,
            "Usage: %s [-alhR] [FILE ...]\n"
            "\n"
            "Options:\n"
            "  -a   include hidden files (names beginning with '.')\n"
            "  -l   use long listing format\n"
            "  -h   show sizes in human-readable units (used with -l)\n"
            "  -R   list subdirectories recursively\n",
            program);
}

int parse_options(int argc, char **argv, Options *opts, int *first_path) {
    if (opts == NULL || first_path == NULL) {
        return -1;
    }

    int i = 1;
    for (; i < argc; ++i) {
        const char *arg = argv[i];
        if (strcmp(arg, "--") == 0) {
            ++i;
            break;
        }
        if (arg[0] != '-' || arg[1] == '\0') {
            break;
        }

        for (size_t j = 1; arg[j] != '\0'; ++j) {
            switch (arg[j]) {
                case 'a':
                    opts->show_all = true;
                    break;
                case 'l':
                    opts->long_format = true;
                    break;
                case 'h':
                    opts->human_readable = true;
                    break;
                case 'R':
                    opts->recursive = true;
                    break;
                default:
                    fprintf(stderr, "ls: invalid option -- '%c'\n", arg[j]);
                    return -1;
            }
        }
    }

    *first_path = i;
    return 0;
}
