#ifndef AOCLIBS_MEM_STR_H
#define AOCLIBS_MEM_STR_H

#include <aoclibs/common.h>

/* === Macros === */

#define mem_strlen(s) _mem_strlen(s, sizeof(s))

#define mem_strcat(s1, s2) \
        _mem_strcat(s1, s2, sizeof(s1), mem_strlen(s1), mem_strlen(s2))

#define mem_strcpy(s1, s2) _mem_strcpy(s1, s2, sizeof(s1), mem_strlen(s2))

#define mem_streq(s1, s2) _mem_streq(s1, s2, mem_strlen(s1), mem_strlen(s2))
#define mem_strreq(s1, s2) _mem_strreq(s1, s2, mem_strlen(s1), mem_strlen(s2))

#define mem_strfind(s, c) _mem_strfind(s, c, mem_strlen(s))
#define mem_strrfind(s, c) _mem_strrfind(s, c, mem_strlen(s))

/* === Functions === */

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

#ifdef TUNIT
TEST(_mem_strlen) {
        char str1[128] = "Hello, world!";
        i32 str_len = mem_strlen(str1);

        TASSERT(str_len == 13, "Hello, world! should be 13 bytes long");
}
#endif

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

#ifdef TUNIT
TEST(_mem_streq) {
        char str1[128] = "Hello, world!";
        char str2[86] = "Hello, world!";
        i32 result = mem_streq(str1, str2);

        TASSERT(result == 0, "strings must be equal");
}
#endif

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
bool _mem_strreq(const char *_Nonnull s1, const char *_Nonnull s2,
                 const usize s1_len, const usize s2_len);

#ifdef TUNIT
TEST(_mem_strreq) {
        char str1[128] = "Hello, world!";
        char str2[86] = "Hello, world!";
        i32 result = mem_strreq(str1, str2);

        TASSERT(result == 0, "strings must be equal");
}
#endif

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

#ifdef TUNIT
TEST(_mem_strcat) {
        char str1[128] = "Hello";
        char str2[86] = ", world!";
        mem_strcat(str1, str2);

        TASSERT(mem_strlen(str1) == 5 + 8,
                "Hello, world! must be 13 bytes long");
        TASSERT(str1[mem_strlen(str1)] == '\0',
                "str1 should be null terminated");

        TASSERT(mem_streq(str1, "Hello, world!") == 0, "strings must be equal");
}
#endif

/*
 * Copies the characters from string (s2) to string (s1)
 *
 * Defines:
 * - NDEBUG :: Disables assertions
 *
 * Failure: ErrBufferTooSmall
*/
Err _mem_strcpy(char *_Nonnull mut s1, const char *_Nonnull s2,
                const usize s1_buff, usize s2_len);

#ifdef TUNIT
TEST(mem_strcpy) {
        char str1[128] = "Hello";
        char str2[86] = ", world!";
        mem_strcpy(str1, str2);

        TASSERT(mem_strlen(str1) == mem_strlen(str2),
                "str1 should be exactly like str2");

        TASSERT(mem_streq(str1, ", world!") == 0, "str1 should be ', world!'");
}
#endif

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

#ifdef TUNIT
TEST(_mem_strfind) {
        char str1[128] = "Hello, world";

        ei32 pos = mem_strfind(str1, ',');
        TASSERT(pos.value == 5, "the ',' should be at the 5th index");
}
#endif

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

#ifdef TUNIT
TEST(_mem_strrfind) {
        char str1[128] = "Hello, world";

        ei32 pos = mem_strrfind(str1, ',');
        TASSERT(pos.value == 5, "the ',' should be at the 5th index");
}
#endif

#endif /* AOCLIBS_STR_H */
