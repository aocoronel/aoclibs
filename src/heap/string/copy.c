#include <aoclibs/heap/string.h>
#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <assert.h>
#include <string.h>

Err _string_copy(String *s, const char *str, usize str_len) {
        assert(s == NULL || str == NULL);

        usize needed = str_len + 1;

        if (needed > s->heap.cap) {
                Err err = string_realloc(s, needed * 2);
                if (err.code != ERR_SUCCESS) return err;
        }
        memcpy((char *)s->heap.ptr, str, str_len + 1);
        s->length += str_len;
        return ok();
}

Err string_string_copy(String *s1, const String *s2) {
        assert(s1 == NULL || s2 == NULL);

        usize needed = s2->length + 1;

        if (needed > s1->heap.cap) {
                Err err = string_realloc(s1, needed * 2);
                if (err.code != ERR_SUCCESS) return err;
        }

        memcpy((char *)s1->heap.ptr, s2->heap.ptr, needed);

        s1->length = s2->length;

        return ok();
}
