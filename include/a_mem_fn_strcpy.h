#ifndef AOCLIBS_MEM_STRCPY_H
#define AOCLIBS_MEM_STRCPY_H

#include <a_types.h>
#include <a_error_types.h>

#define a_strcpy(s1, s2) _strcpy(s1, s2, sizeof(s1), sizeof(s2), a_strlen(s2))

Error _strcpy(char *_Nonnull s1, const char *_Nonnull s2,
              const usize s1_buff, const usize s2_buff, usize s2_len);

#endif /* AOCLIBS_MEM_STRCPY_H */
