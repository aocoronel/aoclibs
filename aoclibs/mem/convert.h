#ifndef AOCLIBS_MEM_CONVERT_H
#define AOCLIBS_MEM_CONVERT_H

#include <aoclibs/int.h>
#include <stdbool.h>

/*
 * Converts given string to integer. Defaults to _default.
*/
i64 convert_strtol(const char *str, const i32 _default);

/*
 * Converts given string to float. Defaults to _default.
*/
f32 convert_strtof(const char *str, const f32 _default);

/*
 * Converts given string to double. Defaults to _default.
*/
f64 convert_strtod(const char *str, const f64 _default);

/*
 * Converts given string to bool. Defaults to _default.
*/
bool convert_strtob(const char *str, const bool _default);

#endif
