#ifndef AOCLIBS_FILE_H_
#define AOCLIBS_FILE_H_

#define _GNU_SOURCE
#include "base.h"
#include <stdbool.h>
#include <stdio.h>

#define AOC_DIR_WALKER_BUFF 4096

typedef enum FileType {
        F_NULL, // Doesn't exist
        F_REG, // Regular file
        F_DIR, // Directory
        F_LNK, // Symbolic Link
        F_FAIL, // Failed to stat
} FileType;

typedef void (*dw_fn)(const char *path);

// Walks into a directory and read it's content
//
// aoc_dir_walk can run four user provided functions based on each filetype:
// reg, directory, lnk and null.
//
// If the user prefers to ignore a certain filetype, the function values can be
// passed as NULL.
//
// When recurse is set to true, when a directory is found, aoc_dir_walk will recurse
// in it.
//
// Sets errno << opendir
int aoc_dir_walk(const char *ref path,
                 bool recurse,
                 dw_fn isdir,
                 dw_fn isreg,
                 dw_fn islnk,
                 dw_fn isnull,
                 dw_fn isempty);

// Stat the file and return its type
//
// F_FAIL :: failed to stat. Sets errno << lstat
FileType aoc_get_filetype(const char *ref path);

// Reads file, splitting the read buffer by the delimiter.
// Returns how many bytes has been read.
//
// Allocates to lineptr. The user owns the allocation.
size_t read_by_delim(char **xref lineptr, size_t *xref n, int delim, FILE *xref stream);

#ifdef AOCLIBS_IMPLEMENTATION
#include "file.c"
#endif

#endif // AOCLIBS_FILE_H_
