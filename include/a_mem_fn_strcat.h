#ifndef AOCLIBS_MEM_STRCAT_H
#define AOCLIBS_MEM_STRCAT_H

#include <a_error_types.h>

#define a_strcat(s1, s2) _strcat(s1, s2, sizeof(s1), a_strlen(s1), a_strlen(s2))

/*
 * Concatenate the characters from s2 to s1
 *
 * Error Values:
 * Success: ok
 * Fail:
 * - -1 :: s1 buffer doesn't fit the contents of s2
*/
Error _strcat(char *s1, const char *s2, const usize s1_buff, const usize s1_len,
              const usize s2_len);

#endif /* AOCLIBS_MEM_STRCAT_H */
