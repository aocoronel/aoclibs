#include <aoclibs/common.h>
#include <aoclibs/mem/convert.h>
#include <aoclibs/mem/str.h>
#include <stdlib.h>

i64 mem_strtol(const char *_Nonnull s, const i32 _default) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        char *endptr;
        i64 val = strtol(s, &endptr, 10);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}
