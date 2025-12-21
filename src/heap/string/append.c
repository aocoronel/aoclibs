#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/mem/str.h>
#include <string.h>

Err string_append(String *_Nonnull s, const char *_Nonnull str, const usize str_len) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");
        ASSERT(str_len > 0, "%s", "string length \"%d\" cannot be negative",
               str_len);

        usize add_len = str_len;
        usize needed = s->length + add_len + 1;

        if (needed > s->heap.cap) {
                Err err = string_realloc(s, needed * 2);
                if (err.code != Ok) return err;
        }
        memcpy((char *)s->heap.ptr + s->length, str, add_len);
        ((char *)s->heap.ptr)[s->length + add_len] = '\0';
        s->length += add_len;
        return ok();
}

Err string_string_append(String *_Nonnull s1, const String *_Nonnull s2) {
        ASSERT(s1 != NULL && s2 != NULL, "%s",
               "passing NULL pointer to Nonnull parameter");
        ASSERT(s1->heap.ptr != NULL && s2->heap.ptr != NULL, "%s",
               "string heap pointer is NULL");

        usize add_len = s2->length;
        usize needed = s1->length + add_len + 1;

        if (needed > s1->heap.cap) {
                Err err = string_realloc(s1, needed * 2);
                if (err.code != Ok) return err;
        }

        memcpy((char *)s1->heap.ptr + s1->length, s2->heap.ptr, add_len);
        ((char *)s1->heap.ptr)[s1->length + add_len] = '\0';
        s1->length += add_len;

        return ok();
}
