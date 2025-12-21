#ifndef AOCLIBS_FS_DIR_H
#define AOCLIBS_FS_DIR_H

#include <stdbool.h>
#include <aoclibs/fs/types.h>
#include <aoclibs/error.h>

typedef void (*dw_dir)(const char *path);
typedef void (*dw_reg)(const char *path);
typedef void (*dw_lnk)(const char *path);
typedef void (*dw_null)(const char *path);
typedef void (*dw_empty)(const char *path);

/*
 * Walks into a directory and read it's content
 *
 * dir_walker can run four user provided functions based on each filetype:
 * reg, directory, lnk and null.
 *
 * If the user prefers to ignore a certain filetype, the function values can be
 * passed as NULL.
 *
 * When recurse is set to true, when a directory is found, dir_walker will recurse
 * in it.
 *
 * Defines:
 * - NDEBUG :: Disable asserts
 *
 * Failure: errno << opendir
*/
Err dir_walker(const char *_Nonnull path, bool recurse, dw_dir isdir, dw_reg isreg,
               dw_lnk islnk, dw_null isnull, dw_empty isempty);

#endif
