#ifndef AOCLIBS_STRING_STRCAT_H
#define AOCLIBS_STRING_STRCAT_H

#include <a_heap_main.h>
#include <a_error_main.h>
#include <a_string_types.h>

#ifdef AOCLIBS_STRING_MACROS_H
#define str_cat(s1, s2) string_append((s1), (s2), (sizeof(s2)))
#define sst_rcat(s1, s2) string_string_append((s1), (s2))
#endif

/*
 * Appends a string literal str to a String s.
 * May reallocate the String if necessary.
 * Return Values:
 * 0 - the String has the string literal appended.
 * -1 - the String is untouched. String may be NULL, or allocation failed.
*/
Error string_append(String *s, const char *str);

/*
 * Appends the contents of String s2 to String s1.
 * May reallocate s1 if necessary.
 * Return Values:
 *  0 - String s1 has String s2 appended.
 * -1 - the String is untouched. String may be NULL, or allocation failed.
 */
Error string_string_append(String *s1, const String *s2);

#endif // AOCLIBS_STRING_STRCAT_H
