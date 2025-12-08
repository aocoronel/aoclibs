#ifndef AOCLIBS_FS_DIR_WALKER_H
#define AOCLIBS_FS_DIR_WALKER_H

#include <stdbool.h>
#include <a_fs_types.h>
#include <a_error_types.h>

typedef void (*dw_dir)(const char *path);
typedef void (*dw_regular)(const char *path);
typedef void (*dw_symlink)(const char *path);
typedef void (*dw_null)(const char *path);

/*
 * Walks into a directory and read it's content
 *
 * dir_walker can run four user provided functions based on each filetype:
 * regular, directory, symlink and null.
 *
 * If the user prefers to ignore a certain filetype, the function values can be
 * passed as NULL.
 *
 * When recurse is set to true, when a directory is found, dir_walker will recurse
 * in it.
 *
 * Error Values:
 * Success: ok
 * Fail: errno
*/
Error dir_walker(const char *path, bool recurse, dw_dir isdir,
                dw_regular isregular, dw_symlink issymlink, dw_null isnull);

#endif
