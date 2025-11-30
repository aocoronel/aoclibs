#ifndef AOCLIBS_STRING_REALLOC_H
#define AOCLIBS_STRING_REALLOC_H

#include <a_string_types.h>

/*
 * Reallocates the String to a new capacity.
 *
 * Defines:
 *
 * - NDEBUG: Disable debug prints. Disable non null h assert.
 * - DEBUG_HEAP: Enable a_debug_heap.h
 *
 * Error.code:
 *
 * - Success: ok()
 *
 * - Fail: errno
 */
Error string_realloc(String *s, usize capacity);

#endif // AOCLIBS_STRING_REALLOC_H
