#ifndef AOCLIBS_MEM_STRCPY_H
#define AOCLIBS_MEM_STRCPY_H

#include <a_types.h>
#include <a_mem_fn_strlen.h>
#include <a_error_types.h>

#define a_strcpy(s1, s2) _strcpy(s1, s2, sizeof(s1), a_strlen(s2))

Error _strcpy(char *s1, const char *s2, const usize s1_buff, usize s2_len);

#endif /* AOCLIBS_MEM_STRCPY_H */
