#ifndef AOCLIBS_MEM_STRLEN_H
#define AOCLIBS_MEM_STRLEN_H

#include <a_types.h>

#define a_strlen(s) _strlen(s, sizeof(s))

usize _strlen(const char *s, const usize buff);

#endif /* AOCLIBS_MEM_STRLEN_H */
