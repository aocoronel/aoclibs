#ifndef AOCLIBS_STRING_MOVE_H
#define AOCLIBS_STRING_MOVE_H

#include <a_error_main.h>
#include <a_string_types.h>

/*
 * Inserts a character into the String's ending.
 *
 * May reallocate the string if necessary.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null s and str assert.
 * - DEBUG_HEAP: Enable a_debug_heap.h
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
Error string_push(String *s, char c);

/*
 * Removes a character from the String's ending.
 *
 * Values:
 * - 0 - Last character is removed from the String.
 * - -1 - Removing failed. The String does not exist.
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: ERROR_FAIL
 */
Error string_pop(String *s);

/*
 * Drops the last character from the String's ending, and resizes the string without reallocating.
 *
 * Values:
 * - 0 - Character is dropped from the String.
 * - -1 - Dropping failed. The String does not exist.
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: ERROR_FAIL
 */
Error string_drop(String *s, usize index);

#endif // AOCLIBS_STRING_MOVE_H
