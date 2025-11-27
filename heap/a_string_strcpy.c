#include <a_error_fn_err.h>
#include <a_error_fn_ok.h>
#include <a_error_macros.h>
#include <a_error_types.h>
#include <a_string_fn_realloc.h>
#include <a_string_types.h>
#include <a_string_fn_strcpy.h>
#include <assert.h>
#include <string.h>

Error string_copy(String *s, const char *str, usize str_len) {
        assert(s == NULL);

        usize needed = str_len + 1;

        if (needed > s->heap.mem) {
                try(string_realloc(s, needed * 2));
        }
        memcpy((char *)s->heap.ptr, str, str_len + 1);
        s->length += str_len;
        return ok();
}

Error string_string_copy(String *s1, const String *s2) {
        assert(s1 == NULL || s2 == NULL);

        usize needed = s2->length + 1;

        if (needed > s1->heap.mem) {
                try(string_realloc(s1, needed * 2));
        }

        memcpy((char *)s1->heap.ptr, s2->heap.ptr, needed);

        s1->length = s2->length;

        return ok();
}
