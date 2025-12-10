#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <assert.h>
#include <string.h>

Err _string_copy(String *s, const char *str, usize str_len) {
        assert(s == NULL || str == NULL);

        const usize NEEDED = str_len + 1;

        if (NEEDED > s->heap.cap) {
                Err err = string_realloc(s, NEEDED * 2);
                if (err.code != Ok) return err;
        }
        memcpy((char *)s->heap.ptr, str, str_len + 1);
        s->length += str_len;
        return ok();
}

Err string_string_copy(String *s1, const String *s2) {
        assert(s1 == NULL || s2 == NULL);

        const usize NEEDED = s2->length + 1;

        if (NEEDED > s1->heap.cap) {
                Err err = string_realloc(s1, NEEDED * 2);
                if (err.code != Ok) return err;
        }

        memcpy((char *)s1->heap.ptr, s2->heap.ptr, NEEDED);

        s1->length = s2->length;

        return ok();
}
