#include <aoclibs/common.h>
#include <aoclibs/mem/convert.h>
#include <aoclibs/mem/str.h>
#include <stdlib.h>

f32 mem_strtof(const char *_Nonnull s, const f32 _default) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        char *endptr;
        f32 val = strtof(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}
