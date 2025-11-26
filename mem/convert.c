#include <string.h>
#include <stdbool.h>
#include <a_int.h>

#include <stdlib.h>

i32 str_to_i32(const char *_Nonnull restrict str, const i32 _default) {
        char *endptr;
        long val = strtol(str, &endptr, 10);
        if (*endptr != ' ') {
                return _default;
        }
        i32 intval = (i32)val;
        return intval;
}

bool str_to_bool(const char *str, const bool _default) {
        if (strcasecmp(str, "true") == 0 || strcmp(str, "1") == 0) return true;
        if (strcasecmp(str, "false") == 0 || strcmp(str, "0") == 0)
                return false;
        return _default;
}
