#ifndef AOCLIBS_MEM_STR_H
#define AOCLIBS_MEM_STR_H

#include <aoclibs/common.h>

/* === Macros === */

#define mem_strlen(s) _mem_strlen(s, sizeof(s))

#define mem_strcat(s1, s2) _mem_strcat(s1, s2, sizeof(s1), mem_strlen(s1), mem_strlen(s2))

#define mem_strcpy(s1, s2) _mem_strcpy(s1, s2, sizeof(s1), mem_strlen(s2))

#define mem_streq(s1, s2) _mem_streq(s1, s2, mem_strlen(s1), mem_strlen(s2))
#define mem_strreq(s1, s2) _strreq(s1, s2, mem_strlen(s1), mem_strlen(s2))

#define mem_strfind(s, c) _mem_strfind(s, c, mem_strlen(s))
#define mem_strrfind(s, c) _mem_strrfind(s, c, mem_strlen(s))

/* === Functions === */

/*
 * Concatenate the characters from string (s2) to string (s1)
 *
 * Defines:
 * - NDEBUG :: Disables assertions
 *
 * Failure: ErrBufferTooSmall
*/
Err _mem_strcat(char *_Nonnull mut s1, const char *_Nonnull s2, usize s1_buff,
            usize s1_len, usize s2_len);

/*
 * Check if strings are equal.
 *
 * Defines:
 * - NDEBUG :: Disables assertions
 *
 * Return Values:
 * - true :: Strings are identical
 * - false :: Strings are different
*/
bool _mem_streq(const char *_Nonnull s1, const char *_Nonnull s2,
            const usize s1_len, const usize s2_len);

/*
 * Check if strings are equal in reverse.
 *
 * Defines:
 * - NDEBUG :: Disables assertions
 *
 * Return Values:
 * - true :: Strings are identical
 * - false :: Strings are different
*/
bool _strreq(const char *_Nonnull s1, const char *_Nonnull s2,
             const usize s1_len, const usize s2_len);

/*
 * Copies the characters from string (s2) to string (s1)
 *
 * Defines:
 * - NDEBUG :: Disables assertions
 *
 * Failure: ErrBufferTooSmall
*/
Err _mem_strcpy(char *_Nonnull mut s1, const char *_Nonnull s2, const usize s1_buff,
            usize s2_len);

/*
 * Returns the length of the string.
 *
 * Return Values:
 * - Success: returns string length. Doesn't include the NULL terminator
 * - Failure: returns the buffer size, as fallback value
 *
 * Defines:
 * - NDEBUG :: Disables assertions
*/
usize _mem_strlen(const char *_Nonnull s, const usize s_buff);

/*
 * Finds and returns the index of the character (c) from string (s).
 *
 * Defines:
 * - NDEBUG :: Disables assertions
 *
 * Return Values:
 * - Success: returns the index of char
 * - Fail: returns 0
 *
 * Failure: ErrCharNotFound
*/
ei32 _mem_strfind(const char *_Nonnull s, const char c, const usize s_len);

/*
 * Finds in reverse and returns the index of the character (c) from string (s).
 *
 * Defines:
 * - NDEBUG :: Disables assertions
 *
 * Return Values:
 * - Success: returns the index of char
 * - Fail: returns 0
 *
 * Failure: ErrCharNotFound
*/
ei32 _mem_strrfind(const char *_Nonnull s, const char c, const usize s_len);

#endif /* AOCLIBS_STR_H */
