#ifndef AOCLIBS_FS_FILE_H
#define AOCLIBS_FS_FILE_H

#include <aoclibs/fs/types.h>

/*
 * Stat the file and return its type
 *
 * Defines:
 * - NDEBUG :: Disable asserts
*/
FileType file_check_ft(const char *_Nonnull path);

#endif
