#ifndef AOCLIBS_MEM_STRTOK_H
#define AOCLIBS_MEM_STRTOK_H

#include <a_error_types.h>

#define a_strtok(s, c) _strtok(s, c, a_strlen(s))
#define a_strrtok(s, c) _strrtok(s, c, a_strlen(s))

i32err _strtok(const char *_Nonnull s, const char c,
               const usize s_len);

i32err _strrtok(const char *_Nonnull s, const char c,
                const usize s_len);

#endif /* AOCLIBS_MEM_STRTOK_H */
