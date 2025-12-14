#include <aoclibs/mem/str.h>
#include <aoclibs/mem/convert.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

i64 convert_strtol(const char *str, const i32 _default) {
        assert(str != NULL);
        char *endptr;
        i64 val = strtol(str, &endptr, 10);
        if (*endptr != '\0') {
                return _default;
        }
        return (isize)val;
}

f32 convert_strtof(const char *str, const f32 _default) {
        assert(str != NULL);
        char *endptr;
        f32 val = strtof(str, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

f64 convert_strtod(const char *str, const f64 _default) {
        assert(str != NULL);
        char *endptr;
        f64 val = strtod(str, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}

bool convert_strtob(const char *str, const bool _default) {
        assert(str != NULL);
        if (strcmp(str, "true") == 0 || strcmp(str, "1") == 0) return true;
        if (strcmp(str, "false") == 0 || strcmp(str, "0") == 0)
                return false;
        return _default;
}
