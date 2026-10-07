#define _XOPEN_SOURCE 700
#include "ls.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_short_entry(const FileEntry *entry) {
    if (entry == NULL || entry->name == NULL) {
        return;
    }
    fputs(entry->name, stdout);
}

void print_long_entry(const FileEntry *entry, const Options *opts, int time_width,
                      const char *link_target) {
    (void)time_width; /* Reserved for future column-layout refinements. */

    if (entry == NULL || opts == NULL) {
        return;
    }

    char *mode = format_mode(entry->st.st_mode);
    char *time_text = format_time(entry->st.st_mtime);
    char *size_text = opts->human_readable
                          ? format_human_size(entry->st.st_size)
                          : NULL;

    if (mode == NULL || time_text == NULL ||
        (opts->human_readable && size_text == NULL)) {
        free(mode);
        free(time_text);
        free(size_text);
        return;
    }

    if (!opts->human_readable) {
        char numeric_size[64];
        snprintf(numeric_size, sizeof(numeric_size), "%lld",
                 (long long)entry->st.st_size);
        printf("%s %2lu %-8s %-8s %8s %s  %s",
               mode,
               (unsigned long)entry->st.st_nlink,
               owner_name(entry->st.st_uid),
               group_name(entry->st.st_gid),
               numeric_size,
               time_text,
               entry->name);
    } else {
        printf("%s %2lu %-8s %-8s %8s %s  %s",
               mode,
               (unsigned long)entry->st.st_nlink,
               owner_name(entry->st.st_uid),
               group_name(entry->st.st_gid),
               size_text,
               time_text,
               entry->name);
    }

    if (S_ISLNK(entry->st.st_mode) && link_target != NULL) {
        printf(" -> %s", link_target);
    }
    putchar('\n');

    free(mode);
    free(time_text);
    free(size_text);
}

size_t calculate_blocks(const EntryList *list) {
    size_t blocks = 0;
    if (list == NULL) {
        return 0;
    }
    for (size_t i = 0; i < list->size; ++i) {
        if (list->items[i].st.st_blocks > 0) {
            blocks += (size_t)list->items[i].st.st_blocks;
        }
    }
    return blocks;
}

int calculate_time_width(const EntryList *list) {
    int width = 0;
    if (list == NULL) {
        return width;
    }
    for (size_t i = 0; i < list->size; ++i) {
        char *time_text = format_time(list->items[i].st.st_mtime);
        if (time_text != NULL) {
            const int current = (int)strlen(time_text);
            if (current > width) {
                width = current;
            }
            free(time_text);
        }
    }
    return width;
}
