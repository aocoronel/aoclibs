#ifndef AOCLIBS_STRING_CLEAR_H
#define AOCLIBS_STRING_CLEAR_H

#include <a_error_main.h>
#include <a_string_types.h>

#define a_string_garbage(s) _string_garbage(s, s.length)

/*
 * Clears a String's ending and reallocates to shrink memory.
 * A null-terminated character is set in the last whitespace.
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
Error string_flush(String *s);

/*
 * Clears the string content.
 * Sets a null-terminated character in the first index.
 *
 * Defines:
 *
 * - NDEBUG: Disable non null s assert.
*/
void string_clear(String *s);

/*
 * Clears the string content.
 * Sets all characters to 0.
 *
 * Defines:
 *
 * - NDEBUG: Disable non null ptr assert.
*/
void _string_garbage(String *s, usize len);

#endif // AOCLIBS_STRING_CLEAR_H
