#ifndef AOCLIBS_CONVERT_H
#define AOCLIBS_CONVERT_H

#include <a_types.h>
#include <stdbool.h>

isize str_to_int(const char *_Nonnull str, const i32 _default);

f64 str_to_float(const char *_Nonnull str, const f64 _default);

bool str_to_bool(const char *str, const bool _default);

#endif
