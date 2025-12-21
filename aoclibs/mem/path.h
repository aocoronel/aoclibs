#ifndef AOCLIBS_FS_PATH_H
#define AOCLIBS_FS_PATH_H

#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>

#define mem_basename(s) _mem_basename(s, mem_strlen(s))
#define mem_dirname(s) _mem_dirname(s, mem_strlen(s))

/*
 * Returns the basename from a given path (s)
*/
char *_mem_basename(char *_Nonnull s, usize s_len);

/*
 * Returns the dirname from a given path (s)
*/
char *_mem_dirname(char *_Nonnull mut s, usize s_len);

#endif
