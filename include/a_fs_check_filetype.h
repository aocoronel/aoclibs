#ifndef AOCLIBS_FS_CHECK_FILETYPE_H
#define AOCLIBS_FS_CHECK_FILETYPE_H

#include <a_fs_types.h>

/*
 * Stat the file and return its type
*/
enum FileType check_filetype(const char *path);

#endif
