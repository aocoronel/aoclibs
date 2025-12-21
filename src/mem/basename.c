#include <aoclibs/common.h>
#include <aoclibs/mem/path.h>

char *_mem_basename(char *_Nonnull s, usize s_len) {
        if (!s || !*s) return ".";
        usize i = s_len - 1;
        while (i && s[i] == '/') {
                s[i] = 0;
                i--;
        }
        while (i && s[i - 1] != '/') i--;
        return s + i;
}
