#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>

usize _mem_strlen(const char *_Nonnull s, const usize s_buff) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        usize i = 0;
        while (i < s_buff && s[i] != '\0')
                i++;
        return i;
}
