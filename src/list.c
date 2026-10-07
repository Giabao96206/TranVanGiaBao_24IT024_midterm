#define _XOPEN_SOURCE 700
#include "ls.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int compare_entries(const void *a, const void *b) {
    const FileEntry *ea = a;
    const FileEntry *eb = b;
    return strcmp(ea->name, eb->name);
}

static void init_entry_list(EntryList *list) {
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

static int append_entry(EntryList *list, const char *name, const char *path,
                        const struct stat *st) {
    if (list->size == list->capacity) {
        size_t new_capacity = list->capacity == 0 ? 16 : list->capacity * 2;
        FileEntry *new_items = realloc(list->items, new_capacity * sizeof(*new_items));
        if (new_items == NULL) {
            return -1;
        }
        list->items = new_items;
        list->capacity = new_capacity;
    }

    list->items[list->size].name = strdup(name);
    list->items[list->size].path = strdup(path);
    if (list->items[list->size].name == NULL || list->items[list->size].path == NULL) {
        free(list->items[list->size].name);
        free(list->items[list->size].path);
        return -1;
    }
    list->items[list->size].st = *st;
    list->size++;
    return 0;
}

void free_entry_list(EntryList *list) {
    if (list == NULL) return;
    for (size_t i = 0; i < list->size; ++i) {
        free(list->items[i].name);
        free(list->items[i].path);
    }
    free(list->items);
    list->items = NULL;
    list->size = 0;
    list->capacity = 0;
}

static int should_show(const char *name, const Options *opts) {
    if (opts->show_all) return 1;
    return name[0] != '.';
}

static int read_directory(const char *path, const Options *opts, EntryList *list) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        print_error(path);
        return -1;
    }

    errno = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!should_show(entry->d_name, opts)) {
            continue;
        }

        char *full_path = join_path(path, entry->d_name);
        if (full_path == NULL) {
            fprintf(stderr, "ls: cannot build path for '%s/%s'\n", path, entry->d_name);
            closedir(dir);
            return -1;
        }

        struct stat st;
        if (lstat(full_path, &st) != 0) {
            print_error(full_path);
            free(full_path);
            continue;
        }

        if (append_entry(list, entry->d_name, full_path, &st) != 0) {
            fprintf(stderr, "ls: memory allocation failed\n");
            free(full_path);
            closedir(dir);
            return -1;
        }
        free(full_path);
    }

    if (errno != 0) {
        print_error(path);
        closedir(dir);
        return -1;
    }

    if (closedir(dir) != 0) {
        print_error(path);
        return -1;
    }

    qsort(list->items, list->size, sizeof(list->items[0]), compare_entries);
    return 0;
}

static void print_entries(const EntryList *list, const Options *opts) {
    if (opts->long_format) {
        if (list->size > 0) {
            print_total_blocks(list);
        }

        const int time_width = calculate_time_width(list);
        for (size_t i = 0; i < list->size; ++i) {
            char *target = S_ISLNK(list->items[i].st.st_mode)
                               ? get_link_target(list->items[i].path)
                               : NULL;
            print_long_entry(&list->items[i], opts, time_width, target);
            free(target);
        }
        return;
    }

    for (size_t i = 0; i < list->size; ++i) {
        print_short_entry(&list->items[i]);
        if (i + 1 < list->size) {
            putchar('\n');
        }
    }
    if (list->size > 0) {
        putchar('\n');
    }
}

static int list_directory_recursive(const char *path, const Options *opts,
                                    bool show_header, bool is_root) {
    EntryList list;
    init_entry_list(&list);

    if (read_directory(path, opts, &list) != 0) {
        return -1;
    }

    if (show_header || !is_root) {
        printf("%s:\n", path);
    }

    print_entries(&list, opts);

    int result = 0;
    if (opts->recursive) {
        bool printed_subdir = false;
        for (size_t i = 0; i < list.size; ++i) {
            if (!S_ISDIR(list.items[i].st.st_mode)) {
                continue;
            }
            if (strcmp(list.items[i].name, ".") == 0 || strcmp(list.items[i].name, "..") == 0) {
                continue;
            }

            if (printed_subdir || list.size > 0) putchar('\n');
            if (list_directory_recursive(list.items[i].path, opts, true, false) != 0) {
                result = -1;
            }
            printed_subdir = true;
        }
    }

    free_entry_list(&list);
    return result;
}

int list_directory(const char *path, const Options *opts, bool show_header) {
    return list_directory_recursive(path, opts, show_header, true);
}

int list_path(const char *path, const Options *opts, bool show_header) {
    struct stat st;
    if (lstat(path, &st) != 0) {
        print_error(path);
        return -1;
    }

    if (S_ISDIR(st.st_mode)) {
        return list_directory(path, opts, show_header);
    }

    FileEntry entry = {
        .name = (char *)path,
        .path = (char *)path,
        .st = st
    };

    if (opts->long_format) {
        char *target = S_ISLNK(st.st_mode) ? get_link_target(path) : NULL;
        print_long_entry(&entry, opts, 0, target);
        free(target);
    } else {
        print_short_entry(&entry);
        putchar('\n');
    }
    return 0;
}
