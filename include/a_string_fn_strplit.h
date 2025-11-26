#ifndef AOCLIBS_STRING_STRSPLIT_H
#define AOCLIBS_STRING_STRSPLIT_H

#include <a_heap_main.h>
#include <a_error_main.h>
#include <a_string_types.h>

/*
 * Splits a String from in the first occurance of a delimiter.
 * Both strings are overwritten, and may cause
 * memory leaks.
 * Will allocate Strings in the heap and return to out_s1 and out_s2, potentially being NULL.
 * Check out_s1 and out_s2 to NULL to prevent dereferencing a NULL pointer.
 * Return Values:
 * 0 - Strings out_s1 and out_s2 may contain the split String s.
 * -1 - String does not exist.
*/
Error string_split(String *s, char delimiter, String *out_s1, String *out_s2);

/*
 * Splits a String from in the last occurance of a delimiter.
 * Both strings are overwritten, and may cause
 * memory leaks.
 * Will allocate Strings in the heap and return to out_s1 and out_s2, potentially being NULL.
 * Check out_s1 and out_s2 to NULL to prevent dereferencing a NULL pointer.
 * Return Values:
 * 0 - Strings out_s1 and out_s2 may contain the split String s.
 * -1 - String does not exist.
*/
Error string_rsplit(String *s, char delimiter, String *out_s1, String *out_s2);

#endif // AOCLIBS_STRING_STRSPLIT_H
