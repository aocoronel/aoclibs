#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <string.h>

Err _string_copy(String *_Nonnull s, const char *_Nonnull str, usize str_len) {
        ASSERT(s != NULL && str != NULL, "%s",
               "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");
        ASSERT(str_len > 0, "%s", "string length \"%d\" cannot be negative",
               str_len);

        const usize NEEDED = str_len + 1;

        if (NEEDED > s->heap.cap) {
                Err err = string_realloc(s, NEEDED * 2);
                if (err.code != Ok) return err;
        }
        memcpy((char *)s->heap.ptr, str, str_len + 1);
        s->length += str_len;
        return ok();
}

Err string_string_copy(String *_Nonnull s1, const String *_Nonnull s2) {
        ASSERT(s1 != NULL && s2 != NULL, "%s",
               "passing NULL pointer to Nonnull parameter");
        ASSERT(s1->heap.ptr != NULL && s2->heap.ptr != NULL, "%s",
               "string heap pointer is NULL");

        const usize NEEDED = s2->length + 1;

        if (NEEDED > s1->heap.cap) {
                Err err = string_realloc(s1, NEEDED * 2);
                if (err.code != Ok) return err;
        }

        memcpy((char *)s1->heap.ptr, s2->heap.ptr, NEEDED);

        s1->length = s2->length;

        return ok();
}
