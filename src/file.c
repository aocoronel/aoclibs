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

int dir_walker(const char *path, DirWalker *dw) {
    ASSERT_NONNULL(path != NULL);

    DIR *dir = opendir(path);
    if (!dir) return errno;

    int8_t empty = 0;

    struct stat st;
    struct dirent *entry;
    char fullpath[DIR_WALKER_BUFF];

    while ((entry = readdir(dir)) != NULL) {
        if (cstr_eq(entry->d_name, ".") || cstr_eq(entry->d_name, "..")) continue;

        cstr_fmt_write(fullpath, DIR_WALKER_BUFF, "%s/%s", path, entry->d_name);

        FileMetadata metadata = { 0 };

        if (dw->metadata) {
            metadata = get_file_data(&st, fullpath);
        } else {
            FileType file_t = get_filetype(fullpath);
            metadata.type = file_t;
        }

        switch (metadata.type) {
        case F_REG:
            if (dw->isreg != NULL) dw->isreg(&metadata);
            break;
        case F_DIR:
            if (dw->isdir != NULL) dw->isdir(&metadata, dw);
            break;
        case F_LNK:
            if (dw->islnk != NULL) dw->islnk(&metadata);
            break;
        case F_NULL:
            if (dw->isnull != NULL) dw->isnull(&metadata);
            break;
        default:
            break;
        }

        empty++;
    }

    if (empty == 0 && dw->isempty != NULL) dw->isempty(path);

    closedir(dir);
    return 0;
}

FileMetadata get_file_data(struct stat *st, const char *path) {
    ASSERT_NONNULL(path != NULL);

    if (lstat(path, st) == -1) return (FileMetadata){ .type = F_FAIL, .stat = NULL };

    if (S_ISREG(st->st_mode))
        return (FileMetadata){ .type = F_REG, .stat = st };
    else if (S_ISDIR(st->st_mode))
        return (FileMetadata){ .type = F_DIR, .stat = st };
    else if (S_ISLNK(st->st_mode))
        return (FileMetadata){ .type = F_LNK, .stat = st };

    return (FileMetadata){ .type = F_FAIL, .stat = st };
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

bool read_entire_file(rc *lines, const char *filepath) {
    FILE *fp = fopen(filepath, "r");

    if (!fp) return false;

    struct stat st;
    bool reserve = false;
    if (stat(filepath, &st) != -1) return false;

    da_reserve(lines, (size_t)st.st_size);
    size_t n = fread(lines->data, sizeof(char), st.st_size, fp);
    lines->len = n;

    da_add_null(lines);

    fclose(fp);
    return lines;
}
