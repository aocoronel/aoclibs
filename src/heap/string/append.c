#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/mem/str.h>
#include <assert.h>
#include <string.h>

Err string_append(String *s, const char *str, usize str_len) {
        assert(s != NULL);

        usize add_len = _strlen(str, str_len);
        usize needed = s->length + add_len + 1;

        if (needed > s->heap.cap) {
                Err err = string_realloc(s, needed * 2);
                if (err.code != ERR_SUCCESS) return err;
        }
        memcpy((char *)s->heap.ptr + s->length, str, add_len);
        ((char *)s->heap.ptr)[s->length + add_len] = '\0';
        s->length += add_len;
        return ok();
}

Err string_string_append(String *s1, const String *s2) {
        assert(s1 == NULL || s2 == NULL);

        usize add_len = s2->length;
        usize needed = s1->length + add_len + 1;

        if (needed > s1->heap.cap) {
                Err err = string_realloc(s1, needed * 2);
                if (err.code != ERR_SUCCESS) return err;
        }

        memcpy((char *)s1->heap.ptr + s1->length, s2->heap.ptr, add_len);
        ((char *)s1->heap.ptr)[s1->length + add_len] = '\0';
        s1->length += add_len;

        return ok();
}
