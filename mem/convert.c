#include <a_mem.h>
#include <a_types.h>
#include <stdbool.h>
#include <stdlib.h>

isize str_to_int(const char *_Nonnull str, const i32 _default) {
        char *endptr;
        long val = strtol(str, &endptr, 10);
        if (*endptr != '\0') {
                return _default;
        }
        return (isize)val;
}

f64 str_to_float(const char *_Nonnull str, const f64 _default) {
        char *endptr;
        double val = strtof(str, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return (f64)val;
}

bool str_to_bool(const char *str, const bool _default) {
        if (a_strcmp(str, "true") == 0 || a_strcmp(str, "1") == 0) return true;
        if (a_strcmp(str, "false") == 0 || a_strcmp(str, "0") == 0)
                return false;
        return _default;
}
