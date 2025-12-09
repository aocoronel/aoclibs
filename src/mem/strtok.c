#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <aoclibs/mem/str.h>
#include <assert.h>
#include <stddef.h>

i32_e _strtok(const char *s, const char c, const usize s_len) {
        assert(s != NULL);

        for (i32 i = 0; i < s_len; i++) {
                if (s[i] == c) return (i32_e){ .err = ok(), .value = i };
        }
        return (i32_e){ .err = err(-1, "character not found"), .value = 0 };
}

i32_e _strrtok(const char *s, const char c, const usize s_len) {
        assert(s != NULL);

        for (i32 i = s_len; i > 0; i--) {
                if (s[i] == c) return (i32_e){ .err = ok(), .value = i };
        }
        return (i32_e){ .err = err(-1, "character not found"), .value = 0 };
}
