#ifndef AOCLIBS_STRING_APPEND_H
#define AOCLIBS_STRING_APPEND_H

#include <a_error_main.h>
#include <a_string_types.h>

/*
 * Appends a string literal str to a String s.
 * May reallocate the String if necessary.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null ptr assert.
 * - DEBUG_HEAP: Enable a_debug_heap.h
 *
 * Error.codes:
 * - Success: ok()
 * - Fail: errno
*/
Error string_append(String *s, const char *str);

/*
 * Appends the contents of String s2 to String s1.
 * May reallocate s1 if necessary.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null ptr assert.
 * - DEBUG_HEAP: Enable a_debug_heap.h
 *
 * Error.codes:
 * - Success: ok()
 * - Fail: errno
*/
Error string_string_append(String *s1, const String *s2);

#endif // AOCLIBS_STRING_APPEND_H
