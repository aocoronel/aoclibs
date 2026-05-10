#pragma once

#include "base.h"
#include "cstr.h"
#include "printfc.h"
#include "slices.h"
#include "da.h"
#include "file.h"
#include "rc.h"

#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

// akin to libc getdelim
size_t read_by_delim(char **restrict lineptr,
                     size_t *restrict n,
                     const char delim,
                     FILE *restrict stream) {
    ASSERT_NONNULL(lineptr);
    ASSERT_NONNULL(n);
    ASSERT_NONNULL(stream);

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

bool expand_path(rc *output, Slice *path) {
    ASSERT_NONNULL(path);
    ASSERT_NONNULL(output);

    const char *home = getenv("HOME");
    if (!home) {
        printfc_debug("failed to get $HOME environment value\n");
        return false;
    }
    size_t home_len = strlen(home);

    char cwd[4096] = { 0 };
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printfc_debug("failed to get current working directory\n");
        return false;
    }
    size_t cwd_len = strlen(cwd);

    const char *pos = path->data;
    size_t len = path->len;

    if (path->len > 2) {
        if (pos[0] == '~' && pos[1] == '/') {
            rc_cat(output, home, home_len);
            da_insert(output, '/');
            pos += 2;
            len -= 2;
        } else if (pos[0] == '.' && pos[1] == '/') {
            rc_cat(output, cwd, cwd_len);
            da_insert(output, '/');
            pos += 2;
            len -= 2;
        }
    } else {
        if (pos[0] == '.' && pos[1] == '\0') {
            rc_cat(output, cwd, cwd_len);
            pos += 1;
            len -= 1;
        }
    }

    while (*pos) {
        if (*pos == '$') {
            pos++;

            char varname[256];
            size_t vi = 0;

            while ((*pos == '_' || isalnum((unsigned char)*pos)) && vi < sizeof(varname) - 1) {
                varname[vi++] = *pos++;
            }

            varname[vi] = '\0';

            const char *env = getenv(varname);
            if (env) rc_cat(output, env, strlen(env));

            continue;
        }

        da_insert(output, *pos);
        pos++;
    }

    rc_cat(output, pos, len);
    da_add_null(output);

    return true;
}

bool dir_walker(const char *path, DirWalker *dw) {
    ASSERT_NONNULL(path != NULL);
    ASSERT_NONNULL(dw);

    DIR *dir = opendir(path);
    if (!dir) return false;

    int8_t empty = 0;

    struct stat st;
    struct dirent *entry;
    char fullpath[DIR_WALKER_BUFF];

    while ((entry = readdir(dir)) != NULL) {
        if (cstr_eq(entry->d_name, ".") || cstr_eq(entry->d_name, "..")) continue;

        cstr_fmt_write(fullpath, DIR_WALKER_BUFF, "%s/%s", path, entry->d_name);

        FileMetadata metadata = get_filedata(&st, fullpath);

        metadata.name = fullpath;

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
    return true;
}

FileMetadata get_filedata(struct stat *st, const char *path) {
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

bool read_entire_file(rc *lines, const char *filepath) {
    ASSERT_NONNULL(lines);

    FILE *fp = fopen(filepath, "r");
    if (!fp) return false;

    struct stat st;
    if (stat(filepath, &st) == -1) {
        fclose(fp);
        return false;
    }

    da_reserve(lines, (size_t)st.st_size);
    size_t n = fread(lines->data, sizeof(char), st.st_size, fp);
    lines->len = n;

    da_add_null(lines);

    fclose(fp);
    return true;
}
