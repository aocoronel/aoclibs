#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>

// === Flush ===

static Err flush_shrink_to_fit(String *_Nonnull s) {
        const usize NEEDED = s->length + 1;
        if (NEEDED < s->heap.cap) {
                Err err = heap_realloc(&s->heap, NEEDED);
                if (err.code != Ok) return err;
        }
        return ok();
}

Err string_flush(String *_Nonnull s) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");

        string_trim_trailing(s);
        return flush_shrink_to_fit(s);
}

// END === Flush ===

void string_clear(String *_Nonnull s) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");

        ((char *)s->heap.ptr)[0] = '\0';
        s->length = 0;
}

void _string_garbage(String *_Nonnull s, usize len) {
        ASSERT(s != NULL, "%s", "passing NULL pointer to Nonnull parameter");
        ASSERT(s->heap.ptr != NULL, "%s", "string heap pointer is NULL");

        volatile unsigned char *p = s->heap.ptr;
        while (len--)
                *p++ = 0;
}
