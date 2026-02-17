#ifndef AOCLIBS_FILE_H_
#define AOCLIBS_FILE_H_

#define _GNU_SOURCE
#include "base.h"
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define AOC_DIR_WALKER_BUFF 4096

typedef enum FileType {
        F_NULL, // Doesn't exist
        F_REG, // Regular file
        F_DIR, // Directory
        F_LNK, // Symbolic Link
} FileType;

typedef void (*dw_fn)(const char *path);

/*
 * Walks into a directory and read it's content
 *
 * aoc_dir_walk can run four user provided functions based on each filetype:
 * reg, directory, lnk and null.
 *
 * If the user prefers to ignore a certain filetype, the function values can be
 * passed as NULL.
 *
 * When recurse is set to true, when a directory is found, aoc_dir_walk will recurse
 * in it.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 *
 * Failure: errno << opendir
*/
int aoc_dir_walk(const char *ref path,
                 bool recurse,
                 dw_fn isdir,
                 dw_fn isreg,
                 dw_fn islnk,
                 dw_fn isnull,
                 dw_fn isempty);

/*
 * Stat the file and return its type
 *
 * Defines:
 * - NDEBUG :: Disable asserts
*/
FileType aoc_get_filetype(const char *ref path);

/*
 * Reads file, splitting the read buffer by the delimiter
 *
 * Allocates to lineptr. The user owns the allocation.
*/
size_t read_by_delim(char **xref lineptr, size_t *xref n, int delim, FILE *xref stream);

#ifdef AOCLIBS_IMPLEMENTATION
size_t read_by_delim(char **xref lineptr, size_t *xref n, int delim, FILE *xref stream) {
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

int aoc_dir_walk(const char *ref path,
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
        char fullpath[AOC_DIR_WALKER_BUFF];

        while ((entry = readdir(dir)) != NULL) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

                snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

                FileType file_t = aoc_get_filetype(fullpath);

                switch (file_t) {
                case F_REG:
                        if (isreg != NULL) isreg(fullpath);
                        break;
                case F_DIR:
                        if (recurse)
                                aoc_dir_walk(
                                        fullpath, recurse, isdir, isreg, islnk, isnull, isempty);
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

FileType aoc_get_filetype(const char *ref path) {
        ASSERT_NONNULL(path != NULL);

        struct stat st;
        if (lstat(path, &st) == -1) return F_NULL;

        if (S_ISREG(st.st_mode))
                return F_REG;
        else if (S_ISDIR(st.st_mode))
                return F_DIR;
        else if (S_ISLNK(st.st_mode))
                return F_LNK;

        return F_NULL;
}
#endif

#endif // AOCLIBS_FILE_H_
