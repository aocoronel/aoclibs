#include <aoclibs/common.h>
#include <aoclibs/heap/string.h>
#include <aoclibs/error.h>
#include <aoclibs/int.h>
#include <assert.h>

// === Flush ===

static Err flush_shrink_to_fit(String *s) {
        const usize NEEDED = s->length + 1;
        if (NEEDED < s->heap.cap) {
                Err err = heap_realloc(&s->heap, NEEDED);
                if (err.code != Ok) return err;
        }
        return ok();
}

Err string_flush(String *s) {
        assert(!s || !s->heap.ptr);

        string_trim_trailing(s);
        return flush_shrink_to_fit(s);
}

// END === Flush ===

void string_clear(String *s) {
        assert(!s || !s->heap.ptr);
        ((char *)s->heap.ptr)[0] = '\0';
        s->length = 0;
}

void string_garbage(String *s, usize len) {
        assert(!s->heap.ptr);
        volatile unsigned char *p = s->heap.ptr;
        while (len--)
                *p++ = 0;
}
