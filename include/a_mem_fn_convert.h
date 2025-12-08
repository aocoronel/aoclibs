#ifndef AOCLIBS_MEM_CONVERT_H
#define AOCLIBS_MEM_CONVERT_H

#include <a_types.h>
#include <stdbool.h>

/*
 * Converts given string to integer. Defaults to _default.
*/
isize str_to_int(const char *str, const i32 _default);

/*
 * Converts given string to float. Defaults to _default.
*/
f32 str_to_float(const char *str, const f32 _default);

/*
 * Converts given string to double. Defaults to _default.
*/
f64 str_to_double(const char *str, const f64 _default);

/*
 * Converts given string to bool. Defaults to _default.
*/
bool str_to_bool(const char *str, const bool _default);

#endif
