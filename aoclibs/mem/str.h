#ifndef AOCLIBS_MEM_STR_H
#define AOCLIBS_MEM_STR_H

#include <aoclibs/int.h>
#include <aoclibs/error.h>

/* === Macros === */

#define a_strlen(s) _strlen(s, sizeof(s))

#define a_strcat(s1, s2) _strcat(s1, s2, sizeof(s1), a_strlen(s1), a_strlen(s2))

#define a_strcpy(s1, s2) _strcpy(s1, s2, sizeof(s1), a_strlen(s2))

#define a_strcmp(s1, s2) _strcmp(s1, s2, a_strlen(s1), a_strlen(s2))
#define a_strrcmp(s1, s2) _strrcmp(s1, s2, a_strlen(s1), a_strlen(s2))

#define a_strtok(s, c) _strtok(s, c, a_strlen(s))
#define a_strrtok(s, c) _strrtok(s, c, a_strlen(s))

/* === Functions === */

/*
 * Concatenate the characters from s2 to s1
 *
 * Err Values:
 * Success: ok
 * Fail:
 * - -1 :: s1 buffer doesn't fit the contents of s2
*/
Err _strcat(char *s1, const char *s2, const usize s1_buff, const usize s1_len,
            const usize s2_len);

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

/*
 * Copies the characters from s2 to s1
 *
 * Err Values:
 * Success: ok
 * Fail:
 * - -1 :: s1 buffer doesn't fit the contents of s2
*/
Err _strcpy(char *s1, const char *s2, const usize s1_buff, usize s2_len);

/*
 * Returns the length of the string.
*/
usize _strlen(const char *s, const usize buff);

/*
 * Finds and returns the index of the character
 *
 * Return Values:
 * Success: returns the index of char
 * Fail: returns 0
 *
 * Err Values:
 * Success: ok
 * Fail:
 * - -1 :: Couldn't find the character
*/
ei32 _strtok(const char *s, const char c, const usize s_len);

ei32 _strrtok(const char *s, const char c, const usize s_len);

#endif /* AOCLIBS_STR_H */
