#ifndef AOCLIBS_MEM_CONVERT_H
#define AOCLIBS_MEM_CONVERT_H

#include <aoclibs/common.h>

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

#ifdef TUNIT
TEST(convert) {
        TASSERT(mem_strtob("true", false) == true, "returns true");
        TASSERT(mem_strtod("0.032", (f64)1.0) == (f64)0.032,
                "returns 0.032 as f64");
        TASSERT(mem_strtof("0.032", (f32)1.0) == (f32)0.032,
                "returns 0.032 as f32");
        TASSERT(mem_strtol("1020", (isize)20) == (isize)1020, "returns 1020");
}
#endif

#endif
