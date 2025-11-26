#ifndef AOCLIBS_STRING_STRCMP_H
#define AOCLIBS_STRING_STRCMP_H

#include <a_heap_main.h>
#include <a_error_main.h>
#include <a_string_types.h>

#ifdef AOCLIBS_STRING_MACROS_H
#define str_cmp(s1, s2) string_compare((s1), (s2))
#define sstr_cmp(s1, s2) string_string_compare((s1), (s2))
#endif

/*
 * Compares a String s to a string literal.
 * Return Values:
 * < 0 - String s is tinier than str
 * 0 - String is equal to str
 * > 0 - String is bigger than str
*/
int string_compare(String *s, const char *str);

/*
 * Compares a String s1 to a String s2
 * Return Values:
 * < 0 - String s1 is tinier than String s2
 * 0 - String s1 is equal to String s2
 * > 0 - String s1 is bigger than String s2
*/
int string_string_compare(const String *s1, const String *s2);

#endif // AOCLIBS_STRING_STRCMP_H
