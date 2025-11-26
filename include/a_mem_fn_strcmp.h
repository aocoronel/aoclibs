#ifndef AOCLIBS_MEM_STRCMP_H
#define AOCLIBS_MEM_STRCMP_H

#include <a_types.h>
#include <a_mem_fn_strlen.h>

#define a_strcmp(s1, s2) _strcmp(s1, s2, a_strlen(s1), a_strlen(s2))
#define a_strrcmp(s1, s2) _strrcmp(s1, s2, a_strlen(s1), a_strlen(s2))

i32 _strcmp(const char *_Nonnull s1, const char *_Nonnull s2,
            const usize s1_len, const usize s2_len);

i32 _strrcmp(const char *_Nonnull s1, const char *_Nonnull s2,
             const usize s1_len, const usize s2_len);

#endif /* AOCLIBS_MEM_STRCMP_H */
