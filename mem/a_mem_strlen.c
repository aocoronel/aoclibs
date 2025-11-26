#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_error_types.h>
#include <a_mem_fn_strlen.h>
#include <assert.h>
#include <stddef.h>

usize _strlen(const char *s, const usize buff) {
        assert(s != NULL);
        for (i32 i = 0; i < buff - 1; i++) {
                if (s[i] == '\0') return i;
        }
        return buff;
}
