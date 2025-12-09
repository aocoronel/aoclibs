#ifndef AOCLIBS_FS_FILE_H
#define AOCLIBS_FS_FILE_H

#include <aoclibs/fs/types.h>

/*
 * Stat the file and return its type
*/
FileType file_check_ft(const char *path);

#endif
