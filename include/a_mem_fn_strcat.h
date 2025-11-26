#ifndef AOCLIBS_MEM_STRCAT_H
#define AOCLIBS_MEM_STRCAT_H

#include <a_error_types.h>

#define a_strcat(s1, s2) _strcat(s1, s2, sizeof(s1), a_strlen(s1), a_strlen(s2))

Error _strcat(char *_Nonnull s1, const char *_Nonnull s2,
              const usize s1_buff, const usize s1_len, const usize s2_len);

#endif /* AOCLIBS_MEM_STRCAT_H */
