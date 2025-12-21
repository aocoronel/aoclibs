#include <aoclibs/common.h>
#include <aoclibs/mem/convert.h>
#include <aoclibs/mem/str.h>
#include <stdlib.h>

f64 mem_strtod(const char *_Nonnull s, const f64 _default) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        char *endptr;
        f64 val = strtod(s, &endptr);
        if (*endptr != '\0') {
                return _default;
        }
        return val;
}
