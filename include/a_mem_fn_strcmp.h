#ifndef AOCLIBS_MEM_STRCMP_H
#define AOCLIBS_MEM_STRCMP_H

#include <a_types.h>
#include <a_mem_fn_strlen.h>

#define a_strcmp(s1, s2) _strcmp(s1, s2, a_strlen(s1), a_strlen(s2))
#define a_strrcmp(s1, s2) _strrcmp(s1, s2, a_strlen(s1), a_strlen(s2))

/*
 * Compares both strings and return its difference
 *
 * Return Values:
 * 0 :: Strings are identical
*/
i32 _strcmp(const char *s1, const char *s2, const usize s1_len,
            const usize s2_len);

/*
 * Compares both strings in reverse and return its difference.
 *
 * Return Values:
 * 0 :: Strings are identical
*/
i32 _strrcmp(const char *s1, const char *s2, const usize s1_len,
             const usize s2_len);

#endif /* AOCLIBS_MEM_STRCMP_H */
