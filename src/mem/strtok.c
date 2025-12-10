#include <aoclibs/common.h>
#include <aoclibs/mem/str.h>
#include <assert.h>
#include <stddef.h>

ei32 _strtok(const char *s, const char c, const usize s_len) {
        assert(s != NULL);

        for (i32 i = 0; i < s_len; i++) {
                if (s[i] == c) return (ei32){ .err = ok(), .value = i };
        }
        return (ei32){ .err = werr(-1, "character not found"), .value = 0 };
}

ei32 _strrtok(const char *s, const char c, const usize s_len) {
        assert(s != NULL);

        for (i32 i = s_len; i > 0; i--) {
                if (s[i] == c) return (ei32){ .err = ok(), .value = i };
        }
        return (ei32){ .err = werr(-1, "character not found"), .value = 0 };
}
