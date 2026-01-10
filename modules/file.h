#ifndef AOCLIBS_FILE_H_
#define AOCLIBS_FILE_H_

#include "./base/attributes.h"
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>

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
int aoc_dir_walk(const char *ref path, bool recurse, dw_fn isdir, dw_fn isreg, dw_fn islnk,
                 dw_fn isnull, dw_fn isempty);

/*
 * Stat the file and return its type
 *
 * Defines:
 * - NDEBUG :: Disable asserts
*/
FileType aoc_get_filetype(const char *ref path);

#endif // AOCLIBS_FILE_H_
