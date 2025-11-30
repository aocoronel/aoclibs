#ifndef AOCLIBS_STRING_FIND_H
#define AOCLIBS_STRING_FIND_H

#include <a_error_main.h>
#include <a_string_types.h>

/*
 * Finds the first occurance of c in the String.
 *
 * Defines:
 *
 * - NDEBUG: Disable non null s assert.
 *
 * Values:
 *
 * - > 0 - returns the position of c.
 *
 * - 0 - c hasn't been found.
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: ERROR_FAIL
 */
usizeerr string_find(String *s, const char c);

/*
 * Finds the last occurance of c in the String.
 *
 * Defines:
 *
 * - NDEBUG: Disable non null s assert.
 *
 * Values:
 *
 * - > 0 - returns the position of c.
 *
 * - 0 - c hasn't been found.
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: ERROR_FAIL
 */
usizeerr string_reverse_find(String *s, const char c);

#endif // AOCLIBS_STRING_FIND_H
