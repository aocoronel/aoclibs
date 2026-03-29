#pragma once

#include "file.h"
#include "da.h"
#include "rc.h"
#include <dirent.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

size_t
read_by_delim(char **restrict lineptr, size_t *restrict n, int delim, FILE *restrict stream) {
    if (!lineptr || !n || !stream) {
        errno = EINVAL;
        return (size_t)-1;
    }

    if (*lineptr == NULL || *n == 0) {
        *n = 128;
        *lineptr = malloc(*n);
        if (!*lineptr) return (size_t)-1;
    }

    size_t pos = 0;

    for (;;) {
        int c = getc(stream);

        if (c == EOF) {
            if (pos == 0) return (size_t)-1;
            break;
        }

        if (pos + 1 >= *n) {
            size_t new_size = *n * 2;
            char *new_ptr = realloc(*lineptr, new_size);
            if (!new_ptr) return (size_t)-1;

            *lineptr = new_ptr;
            *n = new_size;
        }

        (*lineptr)[pos++] = (char)c;

        if (c == delim) break;
    }

    (*lineptr)[pos] = '\0';
    return pos;
}

int dir_walk(const char *path,
             bool recurse,
             dw_fn isdir,
             dw_fn isreg,
             dw_fn islnk,
             dw_fn isnull,
             dw_fn isempty) {
    ASSERT_NONNULL(path != NULL);

    DIR *dir = opendir(path);
    if (!dir) return errno;

    int8_t empty = 0;

    struct dirent *entry;
    char fullpath[DIR_WALKER_BUFF];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

        FileType file_t = get_filetype(fullpath);

        switch (file_t) {
        case F_REG:
            if (isreg != NULL) isreg(fullpath);
            break;
        case F_DIR:
            if (recurse) dir_walk(fullpath, recurse, isdir, isreg, islnk, isnull, isempty);
            if (isdir != NULL) isdir(fullpath);
            break;
        case F_LNK:
            if (islnk != NULL) islnk(fullpath);
            break;
        case F_NULL:
            if (isnull != NULL) isnull(fullpath);
            break;
        default:
            break;
        }

        empty++;
    }

    if (empty == 0 && isempty != NULL) isempty(path);

    closedir(dir);
    return 0;
}

FileType get_filetype(const char *path) {
    ASSERT_NONNULL(path != NULL);

    struct stat st;
    if (lstat(path, &st) == -1) return F_FAIL;

    if (S_ISREG(st.st_mode))
        return F_REG;
    else if (S_ISDIR(st.st_mode))
        return F_DIR;
    else if (S_ISLNK(st.st_mode))
        return F_LNK;

    return F_NULL;
}

rc read_entire_file(const char *filepath) {
    FILE *fp = fopen(filepath, "r");

    struct stat st;
    bool reserve = false;
    if (stat(filepath, &st) != -1) reserve = true;

    rc lines = { 0 };

    if (reserve == true) {
        da_reserve(&lines, st.st_size);
        int c = 0;
        while (lines.len < lines.cap) {
            c = getc(fp);
            if (c == EOF) break;
            da_insert_fast(&lines, c);
        }
    } else {
        da_reserve(&lines, 256);
        for (;;) {
            int c = getc(fp);

            if (c == EOF) {
                if (lines.len == 0) {
                    fclose(fp);
                    da_free(&lines);
                    return (rc){};
                }
                break;
            }

            da_insert(&lines, c);
        }
    }

    da_add_null(&lines);

    fclose(fp);
    return lines;
}
