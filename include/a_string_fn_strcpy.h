#ifndef AOCLIBS_STRING_STRCPY_H
#define AOCLIBS_STRING_STRCPY_H

#include <a_heap_main.h>
#include <a_error_main.h>
#include <a_string_types.h>

#ifdef AOCLIBS_STRING_MACROS_H
#define str_cpy(s1, s2) string_copy((s1), (s2), (sizeof(s2)))
#define sstr_cpy(s1, s2) string_string_copy((s1), (s2))
#endif

/*
 * Copies a string literal str to String s.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
 * Return Values:
 *  0 - s1 now contains the contents str
 * -1 - the String is untouched. String may be NULL, or allocation failed.
 */
Error string_copy(String *s, const char *str, usize str_len);

/*
 * Copies the contents of String s2 into String s1.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
 * Return Values:
 *  0 - s1 now contains the contents of s2
 * -1 - the String is untouched. String may be NULL, or allocation failed.
 */
Error string_string_copy(String *s1, const String *s2);

#endif // AOCLIBS_STRING_STRCPY_H
