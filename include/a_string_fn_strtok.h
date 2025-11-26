#ifndef AOCLIBS_STRING_STRTOK_H
#define AOCLIBS_STRING_STRTOK_H

#include <a_heap_main.h>
#include <a_error_main.h>
#include <a_string_types.h>

#ifdef AOCLIBS_STRING_MACROS_H
#define str_chr(s1, s2) string_find((s1), (s2))
#define str_rchr(s1, s2) string_reverse_find((s1), (s2))
#endif

/*
 * Finds the first occurance of c in the String.
 * Return Values:
 * > 0 - returns the position of c.
 * -1 - the String does not exist, or c hasn't been found.
*/
usizeerr string_find(String *s, const char c);

/*
 * Finds the last occurance of c in the String.
 * Return Values:
 * > 0 - returns the position of c.
 * -1 - the String does not exist, or c hasn't been found.
*/
usizeerr string_reverse_find(String *s, const char c);

#endif // AOCLIBS_STRING_STRTOK_H
