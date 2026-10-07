#ifndef LS_H
#define LS_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>

#define LS_MAX_PATH 4096

typedef struct {
    bool show_all;         /* -a */
    bool long_format;      /* -l */
    bool human_readable;   /* -h */
    bool recursive;        /* -R */
} Options;

typedef struct {
    char *name;
    char *path;
    struct stat st;
} FileEntry;

typedef struct {
    FileEntry *items;
    size_t size;
    size_t capacity;
} EntryList;

/* options.c */
void options_init(Options *opts);
int parse_options(int argc, char **argv, Options *opts, int *first_path);
void print_usage(const char *program);

/* list.c */
int list_path(const char *path, const Options *opts, bool show_header);
int list_directory(const char *path, const Options *opts, bool show_header);
void free_entry_list(EntryList *list);

/* format.c */
void print_short_entry(const FileEntry *entry);
void print_long_entry(const FileEntry *entry, const Options *opts, int time_width,
                      const char *link_target);
size_t calculate_blocks(const EntryList *list);
int calculate_time_width(const EntryList *list);

/* utils.c */
void print_error(const char *path);
int is_directory(const struct stat *st);
char *join_path(const char *dir, const char *name);
char *get_link_target(const char *path);
char *format_mode(mode_t mode);
char *format_human_size(off_t size);
char *format_time(time_t value);
void print_total_blocks(const EntryList *list);
const char *owner_name(uid_t uid);
const char *group_name(gid_t gid);

#endif /* LS_H */
