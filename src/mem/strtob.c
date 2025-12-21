#include <aoclibs/common.h>
#include <aoclibs/mem/convert.h>
#include <aoclibs/mem/str.h>
#include <stdlib.h>

bool mem_strtob(const char *_Nonnull s, const bool _default) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        if (mem_streq(s, "true") == 0 || mem_streq(s, "1") == 0) return true;
        if (mem_streq(s, "false") == 0 || mem_streq(s, "0") == 0) return false;
        return _default;
}
