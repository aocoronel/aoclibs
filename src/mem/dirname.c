#include <aoclibs/common.h>
#include <aoclibs/mem/path.h>

char *_mem_dirname(char *_Nonnull mut s, usize s_len) {
        usize i = s_len - 1;
        if (!s || !*s) return ".";

        while (s[i] == '/') {
                if (!i) return "/";
                i--;
        }
        while (s[i] != '/') {
                if (!i) return ".";
                i--;
        }
        while (s[i] == '/') {
                if (!i) return "/";
                i--;
        }

        s[i + 1] = 0;

        return s;
}
