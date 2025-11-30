#ifndef AOCLIBS_STRING_COPY_H
#define AOCLIBS_STRING_COPY_H

#include <a_error_main.h>
#include <a_string_types.h>

#define a_string_copy(s1, s2) string_copy((s1), (s2), (sizeof(s2)))

/*
 * Copies a string literal str to String s.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
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
Error _string_copy(String *s, const char *str, usize str_len);

/*
 * Copies the contents of String s2 into String s1.
 * Replaces s1’s current contents entirely.
 * May reallocate s1 if necessary.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null s1 and s2 assert.
 * - DEBUG_HEAP: Enable a_debug_heap.h
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
Error string_string_copy(String *s1, const String *s2);

#endif // AOCLIBS_STRING_COPY_H
