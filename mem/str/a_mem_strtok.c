#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_error_types.h>
#include <a_mem_fn_strtok.h>
#include <assert.h>
#include <stdio.h>

i32err _strtok(const char *_Nonnull s, const char c,
               const usize s_len) {
        assert(s != NULL);

        for (i32 i = 0; i < s_len; i++) {
                if (s[i] == c) return (i32err){ .err = ok(), .value = i };
        }
        return (i32err){ .err = err(-1, "character not found"), .value = 0 };
}

i32err _strrtok(const char *_Nonnull s, const char c,
                const usize s_len) {
        assert(s != NULL);

        for (i32 i = s_len; i > 0; i--) {
                if (s[i] == c) return (i32err){ .err = ok(), .value = i };
        }
        return (i32err){ .err = err(-1, "character not found"), .value = 0 };
}
