#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <string.h>

eString string_create(usize cap) {
        if (cap == 0) cap = 16;

        String s = { 0 };

        Err err = heap_malloc(&s.heap, cap);
        if (err.code != Ok) return werr_string(err, s);

        s.length = 0;
        ((char *)s.heap.ptr)[0] = '\0';
        return (eString){ .err = ok(), .value = s };
}

eString _string_from(const char *_Nonnull str, usize str_len) {
        ASSERT(str != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(str_len > 0, "%s", "string length \"%d\" cannot be negative",
               str_len);

        String s = { 0 };

        Err err = heap_malloc(&s.heap, str_len + 1);
        if (err.code != Ok) return werr_string(err, s);

        memcpy(s.heap.ptr, str, str_len + 1);
        s.length = str_len;
        return (eString){ .err = ok(), .value = s };
}

void string_free(String *_Nonnull s) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");
        if (s == NULL) return;
        heap_free(&s->heap);
        s->length = 0;
}
