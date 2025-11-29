#include <a_mem_fn_strcmp.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

isize str_to_int(const char *str, const i32 _default) {
        assert(str != NULL);
        char *endptr;
        long val = strtol(str, &endptr, 10);
        if (*endptr != '\0') {
                return _default;
        }
        return (isize)val;
}

f64 str_to_float(const char *str, const f64 _default) {
        assert(str != NULL);
        char *endptr;
        double val = strtof(str, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return (f64)val;
}

bool str_to_bool(const char *str, const bool _default) {
        assert(str != NULL);
        if (a_strcmp(str, "true") == 0 || a_strcmp(str, "1") == 0) return true;
        if (a_strcmp(str, "false") == 0 || a_strcmp(str, "0") == 0)
                return false;
        return _default;
}
