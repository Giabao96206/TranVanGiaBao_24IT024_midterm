#define _XOPEN_SOURCE 700
#include "ls.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static int process_operands(int count, char **paths, const Options *opts) {
    int first_error = 0;

    /* ls prints non-directory operands before directories. */
    for (int pass = 0; pass < 2; ++pass) {
        for (int i = 0; i < count; ++i) {
            struct stat st;
            if (lstat(paths[i], &st) != 0) {
                print_error(paths[i]);
                first_error = 1;
                continue;
            }

            const int is_dir = S_ISDIR(st.st_mode);
            if ((pass == 0 && is_dir) || (pass == 1 && !is_dir)) {
                continue;
            }

            if (pass == 0) {
                /* Add spacing only between multiple operands when needed. */
                if (count > 1) {
                    if (opts->long_format) {
                        print_long_entry(&(FileEntry){
                            .name = paths[i], .path = paths[i], .st = st
                        }, opts, 0, NULL);
                    } else {
                        print_short_entry(&(FileEntry){
                            .name = paths[i], .path = paths[i], .st = st
                        });
                    }
                    if (i + 1 < count) {
                        putchar('\n');
                    }
                } else {
                    if (opts->long_format) {
                        char *target = S_ISLNK(st.st_mode) ? get_link_target(paths[i]) : NULL;
                        print_long_entry(&(FileEntry){
                            .name = paths[i], .path = paths[i], .st = st
                        }, opts, 0, target);
                        free(target);
                    } else {
                        print_short_entry(&(FileEntry){
                            .name = paths[i], .path = paths[i], .st = st
                        });
                    }
                }
            }
        }
    }

    /* Directories are handled separately so each directory gets a header when
       more than one path is involved. */
    int directory_count = 0;
    for (int i = 0; i < count; ++i) {
        struct stat st;
        if (lstat(paths[i], &st) == 0 && S_ISDIR(st.st_mode)) {
            directory_count++;
        }
    }

    bool first_directory = true;
    for (int i = 0; i < count; ++i) {
        struct stat st;
        if (lstat(paths[i], &st) != 0 || !S_ISDIR(st.st_mode)) {
            continue;
        }

        if (!first_directory || count > 1) {
            if (directory_count > 1 || count > 1) {
                putchar('\n');
            }
        }

        if (list_directory(paths[i], opts, count > 1) != 0) {
            first_error = 1;
        }
        first_directory = false;
    }

    return first_error;
}

int main(int argc, char **argv) {
    Options opts;
    int first_path = 0;

    options_init(&opts);
    if (parse_options(argc, argv, &opts, &first_path) != 0) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (first_path >= argc) {
        char *default_path[] = { "." };
        return process_operands(1, default_path, &opts) ? EXIT_FAILURE : EXIT_SUCCESS;
    }

    return process_operands(argc - first_path, &argv[first_path], &opts)
               ? EXIT_FAILURE
               : EXIT_SUCCESS;
}
