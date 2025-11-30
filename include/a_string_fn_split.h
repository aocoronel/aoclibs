#ifndef AOCLIBS_STRING_SPLIT_H
#define AOCLIBS_STRING_SPLIT_H

#include <a_error_main.h>
#include <a_string_types.h>

/*
 * Splits a String from in the first occurance of a delimiter.
 *
 * out_s1 and out_s2 will allocate and are mutated to contain the split string
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
Error string_split(String *s, char delimiter, String *out_s1, String *out_s2);

/*
 * Splits a String from in the last occurance of a delimiter.
 *
 * out_s1 and out_s2 will allocate and are mutated to contain the split string
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
Error string_rsplit(String *s, char delimiter, String *out_s1, String *out_s2);

#endif // AOCLIBS_STRING_SPLIT_H
