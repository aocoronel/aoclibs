#ifndef AOCLIBS_MEM_CONVERT_H
#define AOCLIBS_MEM_CONVERT_H

#include <aoclibs/int.h>

/*
 * Converts given string to integer. Defaults to _default.
 *
 * Defines:
 * - NDEBUG - Disables assertions
 *
 * Return Values:
 * - Success ::  return string as integer
 * - Failure :: return _default
*/
i64 mem_strtol(const char *_Nonnull s, const i32 _default);

/*
 * Converts given string to float. Defaults to _default.
 *
 * Defines:
 * - NDEBUG - Disables assertions
 *
 * Return Values:
 * - Success ::  return string as float
 * - Failure :: return _default
*/
f32 mem_strtof(const char *_Nonnull s, const f32 _default);

/*
 * Converts given string to double. Defaults to _default.
 *
 * Defines:
 * - NDEBUG - Disables assertions
 *
 * Return Values:
 * - Success ::  return string as double
 * - Failure :: return _default
*/
f64 mem_strtod(const char *_Nonnull s, const f64 _default);

/*
 * Converts given string to bool. Defaults to _default.
 *
 * Defines:
 * - NDEBUG - Disables assertions
 *
 * Return Values:
 * - Success ::  return string as bool
 * - Failure :: return _default
*/
bool mem_strtob(const char *_Nonnull s, const bool _default);

#endif
