#ifndef AOCLIBS_STRING_CREATE_H
#define AOCLIBS_STRING_CREATE_H

#include <a_error_main.h>
#include <a_string_types.h>

#define a_string_from(s) _string_from(s, sizeof(s))

/*
 * Allocates an empty String.
 *
 * If capacity is 0, sets capacity to 16.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints.
 * - DEBUG_HEAP: Enable a_debug_heap.h
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
stringerr string_create(usize cap);

/*
 * Allocates a String from a string literal.
 *
 * The capacity is inferred by the string length.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null str assert.
 * - DEBUG_HEAP: Enable a_debug_heap.h
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
stringerr _string_from(const char *str, usize len);

/*
 * Frees heap allocated String and HeapPtr.ptr.
 *
 * Frees given HeapPtr.
 *
 * The memory and length are set to 0, and pointer is set to NULL.
 *
 * If h or h.ptr is NULL, returns earlier
 *
 * Defines:
 *
 * - DEBUG_HEAP: Enable a_debug_heap.h
 */
void string_free(String *s);

#endif // AOCLIBS_STRING_CREATE_H
